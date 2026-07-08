// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Test_GamePlay : ModuleRules
{
	public Test_GamePlay(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"NavigationSystem",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"Niagara",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"Test_GamePlay",
			"Test_GamePlay/Common",
			"Test_GamePlay/Player",
			"Test_GamePlay/Variant_Strategy",
			"Test_GamePlay/Variant_Strategy/UI",
			"Test_GamePlay/Variant_TwinStick",
			"Test_GamePlay/Variant_TwinStick/AI",
			"Test_GamePlay/Variant_TwinStick/Gameplay",
			"Test_GamePlay/Variant_TwinStick/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
