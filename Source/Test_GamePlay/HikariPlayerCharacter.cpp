// Fill out your copyright notice in the Description page of Project Settings.

#include "HikariPlayerCharacter.h"
#include "Common/AttackAreaBase.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Engine/World.h"

AHikariPlayerCharacter::AHikariPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);
	GetCapsuleComponent()->SetCollisionObjectType(ECC_GameTraceChannel3);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
}

void AHikariPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AHikariPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHikariPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 右键（预留，暂时无用）
	PlayerInputComponent->BindAction("RightClickAttack", IE_Pressed, this, &AHikariPlayerCharacter::OnAttackInput);

	// EnhancedInput WASD + 疾跑
	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInput) return;

	if (MoveHorizontalAction)
		EnhancedInput->BindAction(MoveHorizontalAction, ETriggerEvent::Triggered, this, &AHikariPlayerCharacter::MoveHorizontal);
	if (MoveVerticalAction)
		EnhancedInput->BindAction(MoveVerticalAction, ETriggerEvent::Triggered, this, &AHikariPlayerCharacter::MoveVertical);
	if (SprintAction)
	{
		EnhancedInput->BindAction(SprintAction, ETriggerEvent::Started, this, &AHikariPlayerCharacter::StartSprint);
		EnhancedInput->BindAction(SprintAction, ETriggerEvent::Completed, this, &AHikariPlayerCharacter::StopSprint);
		EnhancedInput->BindAction(SprintAction, ETriggerEvent::Canceled, this, &AHikariPlayerCharacter::StopSprint);
	}
	if (AttackAction)
		EnhancedInput->BindAction(AttackAction, ETriggerEvent::Started, this, &AHikariPlayerCharacter::OnAttack);
}

// ── 移动 ──

void AHikariPlayerCharacter::MoveHorizontal(const FInputActionValue& Value)
{
	if (!CanMove()) return;
	float AxisValue = Value.Get<float>();
	if (FMath::IsNearlyZero(AxisValue)) return;

	const FVector Dir = (FVector::RightVector - FVector::ForwardVector).GetSafeNormal();
	AddMovementInput(Dir, AxisValue);
}

void AHikariPlayerCharacter::MoveVertical(const FInputActionValue& Value)
{
	if (!CanMove()) return;
	float AxisValue = Value.Get<float>();
	if (FMath::IsNearlyZero(AxisValue)) return;

	const FVector Dir = (FVector::ForwardVector + FVector::RightVector).GetSafeNormal();
	AddMovementInput(Dir, AxisValue);
}

// ── 疾跑 ──

void AHikariPlayerCharacter::StartSprint()
{
	if (CurrentActionState == EHikariActionState::Attacking) CancelAttack();
	if (!CanMove()) return;
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AHikariPlayerCharacter::StopSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

// ── 状态 ──

bool AHikariPlayerCharacter::CanMove() const
{
	return CurrentActionState == EHikariActionState::Normal;
}

bool AHikariPlayerCharacter::CanStartAction() const
{
	return CurrentActionState == EHikariActionState::Normal;
}

void AHikariPlayerCharacter::SetActionState(EHikariActionState NewState)
{
	if (CurrentActionState == NewState) return;
	CurrentActionState = NewState;
}

// ── 攻击动画（左键） ──

void AHikariPlayerCharacter::OnAttackInput()
{
	BeginAttack();
}

void AHikariPlayerCharacter::BeginAttack()
{
	if (!CanStartAction()) return;
	if (!AttackMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("AttackMontage is not set."));
		return;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;

	SetActionState(EHikariActionState::Attacking);

	float MontageLength = PlayAnimMontage(AttackMontage);
	if (MontageLength <= 0.0f)
	{
		SetActionState(EHikariActionState::Normal);
		return;
	}

	FOnMontageEnded Delegate;
	Delegate.BindUObject(this, &AHikariPlayerCharacter::OnAttackMontageEnded);
	AnimInstance->Montage_SetEndDelegate(Delegate, AttackMontage);
}

void AHikariPlayerCharacter::EndAttack()
{
	if (CurrentActionState != EHikariActionState::Attacking) return;
	SetActionState(EHikariActionState::Normal);
}

void AHikariPlayerCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != AttackMontage) return;
	EndAttack();
}

void AHikariPlayerCharacter::CancelAttack()
{
	if (CurrentActionState != EHikariActionState::Attacking) return;
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		AnimInstance->Montage_Stop(AttackCancelBlendOutTime, AttackMontage);
	SetActionState(EHikariActionState::Normal);
	UE_LOG(LogTemp, Log, TEXT("Hikari Attack Canceled"));
}

// ── 左键攻击（动画 + 伤害范围） ──

void AHikariPlayerCharacter::OnAttack()
{
	if (bIsDead) return;

	// 左键同时播放攻击动画并生成伤害范围
	BeginAttack();

	if (!AttackAreaClass) return;

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (AAttackAreaBase* AttackArea = GetWorld()->SpawnActor<AAttackAreaBase>(
			AttackAreaClass,
			GetActorLocation() + GetActorForwardVector() * 100.0f,
			GetActorRotation(),
			Params))
	{
		AttackArea->Initialize(0.5f, 0.0f, 300.0f, true, false, true);
	}
}

// ── 受伤与死亡 ──

void AHikariPlayerCharacter::HandleDamage(float DamageAmount)
{
	if (bIsDead || DamageAmount <= 0.0f) return;
	CurrentHealth = FMath::Max(0.0f, CurrentHealth - DamageAmount);
	if (CurrentHealth <= 0.0f) Die();
}

void AHikariPlayerCharacter::TestDie()
{
	Die();
}

void AHikariPlayerCharacter::Die()
{
	if (bIsDead) return;
	bIsDead = true;
	CurrentHealth = 0.0f;
	OnPlayerDeath.Broadcast(this);
	Destroy();
}
