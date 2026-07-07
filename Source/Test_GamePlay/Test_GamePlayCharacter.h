// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Test_GamePlayCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;

/**
 * 俯视角玩家角色
 *
 * 继承自 ACharacter（自带 CharacterMovementComponent 移动组件）
 *
 * 功能：
 *   - 创建摄像机与弹簧臂（实现俯视角）
 *   - 配置角色移动参数
 *   - 实际动画/模型等设置在蓝图子类中完成
 *
 * 类比 Unity：相当于挂在 Player GameObject 上的 MonoBehaviour
 * 但 UE 把"身体"（Character）和"大脑"（PlayerController）拆开了
 */
UCLASS(abstract)
class ATest_GamePlayCharacter : public ACharacter
{
	GENERATED_BODY()

private:

	/** 俯视角摄像机组件 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> TopDownCameraComponent;

	/** 弹簧臂——让摄像机固定在角色上方一定距离和角度
	 *  类似 Unity 中挂在 Camera 上的 SmoothFollow 脚本 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

public:

	/** 构造函数 */
	ATest_GamePlayCharacter();

	/** 游戏开始时调用 */
	virtual void BeginPlay() override;

	/** 每帧调用 */
	virtual void Tick(float DeltaSeconds) override;

	/** 获取摄像机组件 */
	UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent.Get(); }

	/** 获取弹簧臂组件 */
	USpringArmComponent* GetCameraBoom() const { return CameraBoom.Get(); }

};
