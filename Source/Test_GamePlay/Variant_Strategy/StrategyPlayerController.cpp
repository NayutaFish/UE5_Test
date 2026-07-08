// Copyright Epic Games, Inc. All Rights Reserved.


#include "StrategyPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "Camera/CameraComponent.h"
#include "StrategyPawn.h"
#include "Camera/CameraComponent.h"
#include "InputActionValue.h"
#include "StrategyHUD.h"
#include "Engine/CollisionProfile.h"
#include "Kismet/GameplayStatics.h"
#include "StrategyUnit.h"
#include "NavigationSystem.h"
#include "Engine/OverlapResult.h"
#include "InputAction.h"
#include "StrategyTouchControls.h"
#include "Widgets/Input/SVirtualJoystick.h"
#include "Test_GamePlay.h"

AStrategyPlayerController::AStrategyPlayerController()
{
	// 始终显示鼠标光标
	bShowMouseCursor = true;
}

void AStrategyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 仅在本地玩家控制器上生成触控控件
	if (IsLocalPlayerController() && ShouldUseTouchControls())
	{
		// 生成移动端触控控件小部件
		MobileControlsWidget = CreateWidget<UStrategyTouchControls>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// 将触控控件添加到玩家屏幕
			MobileControlsWidget->AddToPlayerScreen(0);

			// 设置移动端控件的玩家控制器指针
			MobileControlsWidget->SetPlayerController(this);

		} else {

			UE_LOG(LogTest_GamePlay, Error, TEXT("Could not spawn mobile controls widget."));

		}

	}

}

void AStrategyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// 仅在本地玩家控制器上设置输入绑定
	if (IsLocalPlayerController())
	{
		// 添加输入映射上下文
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			// 根据输入模式选择映射上下文
			UInputMappingContext* ChosenContext = nullptr;

			if (ShouldUseTouchControls())
			{
				ChosenContext = TouchMappingContext;
			}
			else
			{
				ChosenContext = MouseMappingContext;
			}

			Subsystem->AddMappingContext(ChosenContext, 0);
		}

		// 绑定输入映射
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
		{
			// 摄像机控制
			EnhancedInputComponent->BindAction(MoveCameraAction, ETriggerEvent::Triggered, this, &AStrategyPlayerController::MoveCamera);
			EnhancedInputComponent->BindAction(ZoomCameraAction, ETriggerEvent::Triggered, this, &AStrategyPlayerController::ZoomCamera);
			EnhancedInputComponent->BindAction(ResetCameraAction, ETriggerEvent::Triggered, this, &AStrategyPlayerController::ResetCamera);

			// 鼠标交互
			EnhancedInputComponent->BindAction(SelectHoldAction, ETriggerEvent::Started, this, &AStrategyPlayerController::SelectHoldStarted);
			EnhancedInputComponent->BindAction(SelectHoldAction, ETriggerEvent::Triggered, this, &AStrategyPlayerController::SelectHoldTriggered);
			EnhancedInputComponent->BindAction(SelectHoldAction, ETriggerEvent::Completed, this, &AStrategyPlayerController::SelectHoldCompleted);
			EnhancedInputComponent->BindAction(SelectHoldAction, ETriggerEvent::Canceled, this, &AStrategyPlayerController::SelectHoldCompleted);

			EnhancedInputComponent->BindAction(SelectClickAction, ETriggerEvent::Completed, this, &AStrategyPlayerController::SelectClick);

			EnhancedInputComponent->BindAction(SelectClickAdditiveAction, ETriggerEvent::Completed, this, &AStrategyPlayerController::SelectClickAdditive);

			EnhancedInputComponent->BindAction(SelectAllDoubleClickAction, ETriggerEvent::Completed, this, &AStrategyPlayerController::SelectAllDoubleClick);

			EnhancedInputComponent->BindAction(InteractHoldAction, ETriggerEvent::Started, this, &AStrategyPlayerController::InteractHoldStarted);
			EnhancedInputComponent->BindAction(InteractHoldAction, ETriggerEvent::Triggered, this, &AStrategyPlayerController::InteractHoldTriggered);

			EnhancedInputComponent->BindAction(InteractClickAction, ETriggerEvent::Completed, this, &AStrategyPlayerController::InteractClick);

			// 触控交互
			EnhancedInputComponent->BindAction(TouchPrimaryHoldAction, ETriggerEvent::Started, this, &AStrategyPlayerController::TouchPrimaryHoldStarted);
			EnhancedInputComponent->BindAction(TouchPrimaryHoldAction, ETriggerEvent::Triggered, this, &AStrategyPlayerController::TouchPrimaryHoldTriggered);
			EnhancedInputComponent->BindAction(TouchPrimaryHoldAction, ETriggerEvent::Completed, this, &AStrategyPlayerController::TouchPrimaryHoldCompleted);

			EnhancedInputComponent->BindAction(TouchSecondaryAction, ETriggerEvent::Triggered, this, &AStrategyPlayerController::TouchSecondaryTriggered);
			EnhancedInputComponent->BindAction(TouchSecondaryAction, ETriggerEvent::Completed, this, &AStrategyPlayerController::TouchSecondaryCompleted);
			EnhancedInputComponent->BindAction(TouchSecondaryAction, ETriggerEvent::Canceled, this, &AStrategyPlayerController::TouchSecondaryCompleted);

		}
	}
}

void AStrategyPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// 确保获得正确的 Pawn 类型
	ControlledCameraPawn = Cast<AStrategyPawn>(InPawn);
	check(ControlledCameraPawn);

	// 从摄像机的正交宽度初始化缩放级别
	DefaultZoom = CameraZoom = ControlledCameraPawn->GetCamera()->OrthoWidth;

	// 获取 HUD 指针
	StrategyHUD = Cast<AStrategyHUD>(GetHUD());

	// 如果有触控控件小部件，同步摄像机缩放
	if (MobileControlsWidget)
	{
		MobileControlsWidget->BP_SetZoomPercentage(GetDefaultZoomPercentage());
	}
}

void AStrategyPlayerController::DragSelectUnits(const TArray<AStrategyUnit*>& Units)
{
	// 检查是否选中了单位
	if (Units.Num() > 0)
	{
		// 先取消选择所有当前已选单位
		DoDeselectAllUnitsCommand();

		// 逐个选中新单位
		for (AStrategyUnit* CurrentUnit : Units)
		{
			// 将单位添加到选中列表
			ControlledUnits.Add(CurrentUnit);

			// 通知单位已被选中
			CurrentUnit->UnitSelected();
		}

	}
	else
	{

		// 框选区域没有单位，取消所有选择
		if (ControlledUnits.Num() > 0)
		{
			DoDeselectAllUnitsCommand();
		}

	}
}

const TArray<AStrategyUnit*>& AStrategyPlayerController::GetSelectedUnits()
{
	return ControlledUnits;
}

float AStrategyPlayerController::GetDefaultZoomPercentage() const
{
	float ZoomPct = (DefaultZoom - MinZoomLevel) / (MaxZoomLevel - MinZoomLevel);
	return FMath::Clamp(ZoomPct, 0.0f, 1.0f);
}

bool AStrategyPlayerController::ShouldUseTouchControls() const
{
	// 是否在移动端平台？或强制使用触控？
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}

