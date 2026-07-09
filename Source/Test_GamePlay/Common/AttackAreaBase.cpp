// Copyright Epic Games, Inc. All Rights Reserved.

#include "Common/AttackAreaBase.h"
#include "Engine/World.h"
#include "Enemy/EnemyBase.h"
#include "HikariPlayerCharacter.h"

AAttackAreaBase::AAttackAreaBase()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	// 创建 BoxCollider，挂在根节点下
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(SceneRoot);
	CollisionBox->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
	// 碰撞体设为 DamageArea 通道，默认忽略所有，只检测 EnemyHitbox
	CollisionBox->SetCollisionObjectType(ECC_GameTraceChannel1); // DamageArea
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Overlap); // 检测敌人
	CollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap); // 检测障碍

	// 绑定碰撞事件
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AAttackAreaBase::OnOverlapBegin);
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
	if (ElapsedTime >= LifeTime)
	{
		Destroy();
		return;
	}

	if (Speed > 0.0f)
	{
		AddActorWorldOffset(GetActorForwardVector() * Speed * DeltaTime);
	}
}

void AAttackAreaBase::Initialize(float InLifeTime, float InSpeed, float InDamage, bool bEnemyOnly)
{
	LifeTime = InLifeTime;
	Speed = InSpeed;
	DamageValue = InDamage;
	bDetectEnemyOnly = bEnemyOnly;
}

void AAttackAreaBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this || OtherActor == GetOwner()) return;
	if (!IsValidTarget(OtherActor)) return;

	ApplyDamage(OtherActor);
	Destroy();
}

void AAttackAreaBase::ApplyDamage_Implementation(AActor* Target)
{
	// 默认伤害逻辑：根据目标类型调用对应的受伤接口
	// 如果目标是敌人类
	if (AEnemyBase* Enemy = Cast<AEnemyBase>(Target))
	{
		Enemy->ApplyDamageToEnemy(DamageValue);
	}
	// 如果目标是玩家角色
	else if (AHikariPlayerCharacter* Player = Cast<AHikariPlayerCharacter>(Target))
	{
		Player->HandleDamage(DamageValue);
	}
}

bool AAttackAreaBase::IsValidTarget_Implementation(AActor* Target)
{
	if (!bDetectEnemyOnly) return true;

	// 如果攻击者是玩家 → 只检测敌人
	// 如果攻击者是敌人 → 只检测玩家
	if (GetOwner() && GetOwner()->IsA<AHikariPlayerCharacter>())
	{
		return Target->IsA<AEnemyBase>();
	}
	else
	{
		return Target->IsA<AHikariPlayerCharacter>();
	}
}
