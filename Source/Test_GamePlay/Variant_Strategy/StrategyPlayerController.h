// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "StrategyPlayerController.generated.h"

class AStrategyPawn;
class UInputMappingContext;
class UNiagaraSystem;
struct FInputActionValue;
class AStrategyHUD;
class UInputAction;
struct FInputActionInstance;
class AStrategyUnit;
class UStrategyTouchControls;

/**
 * 俯视角策略游戏的玩家控制器
 * 处理单位选择和命令下达
 * 同时支持鼠标和触控输入方式
 */
UCLASS(abstract)
class AStrategyPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	/** 与此控制器关联的策略摄像机 Pawn */
	TObjectPtr<AStrategyPawn> ControlledCameraPawn;

	/** 与此控制器关联的策略 HUD */
	TObjectPtr<AStrategyHUD> StrategyHUD;

	/** 鼠标输入映射上下文 */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputMappingContext* MouseMappingContext;

	/** 触控输入映射上下文 */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputMappingContext* TouchMappingContext;

	/** 是否为增减选择模式（添加或移除已选单位） */
	bool bSelectionModifier = false;

	/** 是否处于双击全选模式 */
	bool bDoubleTapActive = false;

	/** 是否允许与游戏对象交互 */
	bool bAllowInteraction = true;

	/** 移动摄像机的输入动作 */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveCameraAction;

	/** 缩放摄像机的输入动作 */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* ZoomCameraAction;

	/** 重置摄像机至默认位置的输入动作 */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* ResetCameraAction;

	/** 点击选择的输入动作 */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* SelectClickAction;

	/** 附加点击选择（增加/移除选中的单位）的输入动作 */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* SelectClickAdditiveAction;

	/** 双击全选的输入动作 */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* SelectAllDoubleClickAction;

	/** 按下并拖动选择的输入动作 */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* SelectHoldAction;

	/** 点击交互的输入动作 */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* InteractClickAction;

	/** 按住交互的输入动作 */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* InteractHoldAction;

	/** 切换选择模式的输入动作 */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* SelectionModifierAction;

	/** 触控主手指按住的输入动作 */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* TouchPrimaryHoldAction;

	/** 触控副手指的输入动作 */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* TouchSecondaryAction;

	/** 移动端触控控件小部件指针 */
	TObjectPtr<UStrategyTouchControls> MobileControlsWidget;

	/** 移动端触控控件小部件类 */
	UPROPERTY(EditAnywhere, Category="Input")
	TSubclassOf<UStrategyTouchControls> MobileControlsWidgetClass;

	/** 是否强制使用触控控制 */
	UPROPERTY(EditAnywhere, Category="Input")
	bool bForceTouchControls = false;

	/** 点击或触控时搜索附近单位的最远距离 */
	UPROPERTY(EditAnywhere, Category="Input", meta = (ClampMin = 0, ClampMax = 10000, Units = "cm"))
	float SelectionRadius = 250.0f;

	/** 摄像机拖拽滚动的起始位置缓存 */
	FVector2D StartingDragScrollPosition;

	/** 框选的起始位置缓存 */
	FVector2D StartingBoxSelectionPosition;

	/** 上次拖拽滚动的游戏时间，避免拖拽结束时误触发命令 */
	float LastTouchDragScrollTime = 0.0f;

	/** 手指需按住的时长以触发拖拽滚动 */
	UPROPERTY(EditAnywhere, Category="Input", meta = (ClampMin = 0, ClampMax = 1, Units = "s"))
	float TouchDragScrollHoldTime = 0.15f;

	/** 当前摄像机缩放级别 */
	float CameraZoom = 0.0f;

	/** 默认摄像机缩放级别 */
	float DefaultZoom = 1000.0f;

	/** 摄像机最小缩放级别 */
	UPROPERTY(EditAnywhere, Category = "Camera", meta = (ClampMin = 0, ClampMax = 10000))
	float MinZoomLevel = 1000.0f;

	/** 摄像机最大缩放级别 */
	UPROPERTY(EditAnywhere, Category = "Camera", meta = (ClampMin = 0, ClampMax = 10000))
	float MaxZoomLevel = 2500.0f;

	/** 缩放输入的缩放倍率 */
	UPROPERTY(EditAnywhere, Category = "Camera", meta = (ClampMin = 0, ClampMax = 1000))
	float ZoomScaling = 100.0f;

	/** 鼠标拖拽移动摄像机的速率 */
	UPROPERTY(EditAnywhere, Category = "Camera", meta = (ClampMin = 0, ClampMax = 10000))
	float DragMultiplier = 0.1f;

	/** 选择检测使用的碰撞通道 */
	UPROPERTY(EditAnywhere, Category = "Selection")
	TEnumAsByte<ETraceTypeQuery> SelectionTraceChannel;

	/** 当前选中的单位列表 */
	TArray<AStrategyUnit*> ControlledUnits;