void AStrategyPlayerController::MoveCamera(const FInputActionValue& Value)
{
	FVector2D InputVector = Value.Get<FVector2D>();

	// 获取前方向输入分量
	FRotator ForwardRot = GetControlRotation();
	ForwardRot.Pitch = 0.0f;

	// 获取右方向输入分量
	FRotator RightRot = GetControlRotation();
	RightRot.Pitch = 0.0f;
	RightRot.Roll = 0.0f;

	// 添加前方向移动输入
	if (ControlledCameraPawn)
	{
		ControlledCameraPawn->AddMovementInput(ForwardRot.RotateVector(FVector::ForwardVector), InputVector.X + InputVector.Y);

		// 添加右方向移动输入
		ControlledCameraPawn->AddMovementInput(RightRot.RotateVector(FVector::RightVector), InputVector.X - InputVector.Y);
	}
}

void AStrategyPlayerController::ZoomCamera(const FInputActionValue& Value)
{
	DoCameraModifyZoomCommand(Value.Get<float>() * ZoomScaling);
}

void AStrategyPlayerController::ResetCamera(const FInputActionValue& Value)
{
	DoCameraResetZoomCommand();
}

void AStrategyPlayerController::SelectHoldStarted(const FInputActionValue& Value)
{
	// 保存框选起始位置
	StartingBoxSelectionPosition = GetMouseLocationForPlayer();

}

void AStrategyPlayerController::SelectHoldTriggered(const FInputActionValue& Value)
{
	// 获取当前鼠标位置
	FVector2D SelectionPosition = GetMouseLocationForPlayer();

	// 计算选择框的大小
	FVector2D SelectionSize = SelectionPosition - StartingBoxSelectionPosition;

	// 更新 HUD 上的选择框显示
	if (StrategyHUD)
	{
		StrategyHUD->DragSelectUpdate(StartingBoxSelectionPosition, SelectionSize, SelectionPosition, true);
	}
}

void AStrategyPlayerController::SelectHoldCompleted(const FInputActionValue& Value)
{
	// 重置 HUD 上的选择框
	if (StrategyHUD)
	{
		StrategyHUD->DragSelectUpdate(FVector2D::ZeroVector, FVector2D::ZeroVector, FVector2D::ZeroVector, false);
	}
}

void AStrategyPlayerController::SelectClick(const FInputActionValue& Value)
{
	// 获取光标位置
	FVector CursorLocation;

	if (GetLocationUnderCursor(CursorLocation))
	{
		// 在光标处执行选择
		DoSelectCommand(CursorLocation, false);
	}
}

void AStrategyPlayerController::SelectClickAdditive(const FInputActionValue& Value)
{
	// 获取光标位置
	FVector CursorLocation;

	if (GetLocationUnderCursor(CursorLocation))
	{
		// 在光标处执行附加选择
		DoSelectCommand(CursorLocation, true);
	}
}

void AStrategyPlayerController::SelectAllDoubleClick(const FInputActionValue& Value)
{
	DoSelectAllUnitsOnScreenCommand();
}

void AStrategyPlayerController::InteractHoldStarted(const FInputActionValue& Value)
{

	// 保存交互起始位置
	StartingDragScrollPosition = GetMouseLocationForPlayer();
}

void AStrategyPlayerController::InteractHoldTriggered(const FInputActionValue& Value)
{
	// 执行拖拽滚动
	DoCameraDragScrollCommand(GetMouseLocationForPlayer());
}

void AStrategyPlayerController::InteractClick(const FInputActionValue& Value)
{
	// 获取光标位置
	FVector CursorLocation;

	// 光标下方是否有有效的交互位置？
	if (GetLocationUnderCursor(CursorLocation))
	{
		// 将选中的单位移动到目标位置
		DoMoveUnitsCommand(CursorLocation);
	}
}

void AStrategyPlayerController::TouchPrimaryHoldStarted(const FInputActionValue& Value)
{
	// 保存拖拽滚动的屏幕坐标
	StartingDragScrollPosition = Value.Get<FVector2D>();


}

