// Fill out your copyright notice in the Description page of Project Settings.


#include "WayFinderHealthComponent.h"
#include "WayFinderCharacter.h"
#include "BaseEnemy.h"

// Sets default values for this component's properties
UWayFinderHealthComponent::UWayFinderHealthComponent():
	MaxHealth(100.f),
	CurrentHealth(100.f),
	StartingHealth(14.f)
{
	
	PrimaryComponentTick.bCanEverTick = false;
	this->MaxHealth = this->StartingHealth;

}

// Called when the game starts
void UWayFinderHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	this->MaxHealth = this->StartingHealth;
	this->CurrentHealth = this->MaxHealth;
	
}



void UWayFinderHealthComponent::SetHealthComponentOwner(AWayFinderCharacter* owner)
{
	this->PlayerOwner = owner;
}

void UWayFinderHealthComponent::SetHealthComponentOwner(ABaseEnemy* owner)
{
	this->EnemyOwner = owner;
}

void UWayFinderHealthComponent::HealthTakeDamage(float Damage)
{
	//If damage will kill target set health to 0
	if (this->CurrentHealth - Damage <= 0)
	{
		this->CurrentHealth = 0.f;

		//check if owner of health component is base enemy and call basenemy public kill function: EnemyHasDied();
		ABaseEnemy* enemy_owner = GetOwner<ABaseEnemy>();
		if (enemy_owner)
		{
			if (enemy_owner->GetIsEnemyDead())
			{
				return;
			}
			enemy_owner->EnemyHasDied();
			return;
		}

		AWayFinderCharacter* player = GetOwner<AWayFinderCharacter>();
		if (player)
		{
			//If player is alreadyd dead return
			if (player->GetIsPlayerDead())
			{
				return;
			}
			player->PlayerHasDied();
			return;
		}
		UE_LOG(LogTemp, Warning, TEXT("Health is Zero: %f"), this->CurrentHealth);
		return;
	}

	//Apply damage if health will be above 0
	this->CurrentHealth -= Damage;
	UE_LOG(LogTemp, Warning, TEXT("Health is: %f"), this->CurrentHealth);
}


// Called every frame
void UWayFinderHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

