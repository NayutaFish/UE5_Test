// Fill out your copyright notice in the Description page of Project Settings.

#include "Input/PlayerInputComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"

UPlayerInputComponent::UPlayerInputComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// ── 构造时创建所有 InputAction 和 MappingContext ──
	MoveXAction = CreateDefaultSubobject<UInputAction>(TEXT("MoveXAction"));
	MoveXAction->ValueType = EInputActionValueType::Axis1D;

	MoveYAction = CreateDefaultSubobject<UInputAction>(TEXT("MoveYAction"));
	MoveYAction->ValueType = EInputActionValueType::Axis1D;

	ShiftAction = CreateDefaultSubobject<UInputAction>(TEXT("ShiftAction"));
	ShiftAction->ValueType = EInputActionValueType::Axis1D;

	LmbAction = CreateDefaultSubobject<UInputAction>(TEXT("LmbAction"));
	LmbAction->ValueType = EInputActionValueType::Axis1D;

	RmbAction = CreateDefaultSubobject<UInputAction>(TEXT("RmbAction"));
	RmbAction->ValueType = EInputActionValueType::Axis1D;

	SpaceAction = CreateDefaultSubobject<UInputAction>(TEXT("SpaceAction"));
	SpaceAction->ValueType = EInputActionValueType::Axis1D;

	QAction = CreateDefaultSubobject<UInputAction>(TEXT("QAction"));
	QAction->ValueType = EInputActionValueType::Axis1D;

	EAction = CreateDefaultSubobject<UInputAction>(TEXT("EAction"));
	EAction->ValueType = EInputActionValueType::Axis1D;

	FAction = CreateDefaultSubobject<UInputAction>(TEXT("FAction"));
	FAction->ValueType = EInputActionValueType::Axis1D;

	// ── 创建 Mapping Context ──
	DefaultMappingContext = CreateDefaultSubobject<UInputMappingContext>(TEXT("DefaultMappingContext"));
}

void UPlayerInputComponent::BeginPlay()
{
	Super::BeginPlay();

	if (bInputsMapped) return;
	bInputsMapped = true;

	// ── 映射按键（不能在构造中做，会触发 NewObject 错误） ──

	// 轴：D → +1, A → -1
	DefaultMappingContext->MapKey(MoveXAction, EKeys::D);
	FEnhancedActionKeyMapping& AMapping = DefaultMappingContext->MapKey(MoveXAction, EKeys::A);
	AMapping.Modifiers.Add(NewObject<UInputModifierNegate>());

	// 轴：W → +1, S → -1
	DefaultMappingContext->MapKey(MoveYAction, EKeys::W);
	FEnhancedActionKeyMapping& SMapping = DefaultMappingContext->MapKey(MoveYAction, EKeys::S);
	SMapping.Modifiers.Add(NewObject<UInputModifierNegate>());

	// 轴：LeftShift 按住
	DefaultMappingContext->MapKey(ShiftAction, EKeys::LeftShift);

	// 动作（按一次）
	DefaultMappingContext->MapKey(LmbAction, EKeys::LeftMouseButton);
	DefaultMappingContext->MapKey(RmbAction, EKeys::RightMouseButton);
	DefaultMappingContext->MapKey(SpaceAction, EKeys::SpaceBar);
	DefaultMappingContext->MapKey(QAction, EKeys::Q);
	DefaultMappingContext->MapKey(EAction, EKeys::E);
	DefaultMappingContext->MapKey(FAction, EKeys::F);
}

void UPlayerInputComponent::SetupEnhancedInput(UEnhancedInputComponent* EnhancedInput, APlayerController* PC)
{
	if (!EnhancedInput || !PC) return;

	// ── 注册 Mapping Context 到子系统 ──
	if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// ── 绑定回调 ──
	// 轴（Triggered = 按住持续触发）
	EnhancedInput->BindAction(MoveXAction, ETriggerEvent::Triggered, this, &UPlayerInputComponent::OnMoveX);
	EnhancedInput->BindAction(MoveYAction, ETriggerEvent::Triggered, this, &UPlayerInputComponent::OnMoveY);
	EnhancedInput->BindAction(ShiftAction, ETriggerEvent::Triggered, this, &UPlayerInputComponent::OnShift);

	// 动作（Started = 按下的瞬间触发一次）
	EnhancedInput->BindAction(LmbAction, ETriggerEvent::Started, this, &UPlayerInputComponent::OnLmb);
	EnhancedInput->BindAction(RmbAction, ETriggerEvent::Started, this, &UPlayerInputComponent::OnRmb);
	EnhancedInput->BindAction(SpaceAction, ETriggerEvent::Started, this, &UPlayerInputComponent::OnSpace);
	EnhancedInput->BindAction(QAction, ETriggerEvent::Started, this, &UPlayerInputComponent::OnQ);
	EnhancedInput->BindAction(EAction, ETriggerEvent::Started, this, &UPlayerInputComponent::OnE);
	EnhancedInput->BindAction(FAction, ETriggerEvent::Started, this, &UPlayerInputComponent::OnF);
}

// ──────────────────────────────
// 轴回调（按住持续触发）
// ──────────────────────────────

void UPlayerInputComponent::OnMoveX(const FInputActionValue& Value)
{
	OnMoveXDelegate.Broadcast(Value.Get<float>());
}

void UPlayerInputComponent::OnMoveY(const FInputActionValue& Value)
{
	OnMoveYDelegate.Broadcast(Value.Get<float>());
}

void UPlayerInputComponent::OnShift(const FInputActionValue& Value)
{
	OnShiftDelegate.Broadcast(Value.Get<float>());
}

// ──────────────────────────────
// 动作回调（按下一次触发）
// ──────────────────────────────

void UPlayerInputComponent::OnLmb()
{
	OnLmbDelegate.Broadcast();
}

void UPlayerInputComponent::OnRmb()
{
	OnRmbDelegate.Broadcast();
}

void UPlayerInputComponent::OnSpace()
{
	OnSpaceDelegate.Broadcast();
}

void UPlayerInputComponent::OnQ()
{
	OnQDelegate.Broadcast();
}

void UPlayerInputComponent::OnE()
{
	OnEDelegate.Broadcast();
}

void UPlayerInputComponent::OnF()
{
	OnFDelegate.Broadcast();
}

