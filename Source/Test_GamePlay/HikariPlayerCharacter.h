// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HikariPlayerCharacter.generated.h"

class UInputAction;
struct FInputActionValue;

UCLASS()
class TEST_GAMEPLAY_API AHikariPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AHikariPlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    //用于接收蓝图中的IA_MoveHorizontal和IA_MoveVertical的输入
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Input")
	TObjectPtr<UInputAction> MoveHorizontalAction;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Input")
	TObjectPtr<UInputAction> MoveVerticalAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> SprintAction;
	// 普通移动速度
    // EditDefaultsOnly 表示可以在 BP_Hikari 类默认值里调整
UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
float WalkSpeed = 600.0f;

// 疾跑速度
// 按住 Shift 时，把 MaxWalkSpeed 设置成这个值
UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
float SprintSpeed = 900.0f;
private:
    void MoveHorizontal(const FInputActionValue& Value);
	void MoveVertical(const FInputActionValue& Value);
	// 开始疾跑：按下 Shift 时调用
void StartSprint();

// 停止疾跑：松开 Shift 时调用
void StopSprint();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};
