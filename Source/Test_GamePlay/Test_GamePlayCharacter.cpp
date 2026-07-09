// Copyright Epic Games, Inc. All Rights Reserved.

#include "Test_GamePlayCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"

// ────── 构造函数（类比 Unity：Awake，但场景未加载）──────
ATest_GamePlayCharacter::ATest_GamePlayCharacter()
{
	// 设置胶囊体碰撞大小（半径42，高96）
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCollisionObjectType(ECC_GameTraceChannel3);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

	// 不让角色随控制器旋转（由鼠标控制方向）
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 配置角色移动组件
	GetCharacterMovement()->bOrientRotationToMovement = true;   // 面朝移动方向
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f); // 转身速度
	GetCharacterMovement()->bConstrainToPlane = true;           // 限制在平面上
	GetCharacterMovement()->bSnapToPlaneAtStart = true;         // 开始时吸附到平面

	// ── 创建弹簧臂（Camera Boom）──
	// 类似 Unity 把摄像机作为子物体挂到 Player 上并抬高
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true);   // 弹簧臂自身不旋转
	CameraBoom->TargetArmLength = 800.f;          // 摄像机距离角色800单位
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f)); // 俯视60度
	CameraBoom->bDoCollisionTest = false;          // 不进行碰撞检测

	// ── 创建摄像机 ──
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false;  // 摄像机不随控制器转

	// 开启 Tick（每帧更新）
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ATest_GamePlayCharacter::BeginPlay()
{
	Super::BeginPlay();
	// 初始化逻辑可以写在这里
}

void ATest_GamePlayCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
	// 每帧逻辑可以写在这里
}
