// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Consumable.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EConsumableEffectType : uint8
{

	CET_Heal UMETA(DisplayName = "Heal"),
	CET_CrowdControl UMETA(DisplayName = "Crowd Control"),
	CET_Damage UMETA(DisplayName = "Damage"),
	CET_Buff UMETA(DisplayName = "Buff")
};


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

	//If this consumable is EOT(Effect over time)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable Properties", meta = (AllowPrivateAccess = "true"))
	bool bIsEOT;

	bool Started;
	bool Ended;

	//Timer handle to manage the effect over time 
	FTimerHandle EffectTimerHandle;

	//Consumable's overall effect time (This can be different from effect over time)
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable Properties", meta = (AllowPrivateAccess = "true"))
	//float ConsumableEffectDurationLongTerm;

	//total effect time of consumable
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Consumable Properties", meta = (AllowPrivateAccess = "true"))
	float EffectDuration;

	//Time between tick effects
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Consumable Properties", meta = (AllowPrivateAccess = "true"))
	float EffectTimerTickTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Consumable Properties", meta = (AllowPrivateAccess = "true"))
	float TotalEffectDone;

	bool DestroyOnComplete;

	//Number of ticks to apply to target
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable Properties", meta = (AllowPrivateAccess = "true"))
	int32 NumOfTimesToTick;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Consumable Properties", meta = (AllowPrivateAccess = "true"))
	int32 NumOfTimesTicked;

	//Timer handle for consumables overall duration
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable Properties", meta = (AllowPrivateAccess = "true"))
	FTimerHandle ConsumableEffectDurationTimerHande;

	//Amount to heal, damage, cc or buff the player
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Consumable Properties", meta = (AllowPrivateAccess = "true"))
	float EffectAmount;

	//Effect of consumable
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Consumable Properties", meta = (AllowPrivateAccess = "true"))
	EConsumableEffectType ConsumableEffectType;

	//OVerride from parent class : AItem
	virtual void UseItem(class AWayFinderCharacter* player) override;

	//Triggers the effects of the consumable on the target
	//virtual void OnApplyEffect(AActor* entity_to_apply_effect) PURE_VIRTUAL(AConsumable, );

	virtual void BeginPlay() override;

	

	void ConsumableHeal();

	void HealPlayer();


	class AWayFinderCharacter* TempPlayerEffecting;
	
};
