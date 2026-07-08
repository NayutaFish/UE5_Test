// Copyright Epic Games, Inc. All Rights Reserved.


#include "TwinStickPickup.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "TwinStickCharacter.h"
#include "Components/StaticMeshComponent.h"

// 构造函数：创建碰撞球体和视觉网格体
ATwinStickPickup::ATwinStickPickup()
{
 	PrimaryActorTick.bCanEverTick = true;

	// 创建根组件
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// 创建碰撞球体
	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere"));
	CollisionSphere->SetupAttachment(RootComponent);

	CollisionSphere->SetSphereRadius(100.0f);
	CollisionSphere->SetRelativeLocation(FVector(0.0f, 0.0f, 125.0f));
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionSphere->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// 创建网格体
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(CollisionSphere);

	Mesh->SetCollisionProfileName(FName("NoCollision"));

}

// 与玩家角色重叠时增加物品计数并销毁自身
void ATwinStickPickup::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	// 是否与玩家角色重叠？
	if (ATwinStickCharacter* PlayerCharacter = Cast<ATwinStickCharacter>(OtherActor))
	{
		// 给玩家增加物品
		PlayerCharacter->AddPickup();

		// 销毁拾取物
		Destroy();
	}
}
