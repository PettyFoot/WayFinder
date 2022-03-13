// Fill out your copyright notice in the Description page of Project Settings.


#include "WayFinderHealthComponent.h"
#include "BaseEnemy.h"

// Sets default values for this component's properties
UWayFinderHealthComponent::UWayFinderHealthComponent():
	MaxHealth(100.f),
	CurrentHealth(100.f)
{
	
	PrimaryComponentTick.bCanEverTick = false;

}


// Called when the game starts
void UWayFinderHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	this->OwningActor = GetOwner();
	if (this->OwningActor)
	{
		//UE_LOG(LogTemp, Warning, TEXT("owner of health component found initializing on takeanydamage"));
		
	}

	this->CurrentHealth = this->MaxHealth;
	
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