void AStrategyPlayerController::TouchPrimaryHoldTriggered(const FInputActionInstance& Instance)
{
	FVector2D InputVector = Instance.GetValue().Get<FVector2D>();

	// 更新框选起始位置
	StartingBoxSelectionPosition = InputVector;

	if (Instance.GetElapsedTime() > TouchDragScrollHoldTime)
	{
		DoCameraDragScrollCommand(InputVector);

		// 记录本次游戏时间
		LastTouchDragScrollTime = GetWorld()->GetTimeSeconds();
	}
}

void AStrategyPlayerController::TouchPrimaryHoldCompleted(const FInputActionValue& Value)
{
	// 确保拖拽滚动结束后不会立即触发点击输入
	if (GetWorld()->GetTimeSeconds() - LastTouchDragScrollTime > 0.1f)
	{
		// 获取触控位置的世界坐标
		FVector TouchLocation = ProjectTouchPointToWorldSpace();

		// 尝试执行选择命令
		if (!DoSelectCommand(TouchLocation, true))
		{
			// 如果没有选中任何单位，则执行移动单位命令
			DoMoveUnitsCommand(TouchLocation);
		}
	}
}

void AStrategyPlayerController::TouchSecondaryTriggered(const FInputActionValue& Value)
{
	// 获取第二根手指的屏幕坐标
	FVector2D SelectionPosition = Value.Get<FVector2D>();

	// 计算选择框的大小
	FVector2D SelectionSize = SelectionPosition - StartingBoxSelectionPosition;

	// 更新 HUD 上的选择框
	if (StrategyHUD)
	{
		StrategyHUD->DragSelectUpdate(StartingBoxSelectionPosition, SelectionSize, SelectionPosition, true);
	}

}

void AStrategyPlayerController::TouchSecondaryCompleted(const FInputActionValue& Value)
{
	if (StrategyHUD)
	{
		// 隐藏选择框
		StrategyHUD->DragSelectUpdate(FVector2D::ZeroVector, FVector2D::ZeroVector, FVector2D::ZeroVector, false);
	}
}

bool AStrategyPlayerController::DoSelectCommand(const FVector& SelectLocation, bool bAdditiveSelection)
{
	// 如果不是附加选择模式，先取消所有选择
	if (!bAdditiveSelection)
	{
		DoDeselectAllUnitsCommand();
	}

	// 在光标位置执行重叠检测
	TArray<FOverlapResult> OutOverlaps;

	FCollisionShape CollisionSphere;
	CollisionSphere.SetSphere(SelectionRadius);

	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);

	FCollisionQueryParams QueryParams;

	if (GetWorld()->OverlapMultiByObjectType(OutOverlaps, SelectLocation, FQuat::Identity, ObjectParams, CollisionSphere, QueryParams))
	{
		// 查找重叠到的第一个单位
		for (const FOverlapResult& CurrentOverlap : OutOverlaps)
		{
			if (AStrategyUnit* CurrentUnit = Cast<AStrategyUnit>(CurrentOverlap.GetActor()))
			{
				// 该单位是否已被选中？
				if (ControlledUnits.Contains(CurrentUnit))
				{
					// 取消选中该单位
					ControlledUnits.Remove(CurrentUnit);

					CurrentUnit->UnitDeselected();
				}
				else
				{
					// 选中该单位
					ControlledUnits.Add(CurrentUnit);

					CurrentUnit->UnitSelected();
				}

				// 找到单位，返回成功
				return true;
			}
		}
	}

	// 未找到单位，返回失败
	return false;
}

void AStrategyPlayerController::DoSelectAllUnitsOnScreenCommand()
{
	// 获取关卡中的所有单位
	TArray<AActor*> Units;

	UGameplayStatics::GetAllActorsOfClass(this, AStrategyUnit::StaticClass(), Units);

	// 逐个处理单位
	for (AActor* CurrentActor : Units)
	{
		if (AStrategyUnit* CurrentUnit = Cast<AStrategyUnit>(CurrentActor))
		{
			// 如果单位尚未被选中且在屏幕上可见
			if (!ControlledUnits.Contains(CurrentUnit) && CurrentUnit->WasRecentlyRendered(0.2f))
			{
				// 选中该单位
				ControlledUnits.Add(CurrentUnit);

				CurrentUnit->UnitSelected();
			}
		}

	}
}

