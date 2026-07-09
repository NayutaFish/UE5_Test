// Fill out your copyright notice in the Description page of Project Settings.


#include "HikariPlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"

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

	if (AttackAction)
	{
		// 鼠标左键按下时触发攻击输入
		// IA_Attack 是 Boolean 类型，所以用 Started 即可
		EnhancedInputComponent->BindAction(
			AttackAction,
			ETriggerEvent::Started,
			this,
			&AHikariPlayerCharacter::OnAttackInput
		);
	}
}

void AHikariPlayerCharacter::MoveHorizontal(const FInputActionValue& Value)
{
	// 读取 A / D 的输入值
	// A 通常是 -1
	// D 通常是  1
	const float AxisValue = Value.Get<float>();

	if (!CanMove())
	{
		return;
	}

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

	if (!CanMove())
	{
		return;
	}

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
	// Shift 可以中断攻击
	if (CurrentActionState == EHikariActionState::Attacking)
	{
		CancelAttack();
	}

	// 如果取消攻击后仍然不能移动，就不进入疾跑
	// 例如以后 Dead / HitStun / Dodging 状态
	if (!CanMove())
	{
		return;
	}

	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AHikariPlayerCharacter::StopSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

bool AHikariPlayerCharacter::CanMove() const
{
	// 第一版规则：
	// 只有 Normal 状态可以移动
	// Attacking / Dodging / HitStun / Dead 都不能移动
	return CurrentActionState == EHikariActionState::Normal;
}

bool AHikariPlayerCharacter::CanStartAction() const
{
	// 第一版规则：
	// 只有 Normal 状态可以开始新的动作
	// 这样可以防止攻击中再次攻击
	return CurrentActionState == EHikariActionState::Normal;
}

void AHikariPlayerCharacter::SetActionState(EHikariActionState NewState)
{
	// 如果新状态和当前状态一样，就不用重复设置
	if (CurrentActionState == NewState)
	{
		return;
	}

	CurrentActionState = NewState;
}

void AHikariPlayerCharacter::OnAttackInput()
{
	// 输入函数只负责接收输入
	// 真正的攻击逻辑交给 BeginAttack
	//UE_LOG(LogTemp, Warning, TEXT("Attack Input Received"));//用于测试
	BeginAttack();
	
}

void AHikariPlayerCharacter::BeginAttack()
{
	//UE_LOG(LogTemp, Warning, TEXT("BeginAttack Called"));//用于测试
	// 如果当前不能开始动作，就直接结束
	// 例如：攻击中、闪避中、受击中、死亡
	if (!CanStartAction())
	{
		return;
	}

	// 如果 BP_Hikari 里没有指定 AttackMontage，就不能播放攻击动画
	if (!AttackMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("AttackMontage is not set."));
		return;
	}

	// GetMesh() 是角色的骨骼网格体组件
	// GetAnimInstance() 是这个 Mesh 当前使用的动画蓝图实例
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance)
	{
		return;
	}

	// 进入攻击状态
	SetActionState(EHikariActionState::Attacking);

	// 播放攻击 Montage
	// 返回值是 Montage 的播放时长
	const float MontageLength = PlayAnimMontage(AttackMontage);

	// 如果播放失败，MontageLength 可能小于等于 0
	// 这时要把状态恢复成 Normal，避免角色卡死在 Attacking
	if (MontageLength <= 0.0f)
	{
		SetActionState(EHikariActionState::Normal);
		return;
	}

	// 创建 Montage 结束委托
	// 作用是：当这个 Montage 播放结束时，自动调用 OnAttackMontageEnded
	FOnMontageEnded MontageEndedDelegate;
	MontageEndedDelegate.BindUObject(this, &AHikariPlayerCharacter::OnAttackMontageEnded);

	// 把结束委托绑定到 AttackMontage
	AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, AttackMontage);

	UE_LOG(LogTemp, Log, TEXT("Hikari Begin Attack"));
}

void AHikariPlayerCharacter::EndAttack()
{
	// 如果当前不是攻击状态，就不处理
	if (CurrentActionState != EHikariActionState::Attacking)
	{
		return;
	}

	// 攻击结束，恢复 Normal 状态
	SetActionState(EHikariActionState::Normal);

	UE_LOG(LogTemp, Log, TEXT("Hikari End Attack"));
}

void AHikariPlayerCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// 确认结束的是我们的攻击 Montage
	// 避免以后别的 Montage 结束时误触发 EndAttack
	if (Montage != AttackMontage)
	{
		return;
	}

	EndAttack();
}

void AHikariPlayerCharacter::CancelAttack()
{
	// 只有攻击状态才需要取消
	if (CurrentActionState != EHikariActionState::Attacking)
	{
		return;
	}

	// 停止攻击 Montage
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->Montage_Stop(AttackCancelBlendOutTime, AttackMontage);
	}

	// 恢复正常状态
	SetActionState(EHikariActionState::Normal);

	UE_LOG(LogTemp, Log, TEXT("Hikari Attack Canceled"));
}
