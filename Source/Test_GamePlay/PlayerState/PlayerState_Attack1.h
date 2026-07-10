// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Common/StateBase.h"
#include "PlayerState_Attack1.generated.h"

/**
 * 攻击状态：播放攻击动画，0.5s 后根据 WASD 输入切回 Idle 或 Move
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class TEST_GAMEPLAY_API UPlayerState_Attack1 : public UStateBase
{
	GENERATED_BODY()

protected:
	virtual void OnEnter_Implementation() override;
	virtual void OnExit_Implementation() override;
	virtual void Update_Implementation(float DeltaTime) override;

private:
	void OnMoveX(float Value);
	void OnMoveY(float Value);
	void OnAttackTimer();

	bool bHadMoveInput = false;
	FVector SlideDirection = FVector::ZeroVector;
	FTimerHandle AttackTimerHandle;
};
