// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "InventoryStructs.h"
#include "Consumable.generated.h"

/**
 * 
 */



UCLASS()
class WAYFINDER_API AConsumable : public AItem
{
	GENERATED_BODY()

public:

	AConsumable();

	virtual void OnConstruction(const FTransform& Transform) override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Called from loot table to populate item with item info from enemies loot table
	//Sets item based on item info uobject (used for loot table item spawns)
	virtual void InitWithItemInfo(FItemInfoStruct iteminfo) override;

	//OVerride from parent class : AItem
	virtual void UseItem(class AWayFinderCharacter* player) override;

	//Triggers the effects of the consumable on the target
	//virtual void OnApplyEffect(AActor* entity_to_apply_effect) PURE_VIRTUAL(AConsumable, );

	virtual void BeginPlay() override;

public:


	//If this consumable is EOT(Effect over time)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Consumable Properties", meta = (AllowPrivateAccess = "true"))
	bool bIsEOT;

	//Amount to heal, damage, cc or buff the player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Consumable Properties", meta = (AllowPrivateAccess = "true"))
	float EffectAmount;

	//Number of uses total
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Consumable Properties", meta = (AllowPrivateAccess = "true"))
	uint8 NumberOfUsesTotal;

	//Number of uses current/left
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Consumable Properties", meta = (AllowPrivateAccess = "true"))
	uint8 NumberOfUseLeft;

	//total effect time of consumable
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Consumable Properties", meta = (AllowPrivateAccess = "true"))
	float EffectDuration;

	//Time between tick effects 
	//__Set for 1 second interval so effect formula per tick (1 second) beceomes effectamount / effectduration = effectamount/second
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Consumable Properties", meta = (AllowPrivateAccess = "true"))
	float EffectTimerTickTime;

	//Tracker for effect over time to see if total effect of item is complete
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Consumable Properties", meta = (AllowPrivateAccess = "true"))
	float TotalEffectDone;

	

private:

	//Used to determine if consumable should be destroyed at end of use
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Consumable Properties", meta = (AllowPrivateAccess = "true"))
	bool DestroyOnComplete;

	//Used to start a consumable use
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Consumable Properties", meta = (AllowPrivateAccess = "true"))
	bool Started;

	//Timer handle to manage the effect over time 
	FTimerHandle EffectTimerHandle;

	//Timer handle for consumables overall duration
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable Properties", meta = (AllowPrivateAccess = "true"))
	FTimerHandle ConsumableEffectDurationTimerHande;


	class AWayFinderCharacter* TempPlayerEffecting;
	class ABaseEnemy* TempEnemyEffecting; //Could be used by enemies to buff themselves idk

	void Heal();

	void Buff();

	//Helper function to see if item has multiple uses and manages those uses on item use
	bool IsConsumableMultiUse();

	void EndConsumableUse(bool should_destoy);

	void SetEffectAmountBuffs(FItemInfoStruct iteminfo);

	void UsePotion();
	
};
