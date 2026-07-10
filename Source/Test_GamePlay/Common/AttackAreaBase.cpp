// Copyright Epic Games, Inc. All Rights Reserved.

#include "Common/AttackAreaBase.h"
#include "Engine/World.h"
#include "Enemy/EnemyBase.h"
#include "HikariPlayerCharacter.h"

AAttackAreaBase::AAttackAreaBase()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	RootComponent = CollisionBox;
	CollisionBox->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));

	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AAttackAreaBase::OnOverlapBegin);
}

void AAttackAreaBase::BeginPlay()
{
	Super::BeginPlay();
	ElapsedTime = 0.0f;
	SetupCollision();
}

void AAttackAreaBase::SetupCollision()
{
	// 设为 DamageArea 通道，忽略所有
	CollisionBox->SetCollisionObjectType(ECC_GameTraceChannel1);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);

	// 同时检测敌人和玩家
	CollisionBox->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Overlap);
	CollisionBox->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Overlap);

	if (bDetectObstacle)
	{
		// 障碍物检测改用射线，不依赖碰撞响应矩阵
	}
}

void AAttackAreaBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 障碍物检测（射线扫描前方，不依赖碰撞系统）
	if (bDetectObstacle && Speed > 0.0f)
	{
		FHitResult Hit;
		FVector Start = GetActorLocation();
		FVector End = Start + GetActorForwardVector() * Speed * DeltaTime * 2.0f;

		// 只检测 WorldStatic 对象，忽略 Character/Pawn
		FCollisionObjectQueryParams ObjParams;
		ObjParams.AddObjectTypesToQuery(ECC_WorldStatic);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		QueryParams.AddIgnoredActor(GetOwner());

		if (GetWorld()->LineTraceSingleByObjectType(Hit, Start, End, ObjParams, QueryParams))
		{
			Disappear(EAttackAreaDisappearReason::HitObstacle);
			return;
		}
	}

	ElapsedTime += DeltaTime;
	if (ElapsedTime >= LifeTime)
	{
		Disappear(EAttackAreaDisappearReason::Lifetime);
		return;
	}

	if (Speed > 0.0f)
	{
		AddActorWorldOffset(GetActorForwardVector() * Speed * DeltaTime);
	}

	// 跟随目标（保持初始相对偏移）
	if (FollowTarget)
	{
		SetActorLocation(FollowTarget->GetActorLocation() + FollowOffset);
	}
}

void AAttackAreaBase::Initialize(float InLifeTime, float InSpeed, float InDamage, bool InDamageOpponentOnly, bool InObstacle, bool InMelee, AActor* InFollowTarget)
{
	LifeTime = InLifeTime;
	Speed = InSpeed;
	DamageValue = InDamage;
	bDamageOpponentOnly = InDamageOpponentOnly;
	bDetectObstacle = InObstacle;
	bIsMeleeAttack = InMelee;
	FollowTarget = InFollowTarget;

	// 生成后立即记录与跟随目标的相对偏移
	if (FollowTarget)
	{
		FollowOffset = GetActorLocation() - FollowTarget->GetActorLocation();
	}

	// 立即设置碰撞，不等 BeginPlay
	SetupCollision();
}

void AAttackAreaBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this || OtherActor == GetOwner()) return;

	if (!IsValidTarget(OtherActor)) return;

	ApplyDamage(OtherActor);

	// 远程攻击（子弹/射弹）打到目标后 0.1s 销毁
	// 近战攻击不销毁，等 LifeTime 自然结束
	if (!bIsMeleeAttack)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("AttackArea: 命中敌人"));
		SetLifeSpan(0.1f);
	}
}

void AAttackAreaBase::Disappear(EAttackAreaDisappearReason Reason)
{
	switch (Reason)
	{
	case EAttackAreaDisappearReason::Lifetime:
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("AttackArea: 生命周期结束"));
		break;
	case EAttackAreaDisappearReason::HitEnemy:
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("AttackArea: 命中敌人"));
		break;
	case EAttackAreaDisappearReason::HitObstacle:
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("AttackArea: 碰到障碍物"));
		break;
	}

	Destroy();
}

void AAttackAreaBase::ApplyDamage_Implementation(AActor* Target)
{
	if (AEnemyBase* Enemy = Cast<AEnemyBase>(Target))
	{
		Enemy->ApplyDamageToEnemy(DamageValue);
	}
	else if (AHikariPlayerCharacter* Player = Cast<AHikariPlayerCharacter>(Target))
	{
		Player->HandleDamage(DamageValue);
	}
}

bool AAttackAreaBase::IsValidTarget_Implementation(AActor* Target)
{
	// 如果目标是玩家且正在闪避，跳过
	if (AHikariPlayerCharacter* PlayerTarget = Cast<AHikariPlayerCharacter>(Target))
	{
		if (PlayerTarget->IsDashing()) return false;
	}

	if (!bDamageOpponentOnly) return true;

	if (GetOwner() && GetOwner()->IsA<AHikariPlayerCharacter>())
	{
		return Target->IsA<AEnemyBase>();
	}
	else
	{
		return Target->IsA<AHikariPlayerCharacter>();
	}
}
