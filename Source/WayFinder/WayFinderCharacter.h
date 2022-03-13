// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Animation/AnimMontage.h"
#include "WayFinderCharacter.generated.h"

//weapon_socket_r -- main hand
//weapon_socket_l -- off hand

class ABaseMeleeWeapon;
class UWayFinderHealthComponent;
class UAnimMontage;
class ABaseEnemy;



UCLASS(config=Game)
class AWayFinderCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	AWayFinderCharacter();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE UWayFinderHealthComponent* GetPlayerHealthComponent() { return this->PlayerHealthComponent; }

	FORCEINLINE bool GetIsPlayerDead() const { return this->bIsPlayerDead; }

	//TODO
	bool LooseLife(bool should_apply_multiplier);

	void BeginDie();

public:

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UFUNCTION(BlueprintCallable)
	void EnableWeaponCollision();

	UFUNCTION(BlueprintCallable)
	void DisableWeaponCollision();

	void DoDamage(ABaseEnemy* enemy_target = nullptr);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDoDamage(ABaseEnemy* enemy_target = nullptr);

	void PlayerTakeDamage(float dmg_amount);

	void PlayerHasDied();

	void ZoomEnabled(float value);



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

private:

	UPROPERTY(VisibleAnywhere, Category = "Character Stats", meta = (AllowPrivateAccess = "true"))
	bool bIsAbleToFly;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Character Stats", meta = (AllowPrivateAccess = "true"))
	int32 LivesMax;

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

	// Melee Weapon class, can only be main hand 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ABaseMeleeWeapon> MeleeWeaponClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Components", meta = (AllowPrivateAccess = "true"))
		UWayFinderHealthComponent* PlayerHealthComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats", meta = (AllowPrivateAccess = "true"))
		UAnimMontage* AbilityOneMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats", meta = (AllowPrivateAccess = "true"))
		UAnimMontage* DeathMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player State", meta = (AllowPrivateAccess = "true"))
	bool bIsPlayerDead;

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
	

private:

	UFUNCTION(BlueprintCallable)
	void Die();

	void SpawnDefaultMeleeWeapon();

	/* Ability bar private function calls */

	// Activate use of ability one by character 
	UFUNCTION(BlueprintCallable)
	void ActivateAbilityOne();
	void ActivateAbilityTwo();
	void ActivateAbilityThree();


};

