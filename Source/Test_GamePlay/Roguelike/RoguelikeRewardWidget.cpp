// Copyright Epic Games, Inc. All Rights Reserved.

#include "Roguelike/RoguelikeRewardWidget.h"

#include "Roguelike/RoguelikeRewardManager.h"

void URoguelikeRewardWidget::SetupRewardOptions(ARoguelikeRewardManager* InRewardManager, const TArray<FRoguelikeRewardOption>& InOptions)
{
	RewardManager = InRewardManager;
	RewardOptions = InOptions;
	OnRewardOptionsSet(RewardOptions);
}

void URoguelikeRewardWidget::SelectOption(int32 OptionIndex)
{
	if (RewardManager)
	{
		RewardManager->SelectReward(OptionIndex);
	}
}
