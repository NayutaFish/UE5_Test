// Copyright Epic Games, Inc. All Rights Reserved.


#include "TwinStickPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "TwinStickCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "Blueprint/UserWidget.h"
#include "Test_GamePlay.h"
#include "Widgets/Input/SVirtualJoystick.h"

void ATwinStickPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 仅在本地玩家控制器上生成触摸控件
	if (IsLocalPlayerController() && ShouldUseTouchControls())
	{
		// 生成移动端控制 Widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// 将控件添加到玩家屏幕
			MobileControlsWidget->AddToPlayerScreen(0);

		} else {

			UE_LOG(LogTest_GamePlay, Error, TEXT("Could not spawn mobile controls widget."));

		}

	}
}

// 为本地玩家添加增强输入映射上下文
void ATwinStickPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// 仅对本地玩家控制器添加 IMC
	if (IsLocalPlayerController())
	{
		// 添加输入映射上下文
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// 非移动端时添加额外的映射上下文
			if (!ShouldUseTouchControls())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
}

// 附身 Pawn 时订阅其销毁事件
void ATwinStickPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// 订阅 Pawn 的 OnDestroyed 委托
	InPawn->OnDestroyed.AddDynamic(this, &ATwinStickPlayerController::OnPawnDestroyed);
}

// 角色被销毁后在 PlayerStart 处重生
void ATwinStickPlayerController::OnPawnDestroyed(AActor* DestroyedActor)
{
	// 查找 PlayerStart
	TArray<AActor*> ActorList;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), ActorList);

	if (ActorList.Num() > 0)
	{
		// 在 PlayerStart 处生成新角色
		const FTransform SpawnTransform = ActorList[0]->GetActorTransform();

		if (ATwinStickCharacter* RespawnedCharacter = GetWorld()->SpawnActor<ATwinStickCharacter>(CharacterClass, SpawnTransform))
		{
			// 附身该角色
			Possess(RespawnedCharacter);
		}
	}
}

bool ATwinStickPlayerController::ShouldUseTouchControls() const
{
	// 是否在移动平台或强制触摸模式？
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}
