// Fill out your copyright notice in the Description page of Project Settings.


#include "Consumable.h"
#include "WayFinderCharacter.h"
#include "InventorySystem.h"


AConsumable::AConsumable():
	NumOfTimesToTick(1),
	NumOfTimesTicked(0),
	EffectTimerTickTime(1),
	bIsEOT(false),
	TotalEffectDone(0.f),
	DestroyOnComplete(false)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AConsumable::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AConsumable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AConsumable::InitWithItemInfo(FItemInfoStruct iteminfo)
{
	Super::InitWithItemInfo(iteminfo);
}

void AConsumable::UseItem(AWayFinderCharacter* player)
{
	this->TotalEffectDone = 0;

	switch (ConsumableEffectType)
	{
	case EConsumableEffectType::CET_Heal:
		this->TempPlayerEffecting = player;
		this->ConsumableHeal();
		break;
	case EConsumableEffectType::CET_CrowdControl:
		break;
	case EConsumableEffectType::CET_Damage:
		break;
	case EConsumableEffectType::CET_Buff:
		break;
	default:
		break;
	}

}

void AConsumable::BeginPlay()
{
	Super::BeginPlay();

	this->TotalEffectDone = 0.f;
}

void AConsumable::ConsumableHeal()
{
	
	if (this->bIsEOT)
	{
		if (!this->Started)
		{
			if (this->OwningInventory->RemoveItem(this->InventorySlotIndex) == -1)
			{
				this->DestroyOnComplete = true;
			}
			this->Started = true;
		}
		
		if (this->TempPlayerEffecting) //If player to effect is found
		{
			
			this->TempPlayerEffecting->PlayerGainHealth(this->EffectAmount / this->EffectDuration);
			this->TotalEffectDone += this->EffectAmount / this->EffectDuration;
		
		}

		if (this->TotalEffectDone < this->EffectAmount)
		{
			GetWorldTimerManager().SetTimer(this->EffectTimerHandle, this, &AConsumable::ConsumableHeal, this->EffectTimerTickTime, false);
			return;
		}

		this->Started = false;
	}
	else
	{
		if (this->TempPlayerEffecting) //If player to effect is found
		{
			if (this->OwningInventory->RemoveItem(this->InventorySlotIndex) == -1)
			{
				this->DestroyOnComplete = true;
			}
			this->TempPlayerEffecting->PlayerGainHealth(this->EffectAmount);
		}

		
	}
	//Heal player
	//this->OwningInventory->RemoveItem(this->InventorySlotIndex, 1);

	//Play destory effects here such as player glows for a bit or play sound to signify end of effect

	this->TempPlayerEffecting = nullptr;

	if (this->DestroyOnComplete)
	{
		this->Destroy();
	}

}

void AConsumable::HealPlayer()
{
}
