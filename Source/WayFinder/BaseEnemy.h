// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WayFinderCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BaseEnemy.generated.h"



/**
 * 
 */


class UWayFinderHealthComponent;
class UBehaviorTree;
class ABaseEnemyControllerAI;
class UAnimMontage;
class UBoxComponent;
class UParticleSystem;
class UParticleSystemComponent;

UCLASS()
class WAYFINDER_API ABaseEnemy : public ACharacter
{
	GENERATED_BODY()
	

public:

	ABaseEnemy();

	virtual void Tick(float DeltaTime) override;

public:

	FORCEINLINE UBehaviorTree* GetBehaviorTree() const { return this->EnemyBehaviorTree; }
	FORCEINLINE bool GetEnemyInvulnerability() const { return this->bIsInvulvernable; }
	FORCEINLINE bool GetIsEnemyDead() const { return this->bIsEnemyDead; }
	FORCEINLINE bool GetIsEventTriggered() const { return this->bEventTriggered; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE UWayFinderHealthComponent* GetEnemyHealthComponent() { return this->EnemyHealthComponent; }

	//Damage enemy by amount, death is handled through the gamemode for game mode logic and for eaasy replication (DONT CHANGE THIS METHOD UNTIL FOUND A BETTER METHOD)
	void EnemyTakeDamage(float DamageAmount, AActor* damage_causer);

	//Initializes enemy death
	void EnemyHasDied();

	//This should be moved to boss sub class******
	void ToggleEnemyInvulnverability(bool bShouldEnemyBeInvulnerable);

	UFUNCTION(BlueprintCallable)
	void EnemyAttack();

	UFUNCTION(BlueprintCallable)
		void EnableWeaponCollision();

	UFUNCTION(BlueprintCallable)
		void DisableWeaponCollision();

	void EnableBossParticles();

	UFUNCTION(BlueprintNativeEvent)
	void ShowHealthBar();
	void ShowHealthBar_Implementation();

	UFUNCTION(BlueprintImplementableEvent)
	void HideHealthBar();
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OverlappedEventCollisionBox(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void EndOverlapEventCollisionsBox(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:

	//Enemy behavior tree 
	UPROPERTY(EditAnywhere, Category = "Enemy AI", meta = (AllowPrivateAccess = "true"))
	UBehaviorTree* EnemyBehaviorTree;

	//AI controller 
	UPROPERTY(EditAnywhere, Category = "Enemy AI", meta = (AllowPrivateAccess = "true"))
	ABaseEnemyControllerAI* EnemyController;

	//Crude Patrol path
	//Patrol point one
	UPROPERTY(EditAnywhere, Category = "Enemy AI", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	FVector PatrolPointOne;

	//Patrol point one
	UPROPERTY(EditAnywhere, Category = "Enemy AI", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
		FVector PatrolPointTwo;

	//Patrol point one
	UPROPERTY(EditAnywhere, Category = "Enemy AI", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
		FVector PatrolPointThree;

	//Patrol point one
	UPROPERTY(EditAnywhere, Category = "Enemy AI", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
		FVector PatrolPointFour;

	//Patrol point one
	UPROPERTY(EditAnywhere, Category = "Enemy AI", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
		FVector PatrolPointFive;

	//Patrol point one
	UPROPERTY(EditAnywhere, Category = "Enemy AI", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
		FVector PatrolPointSix;

	//Health component 
	UPROPERTY(EditAnywhere, Category = "Enemy AI", meta = (AllowPrivateAccess = "true"))
	UWayFinderHealthComponent* EnemyHealthComponent;

	//Set from game mode to enable a boss type enemy to exist until end of wave
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character Stats", meta = (AllowPrivateAccess = "true"))
	bool bIsInvulvernable;

	//Collision box to enable events (currently just waves, but maybe other events too)
	UPROPERTY(EditAnywhere, Category = "Enemy AI", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* EventEnableCollisionBox;

	//true when eventcollisionbox is enabled -- used to enable wave logic once player enters collision box 
	bool bEventTriggered;

	//FX
	UPROPERTY(EditAnywhere, Category = "Enemy FX", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* EnemyAttackMontage;

	//Montage for hit react
	UPROPERTY(EditAnywhere, Category = "Enemy FX", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HitReactMontage;

	//Montage for death
	UPROPERTY(EditAnywhere, Category = "Enemy FX", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DeathMontage;

	FTimerHandle DeathTimerHandle;

	UPROPERTY(EditAnywhere, Category = "Enemy AI", meta = (AllowPrivateAccess = "true"))
	float DeathTimerTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character Stats", meta = (AllowPrivateAccess = "true"))
		ABaseMeleeWeapon* EnemyEquippedMeleeWeapon;

	// Melee Weapon class, can only be main hand 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats", meta = (AllowPrivateAccess = "true"))
		TSubclassOf<ABaseMeleeWeapon> MeleeWeaponClass;

	bool bIsEnemyDead;

	//This should be moved to boss sub class*********
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BossParticleTwo;

	//This should be moved to boss sub class******
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BossParticleOne;

	//This should be moved to boss sub class******
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats", meta = (AllowPrivateAccess = "true"))
		UParticleSystemComponent* ParticlesOne;

	//This should be moved to boss sub class******
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats", meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* ParticlesTwo;

	//Player currently attacking
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character Stats", meta = (AllowPrivateAccess = "true"))
	AWayFinderCharacter* AttackingPlayer;

	//Attacker's relative position to enemy (0->90, 0->-270)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character Stats", meta = (AllowPrivateAccess = "true"))
	float AttackerPositionAround;

	//Health bar timer handle 
	FTimerHandle ShowHealthBarHandle;

	//Time before health bar is hidden 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats", meta = (AllowPrivateAccess = "true"))
	float ShowHealthBarTimerTime;


private:

	//Destroys pawn
	UFUNCTION(BlueprintCallable) //Keeping for now I may decide to run this based on animation notify
	void Die();

	//Played when enemy is hit 
	void PlayHitReact();


	void SpawnDefaultMeleeWeapon();



	
	

};
