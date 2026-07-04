// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
//#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"          // ← 父类：APlayerController
#include "Test_GamePlayPlayerController.generated.h" // ← UE反射代码（必须最后include）

// 前置声明（告诉编译器"有这些类"，避免互相include）
class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;
class UPathFollowingComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 * 俯视角点击移动的玩家控制器
 *
 * UE把"玩家"拆成两个角色：
 *   PlayerController（大脑）← 这个类：处理输入、控制哪个Pawn
 *   Character/Pawn（身体） ← 负责物理碰撞、网格显示、动画
 *
 * 类比 Unity：相当于把 Input 处理和 GameObject 控制分开了
 */
UCLASS(abstract)
class ATest_GamePlayPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	/** 导航路径跟随组件——负责让Actor沿着NavMesh走到目标点 */
	UPROPERTY(VisibleDefaultsOnly, Category = AI)
	TObjectPtr<UPathFollowingComponent> PathFollowingComponent;

	/** 短按/长按的时间阈值
	 *  按住时间 ≤ 此值 → 短按（寻路移动到目标点）
	 *  按住时间 > 此值 → 长按（持续跟随鼠标方向移动） */
	UPROPERTY(EditAnywhere, Category="Input")
	float ShortPressThreshold;

	/** 鼠标点击位置的粒子特效
	 *  在 UE 编辑器中拖拽一个 NiagaraSystem 资产到这里 */
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UNiagaraSystem> FXCursor;

	/** 增强输入（EnhancedInput）的输入映射上下文
	 *  相当于 Unity Input System 的 Input Action Asset */
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	/** 鼠标点击的输入动作 */
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> SetDestinationClickAction;

	/** 触摸输入的输入动作 */
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> SetDestinationTouchAction;

	/** 是否让控制的Pawn移向鼠标光标方向 */
	uint32 bMoveToMouseCursor : 1;

	/** 当前是否正在用触摸输入 */
	uint32 bIsTouch : 1;

	/** 缓存的移动目标位置（鼠标点击的世界坐标） */
	FVector CachedDestination;

	/** 当前按住输入的时间（秒） */
	float FollowTime = 0.0f;

public:

	/** 构造函数 */
	ATest_GamePlayPlayerController();

protected:

	/** 绑定输入（UE 生命周期函数）
	 *  类比 Unity：相当于在 Start() 里做 Input Action 的绑定
	 *  将 UInputAction 绑到对应的处理函数上 */
	virtual void SetupInputComponent() override;

	/** 输入处理函数们 */
	void OnInputStarted();              // 刚按下时
	void OnSetDestinationTriggered();   // 按住中（每帧触发）
	void OnSetDestinationReleased();    // 松开时
	void OnTouchTriggered();            // 触摸按住
	void OnTouchReleased();             // 触摸松开

	/** 计算鼠标/触摸位置在世界空间的目标点（射线检测地面） */
	void UpdateCachedDestination();
};
