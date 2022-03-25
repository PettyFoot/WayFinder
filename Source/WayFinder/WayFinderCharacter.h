// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once


#include "CoreMinimal.h"
#include "Containers/Array.h"
#include "Containers/Map.h"
#include "GameFramework/Character.h"
#include "Animation/AnimMontage.h"
#include "WayFinderCharacter.generated.h"

//weapon_socket_r -- main hand
//weapon_socket_l -- off hand

class ABaseMeleeWeapon;
class UWayFinderHealthComponent;
class UAnimMontage;
class ABaseEnemy;
class AItem;
class UInventorySystem;
class AWeapon;

//TODO
//Add enum to track character state
//Add abilities to use ult charge for
//Add footstep sounds to jump and land anims
//Add a reset of health after life lost

UENUM(BlueprintType)
enum class EPlayerState : uint8
{
	PS_Idle, 
	PS_Travel,
	PS_Combat,
	PS_Attacking,
	PS_Dead,
	PS_Buffed
};


UCLASS(config=Game)
class AWayFinderCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	AWayFinderCharacter();

	virtual void Tick(float DeltaTime) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE UWayFinderHealthComponent* GetPlayerHealthComponent() { return this->PlayerHealthComponent; }

	FORCEINLINE bool GetIsPlayerDead() const { return this->bIsPlayerDead; }

	FORCEINLINE ABaseMeleeWeapon* GetPlayerEquippedWeapon() const { return this->PlayerEquippedMeleeWeapon; }


public:

	//
	bool LooseLife(bool should_apply_multiplier);

	//Called from health component to start internal death stuff
	void PlayerHasDied();

	//This is called from equipped weapon to enable any buffs, debuffs, consumable effect, etc. adjustment to wepaon damage when it is attacking and overlapping
	//an a valid enemy target
	//--returns a float value adjustment for weapon damage
	float GetPlayerWeaponWeaponAdjustments();

	//Axis input to zoom (set to mouse scroll wheel)
	void ZoomEnabled(float value);

	void AdjustOverlappedItems(int32 amount_to_adjust);

	//Called when player presses interact input action
	void PressedInteract();

	void UseItem(AItem* item_to_use);


public:

	UFUNCTION(BlueprintCallable)
	void EnableWeaponCollision();

	UFUNCTION(BlueprintCallable)
	void DisableWeaponCollision();

	void DoDamage(ABaseEnemy* enemy_target = nullptr);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDoDamage(ABaseEnemy* enemy_target = nullptr);

	void PlayerTakeDamage(float dmg_amount);

	//Negative for reducing health
	void PlayerGainHealth(float health_amount);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerPlayerTakeDamage(float dmg_amount);

	UFUNCTION(BlueprintCallable)
	void ToggleInvuln(bool bShouldBInvuln) { this->bIsInvuln = bShouldBInvuln; }

	UFUNCTION(BlueprintCallable)
	void EquipItem(AItem* item_to_equip);

	void AttachItemToHand(AItem* item_to_attach);

	

	//UFUNCTION(BlueprintCallable)
	//void UseItem(AItem* item_to_use);



public:

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	//Check if player is invuln (nothing uses this currently)
	bool bIsInvuln;

protected:

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	virtual void BeginPlay() override;




protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	void ToggleFly();

	void MoveUp(float Value);

	void PressedAbilityOne();
	void PressedAbilityTwo();
	void PressedAbilityThree();

	//Will check if weapon ult charge is high enough to use ult
	UFUNCTION(BlueprintCallable)
	void ActivateUltimateAbility();

	void PickUpItem();

private:

	UPROPERTY(VisibleAnywhere, Category = "Character Stats", meta = (AllowPrivateAccess = "true"))
	bool bIsAbleToFly;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Character Stats", meta = (AllowPrivateAccess = "true"))
	int32 LivesMax;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Stats", meta = (AllowPrivateAccess = "true"));
	int32 LivesLeft;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	//Player's melee weapon, if exists

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Character Stats", meta = (AllowPrivateAccess = "true"))
	ABaseMeleeWeapon* PlayerEquippedMeleeWeapon;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Character Stats", meta = (AllowPrivateAccess = "true"))
	AItem* PlayerEquippedItem;

	// Melee Weapon class, can only be main hand 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AWeapon> MeleeWeaponClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Components", meta = (AllowPrivateAccess = "true"))
		UWayFinderHealthComponent* PlayerHealthComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats", meta = (AllowPrivateAccess = "true"))
		UAnimMontage* AbilityOneMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats", meta = (AllowPrivateAccess = "true"))
		UAnimMontage* DeathMontage;

	FTimerHandle DeathTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats", meta = (AllowPrivateAccess = "true"))
	float DeathTimerTime;

	// adjuster of rate of zoom in and out
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats", meta = (AllowPrivateAccess = "true"))
	float ZoomSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float MaxZoomOut;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float MaxZoomIn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player State", meta = (AllowPrivateAccess = "true"))
	EPlayerState WFPlayerState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player State", meta = (AllowPrivateAccess = "true"))
	bool bIsPlayerDead;

	//Timer handle used to get player out of combat state after certain time not attacking (enemy combat state should not run in parallel)
	FTimerHandle PlayerCombatStateTimer;

	//Time before leaving combat state after not engaging enemy
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player State", meta = (AllowPrivateAccess = "true"))
	float PlayerCombatStateTimerTime;

	//Inventory
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Inventory", meta = (AllowPrivateAccess = "true"))
	UInventorySystem* Inventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Inventory", meta = (AllowPrivateAccess = "true"))
	bool bShouldTraceForItems;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Inventory", meta = (AllowPrivateAccess = "true"))
	int32 NumItemsOverlapping;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Inventory", meta = (AllowPrivateAccess = "true"))
	AItem* TraceHitItem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Inventory", meta = (AllowPrivateAccess = "true"))
	AItem* TraceHitLastFrame;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Inventory", meta = (AllowPrivateAccess = "true"))
	uint8 InventoryCapacity;

	
	

private:

	//Line trace for items if overlapping actors
	void TraceForItems();

	bool TraceUnderCrosshairs(FHitResult& out_hit_result, FVector& out_hit_location);

	void SpawnDefaultMeleeWeapon();

	UFUNCTION(BlueprintCallable)
	void Die();

	

	/* Ability bar private function calls */

	// Activate use of ability one by character 
	UFUNCTION(BlueprintCallable)
	void ActivateAbilityOne();
	void ActivateAbilityTwo();
	void ActivateAbilityThree();

	void LeaveCombatState();

	void BeginDie();


};

