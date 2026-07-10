// Copyright Epic Games, Inc. All Rights Reserved.

#include "Roguelike/RoguelikeRewardManager.h"

#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "HikariPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Roguelike/RoguelikeRewardWidget.h"
#include "Skill/HikariSkillComponent.h"

DEFINE_LOG_CATEGORY(LogRoguelike);

ARoguelikeRewardManager::ARoguelikeRewardManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ARoguelikeRewardManager::BeginPlay()
{
	Super::BeginPlay();
	ResolvePlayer();
}

void ARoguelikeRewardManager::ShowRewardAfterRoomClear()
{
	if (ActiveRewardWidget || !ResolvePlayer())
	{
		return;
	}

	CurrentRewardOptions = GenerateRewardOptions();
	if (CurrentRewardOptions.IsEmpty())
	{
		UE_LOG(LogRoguelike, Warning, TEXT("Room clear produced no legal rewards."));
		return;
	}

	if (!RewardWidgetClass)
	{
		UE_LOG(LogRoguelike, Error, TEXT("RewardWidgetClass is not configured. Create WBP_RoguelikeReward from RoguelikeRewardWidget and assign it."));
		return;
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (!PlayerController)
	{
		return;
	}

	ActiveRewardWidget = CreateWidget<URoguelikeRewardWidget>(PlayerController, RewardWidgetClass);
	if (!ActiveRewardWidget)
	{
		return;
	}

	ActiveRewardWidget->SetupRewardOptions(this, CurrentRewardOptions);
	ActiveRewardWidget->AddToViewport();
	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(ActiveRewardWidget->TakeWidget());
	PlayerController->SetInputMode(InputMode);
	PlayerController->SetShowMouseCursor(true);
	UE_LOG(LogRoguelike, Log, TEXT("Room clear reward shown with %d option(s)."), CurrentRewardOptions.Num());
}

TArray<FRoguelikeRewardOption> ARoguelikeRewardManager::GenerateRewardOptions()
{
	TArray<FRoguelikeRewardOption> Candidates;
	if (!ResolvePlayer())
	{
		return Candidates;
	}

	if (!CachedSkillComponent->HasSkill(EPlayerSkillID::CircularSlash) && CachedSkillComponent->HasEmptySkillSlot())
	{
		Candidates.Add(MakeOption(ERoguelikeRewardType::GainSkill, EPlayerSkillID::CircularSlash, ESkillUpgradeType::None,
			FText::FromString(TEXT("Gain Circular Slash")), FText::FromString(TEXT("Release a damaging circle around the player."))));
	}
	if (CachedSkillComponent->CanApplyUpgrade(EPlayerSkillID::TripleProjectile, ESkillUpgradeType::Mechanic))
	{
		Candidates.Add(MakeOption(ERoguelikeRewardType::UpgradeSkill, EPlayerSkillID::TripleProjectile, ESkillUpgradeType::Mechanic,
			FText::FromString(TEXT("Projectile Barrage")), FText::FromString(TEXT("Triple Projectile fires 2 additional projectiles."))));
	}
	if (CachedSkillComponent->CanApplyUpgrade(EPlayerSkillID::TripleProjectile, ESkillUpgradeType::Cooldown))
	{
		Candidates.Add(MakeOption(ERoguelikeRewardType::UpgradeSkill, EPlayerSkillID::TripleProjectile, ESkillUpgradeType::Cooldown,
			FText::FromString(TEXT("Quick Reload")), FText::FromString(TEXT("Triple Projectile cooldown is reduced by 0.5 seconds."))));
	}
	if (CachedSkillComponent->CanApplyUpgrade(EPlayerSkillID::CircularSlash, ESkillUpgradeType::Mechanic))
	{
		Candidates.Add(MakeOption(ERoguelikeRewardType::UpgradeSkill, EPlayerSkillID::CircularSlash, ESkillUpgradeType::Mechanic,
			FText::FromString(TEXT("Expanded Slash")), FText::FromString(TEXT("Circular Slash radius is increased by 60."))));
	}
	if (CachedSkillComponent->CanApplyUpgrade(EPlayerSkillID::CircularSlash, ESkillUpgradeType::Cooldown))
	{
		Candidates.Add(MakeOption(ERoguelikeRewardType::UpgradeSkill, EPlayerSkillID::CircularSlash, ESkillUpgradeType::Cooldown,
			FText::FromString(TEXT("Swift Recovery")), FText::FromString(TEXT("Circular Slash cooldown is reduced by 0.4 seconds."))));
	}

	TArray<FRoguelikeRewardOption> Options;
	while (!Candidates.IsEmpty() && Options.Num() < 2)
	{
		const int32 CandidateIndex = FMath::RandRange(0, Candidates.Num() - 1);
		Options.Add(Candidates[CandidateIndex]);
		Candidates.RemoveAtSwap(CandidateIndex);
	}
	return Options;
}

void ARoguelikeRewardManager::SelectReward(int32 OptionIndex)
{
	if (!CurrentRewardOptions.IsValidIndex(OptionIndex) || !ResolvePlayer())
	{
		return;
	}

	const FRoguelikeRewardOption Reward = CurrentRewardOptions[OptionIndex];
	ApplyReward(Reward);
	UE_LOG(LogRoguelike, Log, TEXT("Player selected reward: %s."), *Reward.Title.ToString());
	CloseRewardUI();
}

bool ARoguelikeRewardManager::ResolvePlayer()
{
	if (IsValid(CachedPlayer) && IsValid(CachedSkillComponent))
	{
		return true;
	}

	CachedPlayer = Cast<AHikariPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	CachedSkillComponent = CachedPlayer ? CachedPlayer->SkillComponent : nullptr;
	if (!CachedSkillComponent)
	{
		UE_LOG(LogRoguelike, Error, TEXT("Could not find a Hikari player with HikariSkillComponent."));
		return false;
	}
	return true;
}

void ARoguelikeRewardManager::ApplyReward(const FRoguelikeRewardOption& Reward)
{
	if (Reward.RewardType == ERoguelikeRewardType::GainSkill)
	{
		CachedSkillComponent->AddSkillToFirstEmptySlot(Reward.SkillID);
	}
	else
	{
		CachedSkillComponent->ApplySkillUpgrade(Reward.SkillID, Reward.UpgradeType);
	}
}

void ARoguelikeRewardManager::CloseRewardUI()
{
	if (ActiveRewardWidget)
	{
		ActiveRewardWidget->RemoveFromParent();
		ActiveRewardWidget = nullptr;
	}
	CurrentRewardOptions.Empty();

	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0))
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
		PlayerController->SetShowMouseCursor(false);
	}
}

FRoguelikeRewardOption ARoguelikeRewardManager::MakeOption(ERoguelikeRewardType RewardType, EPlayerSkillID SkillID, ESkillUpgradeType UpgradeType, const FText& Title, const FText& Description) const
{
	FRoguelikeRewardOption Option;
	Option.RewardType = RewardType;
	Option.SkillID = SkillID;
	Option.UpgradeType = UpgradeType;
	Option.Title = Title;
	Option.Description = Description;
	return Option;
}
