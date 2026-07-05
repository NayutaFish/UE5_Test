// Copyright Epic Games, Inc. All Rights Reserved.

#include "Test_GamePlayPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"     // → SimpleMoveToLocation（AI寻路辅助）
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"                  // → SpawnSystemAtLocation（生成粒子特效）
#include "Test_GamePlayCharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"                  // → BindAction（绑定输入动作）
#include "Navigation/PathFollowingComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"                 // → AddMappingContext
#include "Engine/LocalPlayer.h"
#include "Test_GamePlay.h"

// ─────────────────── 构造函数 ───────────────────
// 类比 Unity：Awake()
// 注意：此时场景还没加载完，不要调用其他 Actor 或 Spawn 对象
ATest_GamePlayPlayerController::ATest_GamePlayPlayerController()
{
	bIsTouch = false;
	bMoveToMouseCursor = false;

	// 创建导航寻路组件（类似 Unity 的 NavMeshAgent，但这是在 Controller 端控制的）
	PathFollowingComponent = CreateDefaultSubobject<UPathFollowingComponent>(TEXT("Path Following Component"));

	// 显示鼠标光标
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
}

// ─────────────────── 绑定输入 ───────────────────
// 类比 Unity：在 Start() 里写 Input System 的事件绑定
// SetupInputComponent 是 APlayerController 的生命周期函数，自动被调用
void ATest_GamePlayPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// 只在本地玩家控制器上设置输入（联网时服务器不需要）
	if (IsLocalPlayerController())
	{
		// 加载输入映射上下文（相当于 Unity Input System 的 Input Action Asset 激活）
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}

		// 绑定输入动作到函数
		// BindAction(InputAction, 触发事件, 对象, 回调函数)
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
		{
			// 鼠标点击
			EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started,   this, &ATest_GamePlayPlayerController::OnInputStarted);
			EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &ATest_GamePlayPlayerController::OnSetDestinationTriggered);
			EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &ATest_GamePlayPlayerController::OnSetDestinationReleased);
			EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled,  this, &ATest_GamePlayPlayerController::OnSetDestinationReleased);

			// 触摸
			EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Started,   this, &ATest_GamePlayPlayerController::OnInputStarted);
			EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Triggered, this, &ATest_GamePlayPlayerController::OnTouchTriggered);
			EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Completed, this, &ATest_GamePlayPlayerController::OnTouchReleased);
			EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Canceled,  this, &ATest_GamePlayPlayerController::OnTouchReleased);
		}
		else
		{
			UE_LOG(LogTest_GamePlay, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
		}
	}
}

// ─────────────────── 输入事件处理 ───────────────────

/// 按下时：停止当前移动
void ATest_GamePlayPlayerController::OnInputStarted()
{
	StopMovement();                   // 停止 AI 寻路（中断之前的移动命令）
	UpdateCachedDestination();        // 缓存当前鼠标位置
}

/// 按住中：每帧触发，角色持续向鼠标方向移动
void ATest_GamePlayPlayerController::OnSetDestinationTriggered()
{
	FollowTime += GetWorld()->GetDeltaSeconds();  // 累计按住时长
	UpdateCachedDestination();                     // 每帧更新目标位置

	// 让角色朝鼠标方向移动
	// 类比 Unity：使用 CharacterController.Move() 向目标方向移动
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
		ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
	}
}

/// 松开时：短按触发寻路，长按不做事（因为按住期间已经走过去了）
void ATest_GamePlayPlayerController::OnSetDestinationReleased()
{
	// 短按 → 使用 NavMesh 寻路走到目标点
	// 类比 Unity：NavMeshAgent.SetDestination()
	if (FollowTime <= ShortPressThreshold)
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
		// 在目标位置播放粒子特效
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	}

	FollowTime = 0.f;  // 重置计时
}

// ─────────────────── 触摸输入 ───────────────────

void ATest_GamePlayPlayerController::OnTouchTriggered()
{
	bIsTouch = true;
	OnSetDestinationTriggered();  // 同鼠标逻辑
}

void ATest_GamePlayPlayerController::OnTouchReleased()
{
	bIsTouch = false;
	OnSetDestinationReleased();   // 同鼠标逻辑
}

// ─────────────────── 获取目标位置 ───────────────────

/// 从鼠标/触摸位置向世界发射射线，计算点击到的地点
/// 类比 Unity：Camera.ScreenPointToRay() + Physics.Raycast()
void ATest_GamePlayPlayerController::UpdateCachedDestination()
{
	FHitResult Hit;
	bool bHitSuccessful = false;

	if (bIsTouch)
	{
		// 触摸 → 从手指位置发射射线检测
		bHitSuccessful = GetHitResultUnderFinger(ETouchIndex::Touch1, ECollisionChannel::ECC_Visibility, true, Hit);
	}
	else
	{
		// 鼠标 → 从光标位置发射射线检测
		bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	}

	// 如果检测到了地面/物体，把目标位置设为点击点
	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
	}
}
