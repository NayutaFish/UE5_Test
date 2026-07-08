// Fill out your copyright notice in the Description page of Project Settings.


#include "HikariPlayerCharacter.h"
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

}

