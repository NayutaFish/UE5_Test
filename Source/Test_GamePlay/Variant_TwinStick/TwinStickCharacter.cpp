// Copyright Epic Games, Inc. All Rights Reserved.


#include "TwinStickCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "TwinStickGameMode.h"
#include "TwinStickAoEAttack.h"
#include "Kismet/KismetMathLibrary.h"
#include "TwinStickProjectile.h"
#include "Engine/World.h"
#include "TimerManager.h"

// 构造函数：创建弹簧臂、相机，并配置角色移动参数
ATwinStickCharacter::ATwinStickCharacter()
{
 	PrimaryActorTick.bCanEverTick = true;

	// 创建弹簧臂
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(RootComponent);

	SpringArm->SetRelativeRotation(FRotator(-50.0f, 0.0f, 0.0f));

	SpringArm->TargetArmLength = 2200.0f;
	SpringArm->bDoCollisionTest = false;
	SpringArm->bInheritYaw = false;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 0.5f;

	// 创建相机
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	Camera->SetFieldOfView(75.0f);

	// 配置角色移动
	GetCharacterMovement()->GravityScale = 1.5f;
	GetCharacterMovement()->MaxAcceleration = 1000.0f;
	GetCharacterMovement()->BrakingFrictionFactor = 1.0f;
	GetCharacterMovement()->bCanWalkOffLedges = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 640.0f, 0.0f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
}

void ATwinStickCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 初始更新物品计数
	UpdateItems();
}

void ATwinStickCharacter::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	/** 清除自动射击定时器 */
	GetWorld()->GetTimerManager().ClearTimer(AutoFireTimer);
}

void ATwinStickCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// 保存玩家控制器引用
	PlayerController = Cast<APlayerController>(GetController());
}

// 每帧更新角色旋转：鼠标瞄准时追踪鼠标位置，手柄瞄准时使用 AimAngle
void ATwinStickCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 获取当前旋转
	const FRotator OldRotation = GetActorRotation();

	// 是否使用鼠标瞄准？
	if (bUsingMouse)
	{
		if (PlayerController)
		{
			// 获取鼠标光标所在的世界位置
			FHitResult OutHit;
			PlayerController->GetHitResultUnderCursorByChannel(MouseAimTraceChannel, true, OutHit);

			// 计算瞄准旋转
			const FRotator AimRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), OutHit.Location);

			// 保存瞄准角度
			AimAngle = AimRot.Yaw;



			// 仅更新偏航角，保留俯仰和翻滚
			SetActorRotation(FRotator(OldRotation.Pitch, AimAngle, OldRotation.Roll));

		}

	} else {

		// 使用四元数插值平滑过渡到目标旋转
		const FRotator TargetRot = FRotator(OldRotation.Pitch, AimAngle, OldRotation.Roll);

		SetActorRotation(TargetRot);
	}
}

// 绑定增强输入系统的各个动作
void ATwinStickCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 设置增强输入的动作绑定
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATwinStickCharacter::Move);
		EnhancedInputComponent->BindAction(StickAimAction, ETriggerEvent::Triggered, this, &ATwinStickCharacter::StickAim);
		EnhancedInputComponent->BindAction(MouseAimAction, ETriggerEvent::Triggered, this, &ATwinStickCharacter::MouseAim);
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &ATwinStickCharacter::Dash);
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Triggered, this, &ATwinStickCharacter::Shoot);
		EnhancedInputComponent->BindAction(AoEAction, ETriggerEvent::Triggered, this, &ATwinStickCharacter::AoEAttack);

	}

}

void ATwinStickCharacter::Move(const FInputActionValue& Value)
{
	// 保存输入向量
	FVector2D InputVector = Value.Get<FVector2D>();

	// 转发输入
	DoMove(InputVector.X, InputVector.Y);
}

void ATwinStickCharacter::StickAim(const FInputActionValue& Value)
{
	// 获取输入向量
	FVector2D InputVector = Value.Get<FVector2D>();

	// 转发输入
	DoAim(InputVector.X, InputVector.Y);
}

void ATwinStickCharacter::MouseAim(const FInputActionValue& Value)
{
	// 标记为鼠标控制
	bUsingMouse = true;

	// 显示鼠标光标
	if (PlayerController)
	{
		PlayerController->SetShowMouseCursor(true);
	}
}

void ATwinStickCharacter::Dash(const FInputActionValue& Value)
{
	// 转发输入
	DoDash();
}

void ATwinStickCharacter::Shoot(const FInputActionValue& Value)
{
	// 转发输入
	DoShoot();
}

