// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "Weapon.h"
#include "BaseMeleeWeapon.generated.h"

//TODO
//Add hit sound for when sword hits flesh (maybe physical materials)
//Add abilities for weapons to use
//add sounds for having full ult charge

class UStaticMeshComponent;
class USkeletalMeshComponent;
class UBoxComponent;
class USphereComponent;
class UParticleSystem;
class USoundCue;


USTRUCT(BlueprintType)
struct FWeaponStats : public FTableRowBase
{
	GENERATED_BODY()

public:


	//Base weapon damage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1", ClampMax = "2.5"))
		float DTBaseWeaponDamageMultiplier;

	//Adjustment to damage if critical strike is landed 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "3"))
		float DTCriticalDamageMultiplier;

	//smaller value makes weapon deteriorate slower
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = -0.0008f, ClampMax = 0.0008f))
		float DTWeaponDurabilityLossRate;

	//smaller value is less ult charge gain
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "-0.5", ClampMax = "1.5"))
	float DTUltimateChargeGainRate;

	//smaller value is less damage on ult (use 5 for mega impact? will have to play test this value)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.5", ClampMax = "5"))
	float DTUltChargeDamageMultiplier;


	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//	UStaticMeshComponent* DTStaticWeaponMeshComponent;

};




UCLASS()
class WAYFINDER_API ABaseMeleeWeapon : public AWeapon
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseMeleeWeapon();

	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void InitWaveDataTable();

	void UpdateWaveTableDate();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Called from loot table to populate item with item info from enemies loot table
	//Sets item based on item info uobject (used for loot table item spawns)
	virtual void InitWithItemInfo(FItemInfoStruct iteminfo) override;

	//FORCEINLINE UStaticMeshComponent* GetStaticWeaponMeshComponent() const { return this->StaticWeaponMeshComponent; }
	//FORCEINLINE USkeletalMeshComponent* GetSkeletalWeaponMeshComponent() const { return this->SkeletalWeaponMeshComponent; }
	FORCEINLINE float GetBaseWeaponDamage() const { return this->BaseWeaponDamage; }
	FORCEINLINE AActor* GetActorOverlappedOnUse() { return this->ActorOverlappedOnUse; }

	//Ult charge public getters
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetUltCurrentCharge() const { return this->UltimateChargeCurrent; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetUltChargeMax() const { return this->UltimateChargeMax; }

	FORCEINLINE void SetWeaponBaseDamage(float amount_to_adjust) { this->BaseWeaponDamage += amount_to_adjust; }
	FORCEINLINE void SetUltChargeRate(float amount_to_adjust) { this->UltChargeRate += amount_to_adjust; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE UParticleSystem* GetUltParticles() const { return this->UltAbilityParticles; }

	//Ult charge public setters, will only set ult charge as high as max. (Use negative numbers to reduce ult charge too)
	UFUNCTION(BlueprintCallable)
	void SetUltimateCharge(float adj_amount);

	UFUNCTION(BlueprintCallable)
	bool CanUseUlt();

	UFUNCTION(BlueprintCallable)
	void UseUlt();

	UFUNCTION(BlueprintCallable)
	void EndUlt();

	void ToggleWeaponCollision(bool bShouldWeaponCollide);

	UFUNCTION()
	void OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	UFUNCTION()
	void OnUltimateOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//Used to give character using weapon the overlapped actor
	void ToggleWeaponWaitingToApplyDamage(bool bIsWeaponWaitingToApplyDamage);

	virtual void UseItem(class AWayFinderCharacter* player) override;


private:


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* WeaponCollisionBox;

	// Only set if weapon has collided with enemy
	UPROPERTY(VisibleAnywhere)
	AActor* ActorOverlappedOnUse;

	//Used for setting actor hit for player to apply damage (will be reset by player)
	UPROPERTY(VisibleAnywhere, Category = "Weapon State")
	bool bIsWaitingToApplyDamage;

	//HitResult to store hit result if target is enemy, could be used for player too
	//FHitResult OverlapHitReturn;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Stats")
	bool bShouldCollide;

	//Bone name of weapon socket to spawn impact particles at, from successful hits
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon FX", meta = (AllowPrivateAccess = "true"))
		FName BoneNameWeaponImpact;

	//Max ultimate charge
	UPROPERTY(VisibleAnywhere, Category = "Weapon Stats")
	float UltimateChargeMax;

	//Ultimate charge
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats", meta = (AllowPrivateAccess = "true"))
	float UltimateChargeCurrent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon FX", meta = (AllowPrivateAccess = "true"))
	USoundCue* UltimateChargeMaxSoundCue;

	//Radius to apply ult effect within
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon FX", meta = (AllowPrivateAccess = "true"))
	float UltChargeRadius;

	//Dmaage of ult
	UPROPERTY(VisibleAnywhere, Category = "Weapon Stats")
	float UltChargeDamage;


	//Area of effect to apply damage to enemies that are within bounds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats", meta = (AllowPrivateAccess = "true"))
	USphereComponent* UltAbilityAOE;

	//Initialized on spawn from FWeaponInfoStruct (Member Inside FItemInfoStruct)
	//Weapon level (Comes from loot table RNG) Adjusts various stats and aspects of weapon interaction (See DT above)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Stats", meta = (AllowPrivateAccess = "true"))
	EWeaponLevel WeaponLevel;

	//Base weapon damage 
	//__effected by WeaponLevel
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Stats", meta = (AllowPrivateAccess = "true"))
	float BaseWeaponDamage;

	//Adjustment to damage if critical strike is landed 
	//__effected by WeaponLevel
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Stats", meta = (AllowPrivateAccess = "true"))
	float CriticalDamageAdjustment;

	//Starting durability of weapon
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Stats", meta = (AllowPrivateAccess = "true"))
	float StartingWeaponDurability;

	//Rate of weapon durability loss per use
	//__effected by WeaponLevel
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Stats", meta = (AllowPrivateAccess = "true"))
	float DurabilityLossRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Stats", meta = (AllowPrivateAccess = "true"))
		float DurabilityLossPerUse;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Stats", meta=(AllowPrivateAccess = "true"))
	float UltChargeRate;


	//Particles to spawn on impact, should probably have different system to spawn particles based on impact surface type 
	//TODO implement surface types and particle specificity to dif surface types
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon FX", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticles;

	//Ult ability particles
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon FX", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* UltAbilityParticles;


	/*
	//Weapon STATIC mesh component
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats", meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* StaticWeaponMeshComponent;

	//skeletal weapon mesh (ensure this mesh has proper sockets needed by the rest of logic to spawn FX etc)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats", meta = (AllowPrivateAccess = "true"))
		USkeletalMesh* SkeletalWeaponMesh;*/
	//Add montages of different attack combos and set from FItemInfoStruct upon spawn
	//Once weapon is picked up player combo montages should be initialized from weapon's montage sections.
	//This allows different weapon types to have different animations (cool!:))

	/* TODO Add weapon type enum to allow the change of montage sections within the way finder character to enable multiple ability montages per weapon type */



private:

	//Called from within overlap event to do fx (particles, sounds, w.e)
	void PlayWeaponHitFX();
};
