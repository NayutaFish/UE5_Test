// Copyright Epic Games, Inc. All Rights Reserved.

#include "Test_GamePlay.h"
#include "Modules/ModuleManager.h"

/* 项目的主模块入口——每个 UE 项目必须有一个这个宏
 * FDefaultGameModuleImpl：使用 UE 默认的模块实现方式
 * 类比 Unity：相当于 Assembly-CSharp 程序集 */
IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, Test_GamePlay, "Test_GamePlay" );

DEFINE_LOG_CATEGORY(LogTest_GamePlay)