void ATwinStickCharacter::AoEAttack(const FInputActionValue& Value)
{
	// 转发输入
	DoAoEAttack();
}

// 根据轴输入执行角色移动，基于控制器旋转计算方向
void ATwinStickCharacter::DoMove(float AxisX, float AxisY)
{
	// 保存输入
	LastMoveInput.X = AxisX;
	LastMoveInput.Y = AxisY;

	// 计算输入的前向分量
	FRotator FlatRot = GetControlRotation();
	FlatRot.Pitch = 0.0f;

	// 应用前向移动
	AddMovementInput(FlatRot.RotateVector(FVector::ForwardVector), AxisX);

	// 应用右向移动
	AddMovementInput(FlatRot.RotateVector(FVector::RightVector), AxisY);
}

// 从轴输入计算瞄准角度，并触发自动射击
void ATwinStickCharacter::DoAim(float AxisX, float AxisY)
{
	// 从输入计算瞄准角度
	AimAngle = FMath::RadiansToDegrees(FMath::Atan2(AxisY, -AxisX));

	// 取消鼠标控制标志
	bUsingMouse = false;

	// 隐藏鼠标光标
	if (PlayerController)
	{
		PlayerController->SetShowMouseCursor(false);
	}

	// 是否处于自动射击冷却中？
	if (!bAutoFireActive)
	{
		// 开始冷却
		bAutoFireActive = true;

		// 发射子弹
		DoShoot();

		// 设定冷却重置定时器
		GetWorld()->GetTimerManager().SetTimer(AutoFireTimer, this, &ATwinStickCharacter::ResetAutoFire, AutoFireDelay, false);
	}
}

// 根据上次移动输入方向执行冲刺
void ATwinStickCharacter::DoDash()
{
	// 基于上次移动输入计算发射冲击向量
	FVector LaunchDir = FVector::ZeroVector;

	LaunchDir.X = FMath::Clamp(LastMoveInput.X, -1.0f, 1.0f);
	LaunchDir.Y = FMath::Clamp(LastMoveInput.Y, -1.0f, 1.0f);

	// 沿选择方向发射角色
	LaunchCharacter(LaunchDir * DashImpulse, true, true);
}

// 在角色前方生成一颗子弹
void ATwinStickCharacter::DoShoot()
{
	// 获取 actor 变换
	FTransform ProjectileTransform = GetActorTransform();

	// 应用子弹生成偏移
	FVector ProjectileLocation = ProjectileTransform.GetLocation() + ProjectileTransform.GetRotation().RotateVector(FVector::ForwardVector * ProjectileOffset);
	ProjectileTransform.SetLocation(ProjectileLocation);

	// 如果与墙壁等障碍物碰撞则不生成
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;

	ATwinStickProjectile* Projectile = GetWorld()->SpawnActor<ATwinStickProjectile>(ProjectileClass, ProjectileTransform, SpawnParams);
}

// 消耗一个物品执行 AoE 范围攻击
void ATwinStickCharacter::DoAoEAttack()
{
	// 是否有足够的物品执行 AoE 攻击？
	if (Items > 0)
	{
		// 获取游戏时间
		const float GameTime = GetWorld()->GetTimeSeconds();

		// 是否已过冷却？
		if (GameTime - LastAoETime > AoECooldownTime)
		{
			// 更新上次 AoE 时间
			LastAoETime = GameTime;

			// 生成 AoE 攻击
			ATwinStickAoEAttack* AoE = GetWorld()->SpawnActor<ATwinStickAoEAttack>(AoEAttackClass, GetActorTransform());

			// 减少物品计数
			--Items;

			// 更新 UI 物品计数
			UpdateItems();
		}
	}
}

// 处理伤害：施加击退并通知蓝图
void ATwinStickCharacter::HandleDamage(float Damage, const FVector& DamageDirection)
{
	// 计算击退向量
	FVector LaunchVector = DamageDirection;
	LaunchVector.Z = 0.0f;

	// 施加击退
	LaunchCharacter(LaunchVector * KnockbackStrength, true, true);

	// 交由蓝图处理
	BP_Damaged();
}

// 增加物品计数并更新 UI
void ATwinStickCharacter::AddPickup()
{
	// 增加物品数量
	++Items;

	// 更新物品计数
	UpdateItems();
}

// 通知 GameMode 当前物品数量
void ATwinStickCharacter::UpdateItems()
{
	// 更新 GameMode
	if (ATwinStickGameMode* GM = Cast<ATwinStickGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GM->ItemUsed(Items);
	}
}

// 重置自动射击标志
void ATwinStickCharacter::ResetAutoFire()
{
	// 重置自动射击标志
	bAutoFireActive = false;
}
