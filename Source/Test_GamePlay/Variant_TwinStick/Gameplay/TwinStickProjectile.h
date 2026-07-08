// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TwinStickProjectile.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;

/**
 *  双摇杆射击游戏的弹跳子弹。
 *  击中 NPC 时造成伤害，碰撞到其他物体会弹跳。
 */
UCLASS(abstract)
class ATwinStickProjectile : public AActor
{
	GENERATED_BODY()

	/** 子弹碰撞球体 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USphereComponent* CollisionSphere;

	/** 子弹视觉效果网格体 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

	/** 子弹移动组件 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

public:

	/** 构造函数 */
	ATwinStickProjectile();

	/** 碰撞处理 */
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

protected:

	/** 子弹停止时销毁自身 */
	UFUNCTION()
	void OnProjectileStop(const FHitResult& ImpactResult);

};
