// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelSystem.h"
#include "BaseEnemy.h"
#include "WayFinderHealthComponent.h"
#include "WayFinderCharacter.h"


// Sets default values for this component's properties
ULevelSystem::ULevelSystem():
	PlayerOwner(nullptr),
	EnemyOwner(nullptr),
	StartingLevel(1),
	CurrentLevel(0),
	ExperienceCurrent(0),
	ExperienceToNextLevel(1)
{
	
}


void ULevelSystem::GainExperience(float experience_to_gain)
{
	if (experience_to_gain >= 0)
	{
		if ((this->ExperienceCurrent + experience_to_gain) > this->ExperienceToNextLevel)
		{
			float experience_left_over = (this->ExperienceCurrent += experience_to_gain) - this->ExperienceToNextLevel;
			this->ExperienceCurrent = (this->ExperienceCurrent + experience_to_gain) - experience_left_over; //For visual effects of experience bar
			this->SetCurrentLevel(this->CurrentLevel + 1);
			this->ExperienceCurrent = experience_left_over;
			
		}
		else
		{
			this->ExperienceCurrent += experience_to_gain;
		}

		if (this->ExperienceCurrent >= this->ExperienceToNextLevel)
		{
			this->SetCurrentLevel(this->CurrentLevel + 1);
		}
	}
}

void ULevelSystem::SetLevelSystemOwner(AWayFinderCharacter* player_owner)
{
}

void ULevelSystem::SetLevelSystemOwner(ABaseEnemy* player_owner)
{
}

void ULevelSystem::SetCurrentLevel(int32 level_to_set)
{
	this->CurrentLevel = level_to_set;

	const float max_health = this->PlayerOwner->GetPlayerHealthComponent()->GetStartingHealth() * this->CurrentLevel;
	this->PlayerOwner->GetPlayerHealthComponent()->SetMaxHealth(max_health);

	this->CalculateExperienceNextLevel();
}

// Called when the game starts
void ULevelSystem::BeginPlay()
{
	Super::BeginPlay();

	this->CurrentLevel = this->StartingLevel;
	this->ExperienceCurrent = this->ExperienceToNextLevel;
	this->CalculateExperienceNextLevel();
	
}


// Called every frame
void ULevelSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void ULevelSystem::CalculateExperienceNextLevel()
{
	this->ExperienceToNextLevel = (20) * FMath::Square(this->CurrentLevel);
}

void ULevelSystem::LevelUp()
{
}

