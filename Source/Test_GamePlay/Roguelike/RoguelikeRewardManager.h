// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Skill/SkillTypes.h"
#include "RoguelikeRewardManager.generated.h"

class AHikariPlayerCharacter;
class UHikariSkillComponent;
class URoguelikeRewardWidget;

DECLARE_LOG_CATEGORY_EXTERN(LogRoguelike, Log, All);

/** Owns room-clear reward generation, UI display, and reward application. */
UCLASS(Blueprintable)
class TEST_GAMEPLAY_API ARoguelikeRewardManager : public AActor
{
	GENERATED_BODY()

public:
	ARoguelikeRewardManager();

	UFUNCTION(BlueprintCallable, Category = "Reward")
	void ShowRewardAfterRoomClear();

	UFUNCTION(BlueprintCallable, Category = "Reward")
	TArray<FRoguelikeRewardOption> GenerateRewardOptions();

	UFUNCTION(BlueprintCallable, Category = "Reward")
	void SelectReward(int32 OptionIndex);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward|UI")
	TSubclassOf<URoguelikeRewardWidget> RewardWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reward|Runtime")
	TArray<FRoguelikeRewardOption> CurrentRewardOptions;

protected:
	virtual void BeginPlay() override;

private:
	bool ResolvePlayer();
	void ApplyReward(const FRoguelikeRewardOption& Reward);
	void CloseRewardUI();
	FRoguelikeRewardOption MakeOption(ERoguelikeRewardType RewardType, EPlayerSkillID SkillID, ESkillUpgradeType UpgradeType, const FText& Title, const FText& Description) const;

	UPROPERTY(Transient)
	TObjectPtr<URoguelikeRewardWidget> ActiveRewardWidget;

	UPROPERTY(Transient)
	TObjectPtr<AHikariPlayerCharacter> CachedPlayer;

	UPROPERTY(Transient)
	TObjectPtr<UHikariSkillComponent> CachedSkillComponent;
};
