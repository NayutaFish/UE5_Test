// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TwinStickPlayerController.generated.h"

class UInputMappingContext;
class ATwinStickCharacter;

/**
 *  双摇杆射击游戏的玩家控制器。
 *  管理输入映射上下文，在角色销毁后自动重生。
 */
UCLASS(abstract, Config="Game")
class ATwinStickPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	/** 默认输入映射上下文列表 */
	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	/** 移动端排除的输入映射上下文 */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> MobileExcludedMappingContexts;

	/** 移动端控制 Widget 类 */
	UPROPERTY(EditAnywhere, Category="Input|Touch Controls")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	/** 移动端控制 Widget 实例 */
	UPROPERTY()
	TObjectPtr<UUserWidget> MobileControlsWidget;

	/** 强制使用触摸控制（即使在非移动平台） */
	UPROPERTY(EditAnywhere, Config, Category = "Input|Touch Controls")
	bool bForceTouchControls = false;

	/** 角色重生时的角色类 */
	UPROPERTY(EditAnywhere, Category="Respawn")
	TSubclassOf<ATwinStickCharacter> CharacterClass;

protected:

	/** 游戏初始化 */
	virtual void BeginPlay() override;

	/** 初始化输入绑定 */
	virtual void SetupInputComponent() override;

	/** 附身 Pawn 时的初始化 */
	virtual void OnPossess(APawn* InPawn) override;

	/** 当附身的 Pawn 被销毁时调用 */
	UFUNCTION()
	void OnPawnDestroyed(AActor* DestroyedActor);

	/** 是否使用 UMG 触摸控制 */
	bool ShouldUseTouchControls() const;
};
