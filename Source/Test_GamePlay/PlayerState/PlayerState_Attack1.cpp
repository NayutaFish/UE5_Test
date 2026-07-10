// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerState/PlayerState_Attack1.h"
#include "PlayerState/PlayerState_Idle.h"
#include "PlayerState/PlayerState_Move.h"
#include "Input/PlayerInputComponent.h"
#include "HikariPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Common/AttackAreaBase.h"
#include "Engine/World.h"
#include "Engine/LocalPlayer.h"

void UPlayerState_Attack1::OnEnter_Implementation()
{
	Super::OnEnter_Implementation();

	AHikariPlayerCharacter* Player = Cast<AHikariPlayerCharacter>(GetOwner());
	if (!Player) return;

	// 订阅 WASD 输入，跟踪是否有移动
	UPlayerInputComponent* Input = Player->FindComponentByClass<UPlayerInputComponent>();
	if (Input)
	{
		Input->OnMoveXDelegate.AddUObject(this, &UPlayerState_Attack1::OnMoveX);
		Input->OnMoveYDelegate.AddUObject(this, &UPlayerState_Attack1::OnMoveY);
	}

	// 播放攻击动画
	Player->BeginAttack();

	// 保存当前速度方向，持续滑行
	FVector CurrentVel = Player->GetCharacterMovement()->Velocity;
	SlideDirection = !CurrentVel.IsNearlyZero() ? CurrentVel.GetSafeNormal() : FVector::ZeroVector;

	// 旋转朝向鼠标方向（仅 Yaw）
	if (APlayerController* PC = Cast<APlayerController>(Player->GetController()))
	{
		FHitResult Hit;
		PC->GetHitResultUnderCursor(ECC_Visibility, false, Hit);
		if (Hit.bBlockingHit)
		{
			FVector ToTarget = Hit.Location - Player->GetActorLocation();
			ToTarget.Z = 0.0f;
			if (!ToTarget.IsNearlyZero())
			{
				Player->SetActorRotation(FRotator(0.0f, ToTarget.Rotation().Yaw, 0.0f));
			}
		}
	}

	// 在面前生成攻击区域
	if (Player->AttackAreaClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = Player;
		SpawnParams.Instigator = Player;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		if (AAttackAreaBase* AttackArea = GetWorld()->SpawnActor<AAttackAreaBase>(
				Player->AttackAreaClass,
				Player->GetActorLocation() + Player->GetActorForwardVector() * 100.0f,
				Player->GetActorRotation(),
				SpawnParams))
		{
			AttackArea->Initialize(0.3f, 0.0f, 300.0f, true, false, true, Player);
		}
	}

	// 0.3s 后检测是否需要切换状态
	bHadMoveInput = false;
	GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, this, &UPlayerState_Attack1::OnAttackTimer, 0.3f, false);

	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Silver, TEXT("状态: Attack1"));
}

void UPlayerState_Attack1::OnExit_Implementation()
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
		GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
	}
}

void UPlayerState_Attack1::OnMoveX(float Value)
{
	if (!FMath::IsNearlyZero(Value)) bHadMoveInput = true;
}

void UPlayerState_Attack1::OnMoveY(float Value)
{
	if (!FMath::IsNearlyZero(Value)) bHadMoveInput = true;
}

void UPlayerState_Attack1::Update_Implementation(float DeltaTime)
{
	Super::Update_Implementation(DeltaTime);

	AHikariPlayerCharacter* Player = Cast<AHikariPlayerCharacter>(GetOwner());
	if (!Player) return;

	// 持续沿保存方向滑动
	if (!SlideDirection.IsNearlyZero())
	{
		Player->GetCharacterMovement()->Velocity = SlideDirection * 500.0f;
	}
	else
	{
		Player->GetCharacterMovement()->Velocity = FVector::ZeroVector;
	}
}

void UPlayerState_Attack1::OnAttackTimer()
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

