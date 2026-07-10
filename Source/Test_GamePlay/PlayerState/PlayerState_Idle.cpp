// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerState/PlayerState_Idle.h"
#include "PlayerState/PlayerState_Move.h"
#include "PlayerState/PlayerState_Attack1.h"
#include "Input/PlayerInputComponent.h"
#include "HikariPlayerCharacter.h"

void UPlayerState_Idle::OnEnter_Implementation()
{
	Super::OnEnter_Implementation();

	AHikariPlayerCharacter* Player = Cast<AHikariPlayerCharacter>(GetOwner());
	if (!Player) return;

	UPlayerInputComponent* Input = Player->FindComponentByClass<UPlayerInputComponent>();
	if (!Input) return;

	Input->OnMoveXDelegate.AddUObject(this, &UPlayerState_Idle::OnMoveInput);
	Input->OnMoveYDelegate.AddUObject(this, &UPlayerState_Idle::OnMoveInput);
	Input->OnLmbDelegate.AddUObject(this, &UPlayerState_Idle::OnLmb);

	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Silver, TEXT("状态: Idle"));
}

void UPlayerState_Idle::OnExit_Implementation()
{
	Super::OnExit_Implementation();

	AHikariPlayerCharacter* Player = Cast<AHikariPlayerCharacter>(GetOwner());
	if (!Player) return;

	UPlayerInputComponent* Input = Player->FindComponentByClass<UPlayerInputComponent>();
	if (!Input) return;

	Input->OnMoveXDelegate.RemoveAll(this);
	Input->OnMoveYDelegate.RemoveAll(this);
	Input->OnLmbDelegate.RemoveAll(this);
}

void UPlayerState_Idle::OnMoveInput(float Value)
{
	if (FMath::IsNearlyZero(Value)) return;

	AHikariPlayerCharacter* Player = Cast<AHikariPlayerCharacter>(GetOwner());
	if (Player)
	{
		Player->SwitchState(UPlayerState_Move::StaticClass());
	}
}

void UPlayerState_Idle::OnLmb()
{
	AHikariPlayerCharacter* Player = Cast<AHikariPlayerCharacter>(GetOwner());
	if (Player)
	{
		Player->SwitchState(UPlayerState_Attack1::StaticClass());
	}
}