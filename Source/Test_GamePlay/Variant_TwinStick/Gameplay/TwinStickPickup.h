// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TwinStickPickup.generated.h"

class USphereComponent;
class UStaticMeshComponent;

/**
 *  双摇杆射击游戏的拾取物。
 *  玩家触碰后可获得一个 AoE 物品。
 */
UCLASS(abstract)
class ATwinStickPickup : public AActor
{
	GENERATED_BODY()

	/** 拾取物碰撞球体 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USphereComponent* CollisionSphere;

	/** 拾取物视觉效果 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

public:

	/** 构造函数 */
	ATwinStickPickup();

	/** 碰撞处理：玩家触碰时拾取 */
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

};
