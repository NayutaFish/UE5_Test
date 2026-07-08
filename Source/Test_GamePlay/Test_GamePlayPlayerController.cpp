// Copyright Epic Games, Inc. All Rights Reserved.

#include "Test_GamePlayPlayerController.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"

ATest_GamePlayPlayerController::ATest_GamePlayPlayerController()
{
	// 显示鼠标光标（俯视角游戏一般保留）
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
}

void ATest_GamePlayPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (IsLocalPlayerController())
	{
		// 加载输入映射上下文
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}

		// WASD 绑定在 Character 的 SetupPlayerInputComponent 中处理
		// 这里不再绑定鼠标和触摸逻辑
	}
}

void ATest_GamePlayPlayerController::RotateActorByDirection(AActor* TargetActor, const FVector& Direction)
{
	if (!TargetActor) return;
	if (Direction.IsNearlyZero()) return;

	FRotator TargetRotation = Direction.Rotation();
	TargetRotation.Pitch = 0.f;
	TargetRotation.Roll = 0.f;
	TargetActor->SetActorRotation(TargetRotation);
}
