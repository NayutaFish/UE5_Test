// Fill out your copyright notice in the Description page of Project Settings.


#include "HikariPlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AHikariPlayerCharacter::AHikariPlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//禁止角色旋转跟随控制器旋转
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	//让角色的朝向跟随移动方向
	GetCharacterMovement()->bOrientRotationToMovement = true;
	// 不使用 Controller Desired Rotation，避免和移动朝向旋转打架
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	// 转身速度，Z 轴是水平转向
	//Frotator(Pitch,Yaw,Roll)
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

// Called when the game starts or when spawned
void AHikariPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

// Called every frame
void AHikariPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AHikariPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// PlayerInputComponent 是 UE 传进来的输入组件
	// 但它的类型比较基础：UInputComponent
	// 我们现在用的是 Enhanced Input，所以需要把它转换成 UEnhancedInputComponent
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	// 如果转换失败，说明当前输入组件不是 Enhanced Input 类型
	// 为了避免空指针崩溃，直接 return
	if (!EnhancedInputComponent)
	{
		return;
	}

	// 如果蓝图里指定了 MoveHorizontalAction
	// 也就是 IA_MoveHorizontal
	if (MoveHorizontalAction)
	{
		// 当 MoveHorizontalAction 被触发时
		// 调用当前角色类里的 MoveHorizontal 函数
		EnhancedInputComponent->BindAction(
			MoveHorizontalAction,
			ETriggerEvent::Triggered,
			this,
			&AHikariPlayerCharacter::MoveHorizontal
		);
	}

	// 如果蓝图里指定了 MoveVerticalAction
	// 也就是 IA_MoveVertical
	if (MoveVerticalAction)
	{
		// 当 MoveVerticalAction 被触发时
		// 调用当前角色类里的 MoveVertical 函数
		EnhancedInputComponent->BindAction(
			MoveVerticalAction,
			ETriggerEvent::Triggered,
			this,
			&AHikariPlayerCharacter::MoveVertical
		);
	}
	if (SprintAction)
{
	// 按下 Shift 的瞬间触发
	// Started 适合“开始某个行为”
	EnhancedInputComponent->BindAction(
		SprintAction,
		ETriggerEvent::Started,
		this,
		&AHikariPlayerCharacter::StartSprint
	);

	// 松开 Shift 时触发
	// Completed 适合“正常结束某个输入”
	EnhancedInputComponent->BindAction(
		SprintAction,
		ETriggerEvent::Completed,
		this,
		&AHikariPlayerCharacter::StopSprint
	);

	// 输入被打断时触发
	// 比如某些情况下输入系统取消了这个动作
	// 为了保险，也恢复普通速度
	EnhancedInputComponent->BindAction(
		SprintAction,
		ETriggerEvent::Canceled,
		this,
		&AHikariPlayerCharacter::StopSprint
	);
}
}

void AHikariPlayerCharacter::MoveHorizontal(const FInputActionValue& Value)
{
	// 读取 A / D 的输入值
	// A 通常是 -1
	// D 通常是  1
	const float AxisValue = Value.Get<float>();

	if (FMath::IsNearlyZero(AxisValue))
	{
		return;
	}

	// 屏幕右方向
	// 你的镜头下：
	// 世界 Right 是屏幕右上
	// 世界 Forward 是屏幕左上
	// 所以 Right - Forward 就是屏幕右方
	const FVector ScreenRightDirection =
		(FVector::RightVector - FVector::ForwardVector).GetSafeNormal();

	AddMovementInput(ScreenRightDirection, AxisValue);
}

void AHikariPlayerCharacter::MoveVertical(const FInputActionValue& Value)
{
	// 读取 W / S 的输入值
	// W 通常是  1
	// S 通常是 -1
	const float AxisValue = Value.Get<float>();

	if (FMath::IsNearlyZero(AxisValue))
	{
		return;
	}

	// 屏幕上方向
	// 你的镜头下：
	// 世界 Forward 是屏幕左上
	// 世界 Right 是屏幕右上
	// 所以 Forward + Right 就是屏幕正上方
	const FVector ScreenUpDirection =
		(FVector::ForwardVector + FVector::RightVector).GetSafeNormal();

	AddMovementInput(ScreenUpDirection, AxisValue);
}
void AHikariPlayerCharacter::StartSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AHikariPlayerCharacter::StopSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}
