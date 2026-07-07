// Copyright Epic Games, Inc. All Rights Reserved.


#include "StrategyUnit.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SphereComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "Engine/OverlapResult.h"

AStrategyUnit::AStrategyUnit()
{
	PrimaryActorTick.bCanEverTick = true;

	// 确保单位有有效的 AI 控制器来处理移动请求
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// 创建交互范围球体
	InteractionRange = CreateDefaultSubobject<USphereComponent>(TEXT("Interaction Range"));
	InteractionRange->SetupAttachment(RootComponent);

	InteractionRange->SetSphereRadius(100.0f);
	InteractionRange->SetCollisionProfileName(FName("OverlapAllDynamic"));

	// 配置角色移动组件
	GetCharacterMovement()->GravityScale = 1.5f;
	GetCharacterMovement()->MaxAcceleration = 1000.0f;
	GetCharacterMovement()->BrakingFrictionFactor = 1.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1000.0f;
	GetCharacterMovement()->PerchRadiusThreshold = 20.0f;
	GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 640.0f, 0.0f);
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->AvoidanceConsiderationRadius = 150.0f;
	GetCharacterMovement()->AvoidanceWeight = 1.0f;
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	GetCharacterMovement()->SetFixedBrakingDistance(200.0f);
	GetCharacterMovement()->SetFixedBrakingDistance(true);
}

void AStrategyUnit::NotifyControllerChanged()
{
	// 验证并保存 AI 控制器引用
	AIController = Cast<AAIController>(Controller);

	if (AIController)
	{
		// 订阅路径跟随组件的移动完成回调
		UPathFollowingComponent* PFComp = AIController->GetPathFollowingComponent();
		if (PFComp)
		{
			PFComp->OnRequestFinished.AddUObject(this, &AStrategyUnit::OnMoveFinished);
		}
	}
}

void AStrategyUnit::StopMoving()
{
	// 使用角色移动组件立即停止移动
	GetCharacterMovement()->StopMovementImmediately();

	// 停止单位的交互动画
	BP_StopAnimation();
}

void AStrategyUnit::UnitSelected()
{
	// 交由蓝图处理
	BP_UnitSelected();
}

void AStrategyUnit::UnitDeselected()
{
	// 交由蓝图处理
	BP_UnitDeselected();
}

void AStrategyUnit::Interact(AStrategyUnit* Interactor)
{
	// 确保交互者有效
	if (IsValid(Interactor))
	{
		// 旋转自身面向交互者
		SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Interactor->GetActorLocation()));

		// 通知交互者播放交互行为
		Interactor->BP_InteractionBehavior(this);

		// 播放自身的交互行为
		BP_InteractionBehavior(Interactor);
	}

}

void AStrategyUnit::MoveToLocation(const FVector& Location, bool bInteract, const TArray<AStrategyUnit*> IgnoreList)
{
	// 缓存移动和交互参数
	CurrentMovementGoal = Location;
	bInteractOnArrival = bInteract;
	InteractIgnoreList = IgnoreList;

	// 停止当前移动和动画
	StopMoving();

	// 根据是否需要交互选择合适的 EQS 查询
	UEnvQuery* MoveQuery = bInteractOnArrival ? InteractionQuery : NoInteractionQuery;

	// 选择运行模式：主要交互单位取最近结果，其余单位从最佳 25% 中随机选取
	TEnumAsByte<EEnvQueryRunMode::Type> RunMode = bInteractOnArrival ? EEnvQueryRunMode::SingleResult : EEnvQueryRunMode::RandomBest25Pct;

	// 运行 EQS 查询，基于 NavMesh 解析移动目标
	EnvQueryInstance = UEnvQueryManager::RunEQSQuery(this, MoveQuery, this,  RunMode, UEnvQueryInstanceBlueprintWrapper::StaticClass());

	if (IsValid(EnvQueryInstance))
	{
		EnvQueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &AStrategyUnit::OnEQSFinished);
	}
}

FVector AStrategyUnit::GetMovementGoal() const
{
	return CurrentMovementGoal;
}

void AStrategyUnit::OnEQSFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	// EQS 查询是否成功？
	if (QueryInstance)
	{
		// 获取查询结果位置
		TArray<FVector> ResultLocations;

		if(QueryInstance->GetQueryResultsAsLocations(ResultLocations))
		{
			// 取最佳结果
			CurrentMovementGoal = ResultLocations[0];

			// 确保有有效的 AI 控制器
			if (AIController)
			{
				// 构建 AI 移动请求
				FAIMoveRequest MoveReq;

				MoveReq.SetGoalLocation(CurrentMovementGoal);
				MoveReq.SetAcceptanceRadius(MovementAcceptanceRadius);
				MoveReq.SetAllowPartialPath(true);
				MoveReq.SetUsePathfinding(true);
				MoveReq.SetProjectGoalLocation(true);
				MoveReq.SetRequireNavigableEndLocation(true);
				MoveReq.SetNavigationFilter(AIController->GetDefaultNavigationFilterClass());
				MoveReq.SetCanStrafe(false);

				// 向 AI 控制器发出移动请求
				FNavPathSharedPtr FollowedPath;
				const FPathFollowingRequestResult ResultData = AIController->MoveTo(MoveReq, &FollowedPath);

				// 检查是否已经到达目标
				if(ResultData.Code == EPathFollowingRequestResult::AlreadyAtGoal)
				{
					// 立即完成移动
					HandleMoveFinished();
				}
			}
		}
	}
}

void AStrategyUnit::OnMoveFinished(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	HandleMoveFinished();
}

void AStrategyUnit::HandleMoveFinished()
{
	// 广播移动完成委托
	OnMoveCompleted.Broadcast(this);

	if (bInteractOnArrival)
	{
		// 执行重叠检测，寻找附近可交互的物体
		TArray<FOverlapResult> OutOverlaps;

		FCollisionShape CollisionSphere;
		CollisionSphere.SetSphere(InteractionRadius);

		FCollisionObjectQueryParams ObjectParams;
		ObjectParams.AddObjectTypesToQuery(ECC_WorldDynamic);

		FCollisionQueryParams QueryParams;

		// 将自身加入忽略列表
		QueryParams.AddIgnoredActor(this);

		for (const AActor* Current : InteractIgnoreList)
		{
			QueryParams.AddIgnoredActor(Current);
		}

		if (GetWorld()->OverlapMultiByObjectType(OutOverlaps, GetActorLocation(), FQuat::Identity, ObjectParams, CollisionSphere, QueryParams))
		{
			// 找到重叠到的第一个单位并与之交互
			for (const FOverlapResult& CurrentOverlap : OutOverlaps)
			{
				if (AStrategyUnit* CurrentUnit = Cast<AStrategyUnit>(CurrentOverlap.GetActor()))
				{
					CurrentUnit->Interact(this);
					return;
				}
			}
		}
	}
}
