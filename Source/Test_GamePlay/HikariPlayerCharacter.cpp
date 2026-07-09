// Fill out your copyright notice in the Description page of Project Settings.


#include "HikariPlayerCharacter.h"
#include "Common/AttackAreaBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"

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
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	
}

// Called when the game starts or when spawned
void AHikariPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
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

	// 右键攻击（DefaultInput.ini 已配好 RightClickAttack = 鼠标右键）
	PlayerInputComponent->BindAction("RightClickAttack", IE_Pressed, this, &AHikariPlayerCharacter::OnAttack);
}

void AHikariPlayerCharacter::OnAttack()
{
	if (bIsDead || !AttackAreaClass) return;

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = this;

	if (AAttackAreaBase* AttackArea = GetWorld()->SpawnActor<AAttackAreaBase>(
			AttackAreaClass,
			GetActorLocation() + GetActorForwardVector() * 100.0f,
			GetActorRotation(),
			Params))
	{
		AttackArea->Initialize(3.0f, 200.0f, 300.0f, true);
	}
}

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
