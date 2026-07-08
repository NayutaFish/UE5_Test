// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Test_GamePlayPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

/**
 * 俯视角玩家控制器
 *
 * WASD 移动控制，不再使用鼠标点击移动
 */
UCLASS()
class ATest_GamePlayPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	/** 增强输入映射上下文 */
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

public:

	ATest_GamePlayPlayerController();

protected:

	virtual void SetupInputComponent() override;

	/** 根据方向向量旋转 Actor（蓝图可调用） */
	UFUNCTION(BlueprintCallable, Category = "Movement")
	static void RotateActorByDirection(AActor* TargetActor, const FVector& Direction);
};
