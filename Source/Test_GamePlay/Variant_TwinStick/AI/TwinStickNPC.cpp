// Copyright Epic Games, Inc. All Rights Reserved.


#include "TwinStickNPC.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "TwinStickCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TwinStickGameMode.h"
#include "TwinStickPickup.h"
#include "Engine/World.h"
#include "TwinStickNPCDestruction.h"
#include "TimerManager.h"

// 构造函数：配置碰撞、网格体、角色移动和 AI 自动附身
ATwinStickNPC::ATwinStickNPC()
{
	PrimaryActorTick.bCanEverTick = true;

	// 生成时自动附身 AI 控制器
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// 配置继承的组件
	GetCapsuleComponent()->SetCapsuleRadius(45.0f);
	GetCapsuleComponent()->SetNotifyRigidBodyCollision(true);

	GetMesh()->SetCollisionProfileName(FName("NoCollision"));

	GetCharacterMovement()->GravityScale = 1.5f;
	GetCharacterMovement()->MaxAcceleration = 1000.0f;
	GetCharacterMovement()->BrakingFriction = 1.0f;
	GetCharacterMovement()->MaxWalkSpeed = 200.0f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 100.0f;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 640.0f, 0.0f);
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseRVOAvoidance = true;
	GetCharacterMovement()->AvoidanceConsiderationRadius = 250.0f;
	GetCharacterMovement()->AvoidanceWeight = 1.0f;
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
}

void ATwinStickNPC::BeginPlay()
{
	Super::BeginPlay();

	// 通知 GameMode 增加 NPC 计数，用于上限控制
	if (ATwinStickGameMode* GM = Cast<ATwinStickGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GM->IncreaseNPCs();
	}

}

void ATwinStickNPC::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// 清除销毁定时器
	GetWorld()->GetTimerManager().ClearTimer(DestructionTimer);
}

// 销毁时通知 GameMode 减少 NPC 计数
void ATwinStickNPC::Destroyed()
{
	// 减少 NPC 计数
	if (ATwinStickGameMode* GM = Cast<ATwinStickGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GM->DecreaseNPCs();
	}

	Super::Destroyed();
}

// 与玩家角色碰撞时对其造成伤害
void ATwinStickNPC::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	// 是否撞到了玩家角色？
	if (ATwinStickCharacter* PlayerCharacter = Cast<ATwinStickCharacter>(Other))
	{
		// 对角色造成伤害
		PlayerCharacter->HandleDamage(1.0f, GetActorForwardVector());
	}
}

// 处理子弹撞击：标记击中、停用移动、奖励分数、生成拾取物和销毁效果
void ATwinStickNPC::ProjectileImpact(const FVector& ForwardVector)
{
	// 如果已被击中则不再处理
	if (bHit)
	{
		return;
	}

	// 标记为已击中
	bHit = true;

	// 停用角色移动
	GetCharacterMovement()->Deactivate();

	// 奖励分数
	if (ATwinStickGameMode* GM = Cast<ATwinStickGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GM->ScoreUpdate(Score);
	}

	// 按概率随机生成拾取物
	if (FMath::RandRange(0, 100) < PickupSpawnChance)
	{
		ATwinStickPickup* Pickup = GetWorld()->SpawnActor<ATwinStickPickup>(PickupClass, GetActorTransform());
	}

	// 生成 NPC 销毁代理（用于播放特效）
	ATwinStickNPCDestruction* DestructionProxy = GetWorld()->SpawnActor<ATwinStickNPCDestruction>(DestructionProxyClass, GetActorTransform());

	// 隐藏自身
	SetActorHiddenInGame(true);

	// 禁用碰撞
	SetActorEnableCollision(false);

	// 延迟销毁
	GetWorld()->GetTimerManager().SetTimer(DestructionTimer, this, &ATwinStickNPC::DeferredDestroy, DeferredDestructionTime, false);
}

void ATwinStickNPC::DeferredDestroy()
{
	// 销毁自身
	Destroy();
}