void AStrategyPlayerController::DoDeselectAllUnitsCommand()
{
	// 取消每个单位的选中状态
	for (AStrategyUnit* CurrentUnit : ControlledUnits)
	{
		if (IsValid(CurrentUnit))
		{
			CurrentUnit->UnitDeselected();
		}
	}

	// 清空选择列表
	ControlledUnits.Empty();
}

void AStrategyPlayerController::DoToggleSelectAllUnitsCommand()
{
	// 当前是否有已选单位？
	if (ControlledUnits.Num() > 0)
	{
		// 取消全选
		DoDeselectAllUnitsCommand();
	}
	else
	{
		// 选中屏幕上所有单位
		DoSelectAllUnitsOnScreenCommand();
	}
}

void AStrategyPlayerController::DoCameraDragScrollCommand(const FVector2D& CurrentCursorPosition)
{
	// 计算起始位置到当前位置的屏幕移动偏移量
	FVector2D MoveDelta = StartingDragScrollPosition - CurrentCursorPosition;

	// 旋转移动偏移量以适应等距视角
	const FRotator IsoRotation(0.0f, -45.0f, 0.0f);

	FVector RotatedDelta = IsoRotation.RotateVector(FVector(MoveDelta.X, MoveDelta.Y, 0.0f));

	// 应用拖拽倍率
	RotatedDelta *= DragMultiplier;

	// 将偏移应用到摄像机 Pawn
	if (ControlledCameraPawn)
	{
		ControlledCameraPawn->AddActorWorldOffset(RotatedDelta);
	}
}

void AStrategyPlayerController::DoMoveUnitsCommand(const FVector& GoalLocation)
{
	if (ControlledUnits.Num() > 0)
	{
		// 找到距离目标位置最近的已选单位
		AStrategyUnit* ClosestUnit = GetClosestSelectedUnitToLocation(GoalLocation);

		// 通知每个单位移动到目标位置
		for (AStrategyUnit* CurrentUnit : ControlledUnits)
		{
			if (IsValid(CurrentUnit))
			{
				CurrentUnit->MoveToLocation(GoalLocation, CurrentUnit == ClosestUnit, ControlledUnits);
			}
		}

		// 显示正向光标反馈
		BP_CursorFeedback(GoalLocation, true);

	}
	else
	{
		// 没有选中单位，显示负向光标反馈
		BP_CursorFeedback(GoalLocation, false);
	}
}

void AStrategyPlayerController::DoCameraModifyZoomCommand(float ZoomDelta)
{
	// 累加缩放增量
	CameraZoom += ZoomDelta;

	// 限制在最小和最大缩放之间
	CameraZoom = FMath::Clamp(CameraZoom, MinZoomLevel, MaxZoomLevel);

	// 将缩放值应用到摄像机 Pawn
	if (ControlledCameraPawn)
	{
		ControlledCameraPawn->SetZoomModifier(CameraZoom);
	}
}

void AStrategyPlayerController::DoCameraResetZoomCommand()
{
	// 重置为默认缩放
	CameraZoom = DefaultZoom;

	// 将缩放值应用到摄像机 Pawn
	if (ControlledCameraPawn)
	{
		ControlledCameraPawn->SetZoomModifier(CameraZoom);
	}
}

void AStrategyPlayerController::DoCameraSetZoomPercentageCommand(float Percentage)
{
	// 在最小和最大缩放之间插值
	CameraZoom = FMath::Lerp(MinZoomLevel, MaxZoomLevel, FMath::Clamp(Percentage, 0.0f, 1.0f));

	// 将缩放值应用到摄像机 Pawn
	if (ControlledCameraPawn)
	{
		ControlledCameraPawn->SetZoomModifier(CameraZoom);
	}
}

