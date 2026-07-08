// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "StrategyPawn.generated.h"

class UCameraComponent;
class UFloatingPawnMovement;

/**
 * 策略游戏的俯视角摄像机 Pawn
 * 提供正交投影摄像机和浮动移动组件，单位通过其他方式间接控制
 */
UCLASS(abstract)
class AStrategyPawn : public APawn
{
	GENERATED_BODY()

	/** 摄像机组件 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	/** 浮动移动组件 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UFloatingPawnMovement* FloatingPawnMovement;

public:

	/** 构造函数 */
	AStrategyPawn();

public:

	/** 设置摄像机缩放值（正交宽度） */
	void SetZoomModifier(float Value);

	/** 获取摄像机组件 */
	UCameraComponent* GetCamera() const { return Camera; }
};
