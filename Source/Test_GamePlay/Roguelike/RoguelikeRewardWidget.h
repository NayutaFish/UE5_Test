// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Skill/SkillTypes.h"
#include "RoguelikeRewardWidget.generated.h"

class ARoguelikeRewardManager;

/** Base class for WBP_RoguelikeReward. Bind each button to SelectOption. */
UCLASS(Abstract, Blueprintable)
class TEST_GAMEPLAY_API URoguelikeRewardWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Reward")
	void SetupRewardOptions(ARoguelikeRewardManager* InRewardManager, const TArray<FRoguelikeRewardOption>& InOptions);

	UFUNCTION(BlueprintCallable, Category = "Reward")
	void SelectOption(int32 OptionIndex);

	UFUNCTION(BlueprintImplementableEvent, Category = "Reward")
	void OnRewardOptionsSet(const TArray<FRoguelikeRewardOption>& InOptions);

	UPROPERTY(BlueprintReadOnly, Category = "Reward")
	TObjectPtr<ARoguelikeRewardManager> RewardManager;

	UPROPERTY(BlueprintReadOnly, Category = "Reward")
	TArray<FRoguelikeRewardOption> RewardOptions;
};
