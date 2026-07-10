// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerState/PlayerState_Skill1.h"
#include "PlayerState/PlayerState_Idle.h"
#include "PlayerState/PlayerState_Move.h"
#include "Input/PlayerInputComponent.h"
#include "HikariPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Skill/HikariSkillComponent.h"
#include "Engine/World.h"

void UPlayerState_Skill1::OnEnter_Implementation()
{
	Super::OnEnter_Implementation();

	AHikariPlayerCharacter* Player = Cast<AHikariPlayerCharacter>(GetOwner());
	if (!Player) return;

	// 订阅 WASD 输入
	UPlayerInputComponent* Input = Player->FindComponentByClass<UPlayerInputComponent>();
	if (Input)
	{
		Input->OnMoveXDelegate.AddUObject(this, &UPlayerState_Skill1::OnMoveX);
		Input->OnMoveYDelegate.AddUObject(this, &UPlayerState_Skill1::OnMoveY);
	}

	// 释放 1 技能（在动画之前，否则 CanStartAction 会拦截）
	Player->TryCastSkillSlot1();

	// 播放攻击动画（和 Attack1 同款）
	Player->BeginAttack();

	// 保存当前速度方向，持续滑行
	FVector CurrentVel = Player->GetCharacterMovement()->Velocity;
	SlideDirection = !CurrentVel.IsNearlyZero() ? CurrentVel.GetSafeNormal() : FVector::ZeroVector;

	// 0.3s 后检测退出
	bHadMoveInput = false;
	GetWorld()->GetTimerManager().SetTimer(SkillTimerHandle, this, &UPlayerState_Skill1::OnSkillTimer, 0.3f, false);

	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Silver, TEXT("状态: Skill1"));
}

void UPlayerState_Skill1::OnExit_Implementation()
{
	Super::OnExit_Implementation();

	AHikariPlayerCharacter* Player = Cast<AHikariPlayerCharacter>(GetOwner());
	if (!Player) return;

	// 取消订阅
	UPlayerInputComponent* Input = Player->FindComponentByClass<UPlayerInputComponent>();
	if (Input)
	{
		Input->OnMoveXDelegate.RemoveAll(this);
		Input->OnMoveYDelegate.RemoveAll(this);
	}

	// 取消计时器
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(SkillTimerHandle);
	}
}

void UPlayerState_Skill1::Update_Implementation(float DeltaTime)
{
	Super::Update_Implementation(DeltaTime);

	AHikariPlayerCharacter* Player = Cast<AHikariPlayerCharacter>(GetOwner());
	if (!Player) return;

	// 持续滑行
	if (!SlideDirection.IsNearlyZero())
	{
		Player->GetCharacterMovement()->Velocity = SlideDirection * 200.0f;
	}
	else
	{
		Player->GetCharacterMovement()->Velocity = FVector::ZeroVector;
	}
}

void UPlayerState_Skill1::OnMoveX(float Value)
{
	if (!FMath::IsNearlyZero(Value)) bHadMoveInput = true;
}

void UPlayerState_Skill1::OnMoveY(float Value)
{
	if (!FMath::IsNearlyZero(Value)) bHadMoveInput = true;
}

void UPlayerState_Skill1::OnSkillTimer()
{
	AHikariPlayerCharacter* Player = Cast<AHikariPlayerCharacter>(GetOwner());
	if (!Player) return;

	if (bHadMoveInput)
	{
		Player->SwitchState(UPlayerState_Move::StaticClass());
	}
	else
	{
		Player->SwitchState(UPlayerState_Idle::StaticClass());
	}
}