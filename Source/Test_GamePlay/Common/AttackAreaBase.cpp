// Copyright Epic Games, Inc. All Rights Reserved.

#include "Common/AttackAreaBase.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"

AAttackAreaBase::AAttackAreaBase()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	RootComponent = CollisionComponent;
	CollisionComponent->SetSphereRadius(100.0f);
	CollisionComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	// 绑定碰撞事件
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AAttackAreaBase::OnOverlapBegin);
}

void AAttackAreaBase::BeginPlay()
{
	Super::BeginPlay();
	ElapsedTime = 0.0f;
}

void AAttackAreaBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ElapsedTime += DeltaTime;

	// 存在时间到期 → 销毁
	if (ElapsedTime >= LifeTime)
	{
		Destroy();
		return;
	}

	// 移动（射弹类沿当前前方移动）
	if (Speed > 0.0f)
	{
		AddActorWorldOffset(GetActorForwardVector() * Speed * DeltaTime);
	}
}

void AAttackAreaBase::Initialize(AActor* InOwner, float InLifeTime, float InSpeed, float InDamage, bool bEnemyOnly)
{
	OwnerActor = InOwner;
	LifeTime = InLifeTime;
	Speed = InSpeed;
	DamageValue = InDamage;
	bDetectEnemyOnly = bEnemyOnly;

	// TODO: 根据 OwnerActor 自动设置碰撞过滤
	//
	// 如果 OwnerActor 是 玩家：
	//   bDetectObstacle     → 射弹碰撞障碍物才启用（由外部传入控制）
	//   bDetectDestructible → 检测敌方可破坏建筑
	//   CollisionComponent 的碰撞通道 → 只检测 Enemy 等敌对类型
	//
	// 如果 OwnerActor 是 敌人：
	//   bDetectObstacle     → 同上
	//   bDetectDestructible → 检测玩家的可破坏建筑
	//   bDetectEnemyOnly    → 只检测玩家
}

void AAttackAreaBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this || OtherActor == OwnerActor) return;
	if (!IsValidTarget(OtherActor)) return;

	ApplyDamage(OtherActor);

	// TODO: 是否需要穿透？需要的话加 bPenetrate 变量判断
	// 默认识别到目标就销毁（子弹逻辑），近战可改为不销毁
	Destroy();
}

void AAttackAreaBase::ApplyDamage_Implementation(AActor* Target)
{
	// TODO: 具体伤害逻辑，子类重写
	// 可通过 Target 获取 ActorBase 或 Character 组件调 TakeDamage
}

bool AAttackAreaBase::IsValidTarget_Implementation(AActor* Target)
{
	// TODO: 标签/类判断逻辑，子类重写
	// 通过 Target->ActorHasTag("Enemy") 等方式判断
	return true;
}
