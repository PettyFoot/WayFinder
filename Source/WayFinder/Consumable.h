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

	//If this consumable is EOT(Effect over time)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable Properties", meta = (AllowPrivateAccess = "true"))
	bool bIsEOT;

	//Timer handle to manage the effect over time 
	FTimerHandle EffectTimerHandle;

	//total effect time of consumable
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable Properties", meta = (AllowPrivateAccess = "true"))
	float EffectTimerTotalTime;

	//Number of ticks to apply to target
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable Properties", meta = (AllowPrivateAccess = "true"))
	int32 NumOfTimesToTick;

	//Time between tick effects
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable Properties", meta = (AllowPrivateAccess = "true"))
	float EffectTimerTickTime;

	//Timer handle for consumables overall duration
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable Properties", meta = (AllowPrivateAccess = "true"))
	FTimerHandle ConsumableEffectDurationTimerHande;

	//Consumable's overall effect time (This can be different from effect over time)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable Properties", meta = (AllowPrivateAccess = "true"))
	float ConsumableEffectDuration;

	//Effect of consumable
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable Properties", meta = (AllowPrivateAccess = "true"))
	EConsumableEffectType ConsumableEffectType;

	//OVerride from parent class : AItem
	virtual void UseItem(class AWayFinderCharacter* player) override;

	//Triggers the effects of the consumable on the target
	//virtual void OnApplyEffect(AActor* entity_to_apply_effect) PURE_VIRTUAL(AConsumable, );

	virtual void BeginPlay() override;
	
};
