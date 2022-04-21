// Fill out your copyright notice in the Description page of Project Settings.


#include "Consumable.h"
#include "BaseMeleeWeapon.h"
#include "WayFinderCharacter.h"
#include "WayFinderHealthComponent.h"
#include "InventorySystem.h"


AConsumable::AConsumable():
	EffectTimerTickTime(1),
	bIsEOT(false),
	TotalEffectDone(0.f),
	DestroyOnComplete(false),
	Started(false),
	NumberOfUsesTotal(1),
	NumberOfUseLeft(1),
	EffectDuration(1.f)
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

	this->ConsumableEffectType = iteminfo.ConsumableInfoStruct.ConsumableEffectType; //This may not work and need to be moved to AItem parent call (InitWithItemInfo())
	bIsEOT = iteminfo.ConsumableInfoStruct.bIsEOT;
	NumberOfUsesTotal = iteminfo.ConsumableInfoStruct.NumberOfUseLeft;
	NumberOfUseLeft = iteminfo.ConsumableInfoStruct.NumberOfUseLeft; //Add randomness to this
	EffectDuration = iteminfo.ConsumableInfoStruct.EffectDuration; //Add randomness to this

	this->ConsumableType = iteminfo.ConsumableInfoStruct.ConsumableType; //Set consumable Tpye (enum describing the type of consume this is (potion, fooddrink, CC, damage, buff)
	////This may not work and need to be moved to AItem parent call (InitWithItemInfo())


	switch (this->ConsumableEffectType)
	{
	case EConsumableEffectType::CET_Potion:
		this->EffectAmount = iteminfo.ConsumableInfoStruct.BaseEffectAmount * ItemLevel;
		break;
	case EConsumableEffectType::CET_FoodDrink:
		break;
	case EConsumableEffectType::CET_Buff:
		this->SetEffectAmountBuffs(iteminfo);
		break;
	case EConsumableEffectType::CET_CrowdControl:
		break;
	default:
		break;
	}
	/*	ConsumableBuffType
		EffectAmount*/
}

void AConsumable::UseItem(AWayFinderCharacter* player)
{
	this->TotalEffectDone = 0;
	this->TempPlayerEffecting = player;

	switch (ConsumableEffectType)
	{
	case EConsumableEffectType::CET_Potion:
		this->UsePotion();
		break;
	case EConsumableEffectType::CET_FoodDrink:
		break;
	case EConsumableEffectType::CET_Buff:
		this->Buff();
		break;
	case EConsumableEffectType::CET_CrowdControl:
		break;
	
	default:
		break;
	}

}

void AConsumable::BeginPlay()
{
	Super::BeginPlay();

	this->TotalEffectDone = 0.f;
	this->Started = false;
}

//TODO overloaded Consumable heal that allows for it to be a projectile and effect an area with consumable effect upon impact and explosion
//Pass in AActor and effect that Actor with effects of consumable
void AConsumable::Heal()
{
	if (this->bIsEOT) //if effect over time
	{
		if (this->TempPlayerEffecting) //If player to effect is found
		{
			this->TempPlayerEffecting->PlayerGainHealth(this->EffectAmount / this->EffectDuration);
			this->TotalEffectDone += this->EffectAmount / this->EffectDuration;
		}
		else {/*AOE Splash ? */ }

		if (!this->Started) //if effect hasn't started seek to remove item from inventory (used)
		{
			this->Started = true; //Consumable effect started
			this->DestroyOnComplete = this->IsConsumableMultiUse(); //Uses item and decrements use if multi use or returns true if item is last item in stack, false otherwise
		}
		else
		{
			if (this->TotalEffectDone < this->EffectAmount) //will stop calling after total effect is completed
			{
				GetWorldTimerManager().SetTimer(this->EffectTimerHandle, this, &AConsumable::Heal, this->EffectTimerTickTime, false);
				return;
			}
			else
			{ 
				this->Started = false; //reset effect for next use
			}
		}
	}
	else
	{
		if (this->TempPlayerEffecting) //If player to effect is found
		{
			this->DestroyOnComplete = this->IsConsumableMultiUse();
			this->TempPlayerEffecting->PlayerGainHealth(this->EffectAmount);
		}
	}

	this->EndConsumableUse(this->DestroyOnComplete);

}

