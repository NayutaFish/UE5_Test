// Copyright Epic Games, Inc. All Rights Reserved.


#include "TwinStickAoEAttack.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "TwinStickNPC.h"

// 构造函数：创建根组件、视觉网格体、碰撞球体，并绑定重叠事件
ATwinStickAoEAttack::ATwinStickAoEAttack()
{
 	PrimaryActorTick.bCanEverTick = true;

	// 创建根组件
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// 创建视觉效果网格体
	SphereVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sphere Visual"));
	SphereVisual->SetupAttachment(RootComponent);

	SphereVisual->SetCollisionProfileName(FName("NoCollision"));

	// 创建碰撞球体
	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere"));
	CollisionSphere->SetupAttachment(RootComponent);

	CollisionSphere->SetSphereRadius(750.0f);
	CollisionSphere->SetNotifyRigidBodyCollision(true);
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionSphere->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ATwinStickAoEAttack::OnAoEOverlap);
}

// 设置启动和停止 AoE 的定时器
void ATwinStickAoEAttack::BeginPlay()
{
	Super::BeginPlay();

	// 设置 AoE 定时器
	GetWorld()->GetTimerManager().SetTimer(StartAoETimer, this, &ATwinStickAoEAttack::StartAoE, StartAoETime, false);
	GetWorld()->GetTimerManager().SetTimer(StopAoETimer, this, &ATwinStickAoEAttack::StopAoE, StopAoETime, false);

}

void ATwinStickAoEAttack::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// 清除定时器
	GetWorld()->GetTimerManager().ClearTimer(StartAoETimer);
	GetWorld()->GetTimerManager().ClearTimer(StopAoETimer);
}

// 激活 AoE 并对当前重叠范围内所有 NPC 造成伤害
void ATwinStickAoEAttack::StartAoE()
{
	// 激活标志
	bIsAoEActive = true;

	// 查找所有重叠的 NPC
	TArray<AActor*> Overlaps;
	CollisionSphere->GetOverlappingActors(Overlaps, ATwinStickNPC::StaticClass());

	// 处理每个重叠 Actor
	for (AActor* Current : Overlaps)
	{
		if (ATwinStickNPC* NPC = Cast<ATwinStickNPC>(Current))
		{
			// 通知 NPC 被击中
			NPC->ProjectileImpact(FVector::ZeroVector);
		}
	}
}

// 停用 AoE 并通知蓝图处理淡出
void ATwinStickAoEAttack::StopAoE()
{
	// 关闭激活标志
	bIsAoEActive = false;

	// 停止伤害检测定时器
	GetWorld()->GetTimerManager().ClearTimer(StartAoETimer);

	// 调用蓝图处理。由蓝图负责销毁 Actor。
	BP_AoEFinished();
}

// AoE 激活期间处理新进入范围的 NPC
void ATwinStickAoEAttack::OnAoEOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// AoE 是否激活？
	if (bIsAoEActive)
	{
		// 是否重叠了 NPC？
		if (ATwinStickNPC* NPC = Cast<ATwinStickNPC>(OtherActor))
		{
			// 通知 NPC 被击中
			NPC->ProjectileImpact(FVector::ZeroVector);
		}
	}
}