public:

	/** 构造函数 */
	AStrategyPlayerController();

	/** BeginPlay 初始化 */
	virtual void BeginPlay() override;

	/** 初始化输入绑定 */
	virtual void SetupInputComponent() override;

	/** 操控 Pawn 时的初始化 */
	virtual void OnPossess(APawn* InPawn);

public:

	/** 通过 HUD 拖拽框选来更新选中的单位 */
	void DragSelectUnits(const TArray<AStrategyUnit*>& Units);

	/** 返回当前选中的单位列表 */
	const TArray<AStrategyUnit*>& GetSelectedUnits();

	/** 返回默认摄像机缩放的百分比值 */
	float GetDefaultZoomPercentage() const;

protected:

	/** 返回当前玩家控制器是否应使用触控控制 */
	bool ShouldUseTouchControls() const;

	// 鼠标 + 键盘输入

protected:

	/** 按给定输入移动摄像机 */
	void MoveCamera(const FInputActionValue& Value);

	/** 按给定输入改变摄像机缩放级别 */
	void ZoomCamera(const FInputActionValue& Value);

	/** 重置摄像机至初始值 */
	void ResetCamera(const FInputActionValue& Value);

	/** 开始按下选择（框选起点） */
	void SelectHoldStarted(const FInputActionValue& Value);

	/** 按下选择持续触发（更新框选大小） */
	void SelectHoldTriggered(const FInputActionValue& Value);

	/** 按下选择完成（结束框选） */
	void SelectHoldCompleted(const FInputActionValue& Value);

	/** 点击选择操作 */
	void SelectClick(const FInputActionValue& Value);

	/** 附加点击选择操作 */
	void SelectClickAdditive(const FInputActionValue& Value);

	/** 双击全选操作 */
	void SelectAllDoubleClick(const FInputActionValue& Value);

	/** 开始按住交互操作 */
	void InteractHoldStarted(const FInputActionValue& Value);

	/** 按住交互持续触发 */
	void InteractHoldTriggered(const FInputActionValue& Value);

	/** 点击交互操作 */
	void InteractClick(const FInputActionValue& Value);

	/** 触控主手指按住开始 */
	void TouchPrimaryHoldStarted(const FInputActionValue& Value);

	/** 触控主手指按住持续触发 */
	void TouchPrimaryHoldTriggered(const FInputActionInstance& Instance);

	/** 触控主手指按住完成 */
	void TouchPrimaryHoldCompleted(const FInputActionValue& Value);

	/** 触控副手指持续触发 */
	void TouchSecondaryTriggered(const FInputActionValue& Value);

	/** 触控副手指完成 */
	void TouchSecondaryCompleted(const FInputActionValue& Value);

	// 命令

public:

	/** 尝试在指定位置选择或取消选择一个单位，支持附加选择模式 */
	bool DoSelectCommand(const FVector& SelectLocation, bool bAdditiveSelection);

	/** 尝试选中屏幕上的所有单位 */
	void DoSelectAllUnitsOnScreenCommand();

	/** 取消选择所有已选单位 */
	void DoDeselectAllUnitsCommand();

	/** 在全选和取消全选之间切换 */
	void DoToggleSelectAllUnitsCommand();

	/** 根据新的屏幕坐标滚动摄像机 */
	void DoCameraDragScrollCommand(const FVector2D& CurrentCursorPosition);

	/** 尝试将选中的所有单位移动到指定位置 */
	void DoMoveUnitsCommand(const FVector& GoalLocation);

	/** 对摄像机应用缩放变化 */
	void DoCameraModifyZoomCommand(float ZoomDelta);

	/** 重置摄像机缩放至默认值 */
	void DoCameraResetZoomCommand();

	/** 将摄像机缩放设置为最小和最大之间的百分比 */
	void DoCameraSetZoomPercentageCommand(float Percentage);

protected:

	/** 按距离目标位置的远近，对已选单位排序并返回最近的单位 */
	AStrategyUnit* GetClosestSelectedUnitToLocation(FVector TargetLocation);

	/** 计算并返回当前鼠标位置 */
	FVector2D GetMouseLocationForPlayer();

	/** 尝试获取光标下方的世界坐标位置，成功返回 true */
	bool GetLocationUnderCursor(FVector& Location);

	/** 尝试获取手指下方的世界坐标位置，成功返回 true */
	bool GetLocationUnderFinger(FVector& Location);

	/** 将当前触控位置投射到世界空间 */
	FVector ProjectTouchPointToWorldSpace();

protected:

	/** 生成光标反馈特效（蓝图可覆写） */
	UFUNCTION(BlueprintImplementableEvent, Category="Cursor", meta = (DisplayName="Cursor Feedback"))
	void BP_CursorFeedback(FVector Location, bool bPositive);
};