void AConsumable::Buff()
{
	if (!this->TempPlayerEffecting) { return; }
	if (!this->Started)
	{
		this->Started = true;
		//this->TempPlayerEffecting->GetPlayerHealthComponent()->SetCurrentHealth(this->EffectAmount);
		this->DestroyOnComplete = this->IsConsumableMultiUse();

		switch (this->ConsumableType.ConsumableBuffType)
		{
		case EBuffType::BUFF_Fortify:
			this->TempPlayerEffecting->GetPlayerHealthComponent()->SetMaxHealth(this->EffectAmount);
			break;
		case EBuffType::BUFF_Enrage:
			this->TempPlayerEffecting->GetPlayerEquippedWeapon()->SetWeaponBaseDamage(this->EffectAmount);
			break;
		case EBuffType::BUFF_Invigorate:
			this->TempPlayerEffecting->GetPlayerEquippedWeapon()->SetUltChargeRate(this->EffectAmount);
			break;
		case EBuffType::BUFF_Shield:
			//TODO need to implement player shield first (not the kind u hold
			break;
		case EBuffType::BUFF_Swift:
			//TODO
			break;
		case EBuffType::BUFF_Default:
			UE_LOG(LogTemp, Warning, TEXT("Buff Type: Default"));
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("No Buff Type"));
			break;
		}

		//TODO fix this, this will not work with multiple effects as it will overide the reset timer of a current effect if another is used before this timer is done, may need 4 different handles
		//Check if one is running and start another if it is to avoid the reset and keep multiple effects of consumables
		GetWorldTimerManager().SetTimer(this->ConsumableEffectDurationTimerHande, this, &AConsumable::Buff, this->EffectDuration, false);
		return;
	}

	if (this->Started)
	{
		this->Started = false;
		switch (this->ConsumableType.ConsumableBuffType)
		{
		case EBuffType::BUFF_Fortify:
			this->TempPlayerEffecting->GetPlayerHealthComponent()->SetMaxHealth(-this->EffectAmount);
			break;
		case EBuffType::BUFF_Enrage:
			this->TempPlayerEffecting->GetPlayerEquippedWeapon()->SetWeaponBaseDamage(-this->EffectAmount);
			break;
		case EBuffType::BUFF_Invigorate:
			this->TempPlayerEffecting->GetPlayerEquippedWeapon()->SetUltChargeRate(-this->EffectAmount);
			break;
		case EBuffType::BUFF_Shield:
			//TODO need to implement player shield first (not the kind u hold
			break;
		case EBuffType::BUFF_Swift:
			//TODO
			break;
		case EBuffType::BUFF_Default:
			UE_LOG(LogTemp, Warning, TEXT("Buff Type: Default"));
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("No Buff Type"));
			break;
		}
		this->EndConsumableUse(this->DestroyOnComplete);
	}
}

bool AConsumable::IsConsumableMultiUse()
{
	//Remove item set for destruction or reduce item stack by 1
	if (this->OwningInventory->RemoveItem(this->InventorySlotIndex) == -1)
	{
		return true; //This is last consumable of type in stack, mark item destruction
	}

	if (this->NumberOfUsesTotal > 1) //if more than 1 use left
	{
		this->NumberOfUseLeft--; //Use charge of consumable 
		if (this->NumberOfUseLeft <= 0) //check if consumable has all its uses used (remove from inventory if so
		{
			return true; //This is last consumable of type in stack, mark item destruction
		}
		else {}
	}
	else {}

	return false;
}

void AConsumable::EndConsumableUse(bool should_destoy)
{

	this->TempPlayerEffecting = nullptr; //Reset player target to null

	if (should_destoy)
	{
		this->Destroy();
	}
	else
	{
		this->TotalEffectDone = 0.f; //reset for next use
	}
}

void AConsumable::SetEffectAmountBuffs(FItemInfoStruct iteminfo)
{
	switch (this->ConsumableType.ConsumableBuffType)
	{
	case EBuffType::BUFF_Fortify:
		this->EffectAmount = iteminfo.ConsumableInfoStruct.FortifyEffectAmount * ItemLevel;
		break;
	case EBuffType::BUFF_Enrage:
		this->EffectAmount = iteminfo.ConsumableInfoStruct.EnrageEffectAmount * ItemLevel;
		break;
	case EBuffType::BUFF_Invigorate:
		this->EffectAmount = iteminfo.ConsumableInfoStruct.InvigoreEffectAmount;
		break;
	case EBuffType::BUFF_Shield:
		//TODO need to implement player shield first (not the kind u hold
		break;
	case EBuffType::BUFF_Swift:
		//TODO
		break;
	case EBuffType::BUFF_Default:
		UE_LOG(LogTemp, Warning, TEXT("Buff Type: Default"));
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("No Buff Type"));
		break;
	}
}

void AConsumable::UsePotion()
{
	switch (this->ConsumableType.PotionType)
	{
	case EPotionType::PT_Heal:
		this->Heal();
		break;
	case EPotionType::PT_Ult:
		break;
	case EPotionType::PT_Stamina:
		break;
	case EPotionType::PT_Magica:
		break;
	case EPotionType::PT_Poison:
		break;
	case EPotionType::PT_Fire:
		break;
	case EPotionType::PT_Bleed:
		break;
	case EPotionType::PT_Default:
		break;
	default:
		break;
	}
}
