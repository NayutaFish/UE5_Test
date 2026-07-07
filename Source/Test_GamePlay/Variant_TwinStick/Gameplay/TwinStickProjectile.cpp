// Copyright Epic Games, Inc. All Rights Reserved.


#include "TwinStickProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TwinStickNPC.h"

// 构造函数：创建碰撞体、网格体和子弹移动组件
ATwinStickProjectile::ATwinStickProjectile()
{
 	PrimaryActorTick.bCanEverTick = true;

	// 自动销毁生命周期
	InitialLifeSpan = 2.0f;

	// 创建碰撞球体并设为根组件
	RootComponent = CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere"));

	CollisionSphere->SetSphereRadius(35.0f);
	CollisionSphere->SetNotifyRigidBodyCollision(true);
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionSphere->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionSphere->SetCollisionResponseToAllChannels(ECR_Block);

	// 创建网格体
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	Mesh->SetCollisionProfileName(FName("NoCollision"));

	// 创建子弹移动组件（非 Scene Component，无需挂载）
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));

	ProjectileMovement->InitialSpeed = 2000.0f;
	ProjectileMovement->MaxSpeed = 15000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bRotationRemainsVertical = true;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->bForceSubStepping = true;

	ProjectileMovement->OnProjectileStop.AddDynamic(this, &ATwinStickProjectile::OnProjectileStop);
}

// 碰撞处理：击中 NPC 时造成伤害并销毁自身
void ATwinStickProjectile::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	// 是否击中了 NPC？
	if (ATwinStickNPC* NPC = Cast<ATwinStickNPC>(Other))
	{
		// 通知 NPC 被击中
		NPC->ProjectileImpact(FVector::ZeroVector);

		// 销毁子弹
		Destroy();
	}
}

// 子弹停止时立即销毁
void ATwinStickProjectile::OnProjectileStop(const FHitResult& ImpactResult)
{
	// 立即销毁
	Destroy();
}
