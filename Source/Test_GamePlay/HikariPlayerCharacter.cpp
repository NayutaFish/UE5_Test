// Fill out your copyright notice in the Description page of Project Settings.

#include "HikariPlayerCharacter.h"
#include "Common/AttackAreaBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Engine/World.h"
#include "Skill/HikariSkillComponent.h"
#include "Input/PlayerInputComponent.h"
#include "EnhancedInputComponent.h"
#include "Common/StateBase.h"
#include "PlayerState/PlayerState_Idle.h"

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

	SkillComponent = CreateDefaultSubobject<UHikariSkillComponent>(TEXT("SkillComponent"));
}

void AHikariPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	// ── 订阅 PlayerInputComponent 事件 ──
	if (UPlayerInputComponent* InputComp = FindComponentByClass<UPlayerInputComponent>())
	{
			InputComp->OnRmbDelegate.AddUObject(this, &AHikariPlayerCharacter::OnInputRmb);
		InputComp->OnSpaceDelegate.AddUObject(this, &AHikariPlayerCharacter::OnInputSpace);
		InputComp->OnQDelegate.AddUObject(this, &AHikariPlayerCharacter::OnInputQ);
		InputComp->OnEDelegate.AddUObject(this, &AHikariPlayerCharacter::OnInputE);
		InputComp->OnFDelegate.AddUObject(this, &AHikariPlayerCharacter::OnInputF);
	}

	// ── 默认进入 Idle 状态 ──
	// 自动从角色身上挂载的组件中查找 Idle 状态
	if (UPlayerState_Idle* Idle = FindComponentByClass<UPlayerState_Idle>())
	{
		SwitchState(UPlayerState_Idle::StaticClass());
	}
}

void AHikariPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentState)
	{
		CurrentState->Update(DeltaTime);
	}
}

void AHikariPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 通知 PlayerInputComponent 注册子系统和绑定回调（此时 Controller 和 InputComponent 均已就绪）
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (UPlayerInputComponent* InputComp = FindComponentByClass<UPlayerInputComponent>())
		{
			InputComp->SetupEnhancedInput(EnhancedInput, Cast<APlayerController>(GetController()));
		}
	}
}

// ── 主动技能 ──

void AHikariPlayerCharacter::TryCastSkill1()
{
	TryCastSkillSlot1();
}

void AHikariPlayerCharacter::TryCastSkill2()
{
	TryCastSkillSlot2();
}

void AHikariPlayerCharacter::TryCastSkillSlot1()
{
	if (SkillComponent)
	{
		SkillComponent->TryCastSkillSlot(0);
	}
}

void AHikariPlayerCharacter::TryCastSkillSlot2()
{
	if (SkillComponent)
	{
		SkillComponent->TryCastSkillSlot(1);
	}
}

void AHikariPlayerCharacter::TryCastSkillSlot3()
{
	if (SkillComponent)
	{
		SkillComponent->TryCastSkillSlot(2);
	}
}

// ── 状态机 ──

void AHikariPlayerCharacter::SwitchState(TSubclassOf<UStateBase> StateClass)
{
	if (!StateClass) return;

	// 查找指定子类的状态组件
	TArray<UStateBase*> Found;
	GetComponents(StateClass, Found);
	if (Found.Num() == 0) return;

	UStateBase* NewState = Found[0];
	if (NewState == CurrentState) return;

	// 旧状态退出
	if (CurrentState)
	{
		CurrentState->OnExit();
	}

	// 新状态进入
	CurrentState = NewState;
	CurrentState->OnEnter();
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
	bIsSprinting = false;
	CurrentHealth = 0.0f;
	OnPlayerDeath.Broadcast(this);
	Destroy();
}

// ──────────────────────────────
// PlayerInputComponent 事件响应
// ──────────────────────────────

void AHikariPlayerCharacter::OnInputLmb()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, TEXT("左键: LMB"));
}

void AHikariPlayerCharacter::OnInputRmb()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, TEXT("右键: RMB"));
}

void AHikariPlayerCharacter::OnInputSpace()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, TEXT("空格: Space"));
}

void AHikariPlayerCharacter::OnInputQ()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, TEXT("技能: Q"));
}

void AHikariPlayerCharacter::OnInputE()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, TEXT("技能: E"));
}

void AHikariPlayerCharacter::OnInputF()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, TEXT("技能: F"));
}
