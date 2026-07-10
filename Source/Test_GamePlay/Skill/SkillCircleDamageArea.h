// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkillCircleDamageArea.generated.h"

class USphereComponent;

/** Short-lived, player-owned radial damage area for Circular Slash. */
UCLASS(Blueprintable)
class TEST_GAMEPLAY_API ASkillCircleDamageArea : public AActor
{
	GENERATED_BODY()

public:
	ASkillCircleDamageArea();

	UFUNCTION(BlueprintCallable, Category = "SkillArea")
	void Initialize(float InRadius, float InDamage, float InLifeTime, AActor* InInstigator);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SkillArea")
	TObjectPtr<USphereComponent> CollisionSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillArea", meta = (ClampMin = "0.0"))
	float Damage = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillArea", meta = (ClampMin = "1.0"))
	float Radius = 260.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillArea", meta = (ClampMin = "0.01"))
	float LifeTime = 0.25f;

	/** Development-only wireframe preview; compiled out when debug drawing is disabled. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillArea|Debug")
	bool bDrawDebugArea = true;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

private:
	void TryDamageActor(AActor* OtherActor);

	UPROPERTY(Transient)
	TObjectPtr<AActor> DamageInstigator;

	TSet<TWeakObjectPtr<AActor>> HitActors;
};
