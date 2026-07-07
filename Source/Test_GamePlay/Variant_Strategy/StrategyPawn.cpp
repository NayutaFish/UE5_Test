// Copyright Epic Games, Inc. All Rights Reserved.


#include "StrategyPawn.h"
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/FloatingPawnMovement.h"

AStrategyPawn::AStrategyPawn()
{
 	PrimaryActorTick.bCanEverTick = true;

	// 创建根组件
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// 创建摄像机组件
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(RootComponent);

	// 创建浮动移动组件
	FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Floating Pawn Movement"));

	// 配置摄像机为正交投影模式
	Camera->ProjectionMode = ECameraProjectionMode::Orthographic;
	Camera->OrthoWidth = 1500.0f;
	Camera->AutoPlaneShift = 1.0f;
	Camera->bUpdateOrthoPlanes = false;

	// 配置移动组件：限制在水平平面上移动
	FloatingPawnMovement->bConstrainToPlane = true;
	FloatingPawnMovement->SetPlaneConstraintNormal(FVector::UpVector);
	FloatingPawnMovement->SetPlaneConstraintOrigin(FVector::UpVector * 1500.0f);
}

void AStrategyPawn::SetZoomModifier(float Value)
{
	// 设置摄像机的正交宽度（实现缩放效果）
	Camera->SetOrthoWidth(Value);
}
