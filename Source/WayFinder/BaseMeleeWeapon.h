// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "Item.h"
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

UENUM(BlueprintType)
enum class EWeaponLevel : uint8
{
	WL_NoviceWeapon UMETA(DisplayName = "Novice"),
	WL_ApprenticeWeapon UMETA(DisplayName = "Apprentice"),
	WL_AdeptWeapon UMETA(DisplayName = "Adept"),
	WL_MasterWeapon UMETA(DisplayName = "Master"),
	WL_ExhaltedWeapon UMETA(DisplayName = "Exhalted"),
	WL_LegendaryWeapon UMETA(DisplayName = "Legendary"),
	WL_DefaultWeapon UMETA(DisplayName = "Default")
	
};

USTRUCT(BlueprintType)
struct FWeaponStats : public FTableRowBase
{
	GENERATED_BODY()

public:
	//Base weapon damage
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float DTBaseWeaponDamage;

	//Adjustment to damage if critical strike is landed 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1", ClampMax = "10"))
		float DTCriticalDamageAdjustment;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", ClampMax = "1"))
		float DTStartingWeaponDurability;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USkeletalMeshComponent* DTSkeletalWeaponMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DTUltimateChargeMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float DTUltChargeDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystem* DTImpactParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystem* DTUltAbilityParticles;





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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//FORCEINLINE UStaticMeshComponent* GetStaticWeaponMeshComponent() const { return this->StaticWeaponMeshComponent; }
	//FORCEINLINE USkeletalMeshComponent* GetSkeletalWeaponMeshComponent() const { return this->SkeletalWeaponMeshComponent; }
	FORCEINLINE float GetBaseWeaponDamage() const { return this->BaseWeaponDamage; }
	FORCEINLINE AActor* GetActorOverlappedOnUse() { return this->ActorOverlappedOnUse; }

	//Ult charge public getters
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetUltCurrentCharge() const { return this->UltimateChargeCurrent; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetUltChargeMax() const { return this->UltimateChargeMax; }

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
		UStaticMeshComponent* StaticWeaponMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats", meta = (AllowPrivateAccess = "true"))
	EWeaponLevel WeaponLevel;

	//Base weapon damage
	UPROPERTY(VisibleAnywhere, Category = "Weapon Stats")
		float BaseWeaponDamage;

	//Adjustment to damage if critical strike is landed 
	UPROPERTY(VisibleAnywhere, Category = "Weapon Stats")
		float CriticalDamageAdjustment;

	//Starting durability of weapon
	UPROPERTY(VisibleAnywhere, Category = "Weapon Stats")
		float StartingWeaponDurability;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* WeaponCollisionBox;

	// Only set if weapon has collided with enemy
	UPROPERTY(VisibleAnywhere)
	AActor* ActorOverlappedOnUse;

	//Used for setting actor hit for player to apply damage (will be reset by player)
	UPROPERTY(VisibleAnywhere, Category = "Weapon State")
	bool bIsWaitingToApplyDamage;

	//HitResult to store hit result if target is enemy, could be used for player too
	FHitResult OverlapHitReturn;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Stats")
	bool bShouldCollide;

	//Bone name of weapon socket to spawn impact particles at, from successful hits
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon FX", meta = (AllowPrivateAccess = "true"))
		FName BoneNameWeaponImpact;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon FX", meta = (AllowPrivateAccess = "true"))
		UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon FX", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* UltAbilityParticles;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Stats")
	float UltimateChargeMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats", meta = (AllowPrivateAccess = "true"))
	float UltimateChargeCurrent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon FX", meta = (AllowPrivateAccess = "true"))
	USoundCue* UltimateChargeMaxSoundCue;

	//Radius to apply ult effect within
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon FX", meta = (AllowPrivateAccess = "true"))
		float UltChargeRadius;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Stats")
	float UltChargeDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats", meta = (AllowPrivateAccess = "true"))
	USphereComponent* UltAbilityAOE;


	/* TODO Add weapon type enum to allow the change of montage sections within the way finder character to enable multiple ability montages per weapon type */



private:

	//Called from within overlap event to do fx (particles, sounds, w.e)
	void PlayWeaponHitFX();
};
