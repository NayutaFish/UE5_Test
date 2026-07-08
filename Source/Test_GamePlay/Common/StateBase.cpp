// Copyright Epic Games, Inc. All Rights Reserved.

#include "Common/StateBase.h"

void UStateBase::OnEnter_Implementation()
{
	// 进入状态时的逻辑，子类可重写
}

void UStateBase::Update_Implementation(float DeltaTime)
{
	// 每帧更新逻辑，子类可重写
}

void UStateBase::OnExit_Implementation()
{
	// 退出状态时的逻辑，子类可重写
}