AStrategyUnit* AStrategyPlayerController::GetClosestSelectedUnitToLocation(FVector TargetLocation)
{
	// 最近单位及其距离
	AStrategyUnit* OutUnit = nullptr;
	float Closest = 0.0f;

	// 遍历已选单位列表
	for (AStrategyUnit* CurrentUnit : ControlledUnits)
	{
		if (IsValid(CurrentUnit))
		{
			// 是否已选中了一个单位？
			if (OutUnit != nullptr)
			{
				// 计算到目标位置的平方距离
				float Dist = FVector::DistSquared2D(TargetLocation, CurrentUnit->GetActorLocation());

				// 这个单位更近吗？
				if (Dist < Closest)
				{
					// 更新最近单位和距离
					OutUnit = CurrentUnit;
					Closest = Dist;
				}

			}
			else
			{

				// 尚未选中单位，使用当前单位
				OutUnit = CurrentUnit;

				// 初始化最近距离
				Closest = FVector::DistSquared2D(TargetLocation, CurrentUnit->GetActorLocation());
			}
		}

	}

	// 返回找到的最近单位
	return OutUnit;
}

FVector2D AStrategyPlayerController::GetMouseLocationForPlayer()
{
	// 尝试从该玩家控制器获取鼠标位置
	float MouseX, MouseY;

	if (GetMousePosition(MouseX, MouseY))
	{
		return FVector2D(MouseX, MouseY);
	}

	// 返回无效向量
	return FVector2D::ZeroVector;
}

bool AStrategyPlayerController::GetLocationUnderCursor(FVector& Location)
{
	// 在光标位置沿可见性通道进行射线检测
	FHitResult OutHit;

	GetHitResultUnderCursorByChannel(SelectionTraceChannel, false, OutHit);

	// 如果有阻挡命中，返回命中位置
	if (OutHit.bBlockingHit)
	{
		Location = OutHit.Location;
		return true;
	}

	return OutHit.bBlockingHit;
}

bool AStrategyPlayerController::GetLocationUnderFinger(FVector& Location)
{
	// 在 Touch 1 位置沿可见性通道进行射线检测
	FHitResult OutHit;

	GetHitResultUnderFingerByChannel(ETouchIndex::Touch1, SelectionTraceChannel, false, OutHit);

	// 如果有阻挡命中，返回命中位置
	if (OutHit.bBlockingHit)
	{
		Location = OutHit.Location;
		return true;
	}

	return OutHit.bBlockingHit;
}

FVector AStrategyPlayerController::ProjectTouchPointToWorldSpace()
{
	// 获取第一根手指的触控坐标
	float TouchX, TouchY = 0.0f;
	bool bPressed = false;

	GetInputTouchState(ETouchIndex::Touch1, TouchX, TouchY, bPressed);

	FVector WorldLocation = FVector::ZeroVector;
	FVector WorldDirection = FVector::ZeroVector;

	// 将屏幕坐标反投影到世界空间
	if (DeprojectScreenPositionToWorld(TouchX, TouchY, WorldLocation, WorldDirection))
	{
		// 沿摄像机方向执行射线检测
		FHitResult OutHit;

		GetWorld()->LineTraceSingleByChannel(OutHit, WorldLocation, WorldLocation + WorldDirection * 10000.0f, ECC_Visibility);

		// 如果命中物体，返回碰撞点
		if (OutHit.bBlockingHit)
		{
			return OutHit.ImpactPoint;
		}


		// 与水平平面求交并返回交点
		const FPlane IntersectPlane(FVector::ZeroVector, FVector::UpVector);
		return FMath::LinePlaneIntersection(WorldLocation, WorldLocation + (WorldDirection * 100000.0f), IntersectPlane);
	}

	// 反投影失败，返回零向量
	return FVector::ZeroVector;
}
