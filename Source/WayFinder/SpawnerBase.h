// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnerBase.generated.h"

class ABaseEnemy;
class USphereComponent;
class USoundCue;
class UParticleSystem;

UCLASS()
class WAYFINDER_API ASpawnerBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnerBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	//Used to call from gamemode to reduce spawned thing count
	void ReduceSpawnCount(int32 spawn_reduction_amount);

	//Callback function for spawner start collision sphere overlap delegate
	UFUNCTION()
	void OnStartSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//Called when the time is right to spawn something
	UFUNCTION(BlueprintCallable)
	void Spawn();


private:

	//Current number of things spawned
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner Stats", meta = (AllowPrivateAccess = "true"))
	int32 CurrentSpawnCount;
	
	//Number of things to spawn, once activated
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner Stats", meta = (AllowPrivateAccess = "true"))
	int32 SpawnCountTarget;

	//Collision sphere to start the spawning of enemies
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner Stats", meta = (AllowPrivateAccess = "true"))
	USphereComponent* SpawnerStartCollisionSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner Stats", meta = (AllowPrivateAccess = "true"))
	int32 SpawnerLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner Stats", meta = (AllowPrivateAccess = "true"))
	int32 NumPlayersOverlapping;

	FTimerHandle SpawnTimerHandle;

	//Time between spawns
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner Stats", meta = (AllowPrivateAccess = "true"))
	float SpawnerTimerTime;

	//Min range for enemy to randomly be spawned from (from actor location)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner Stats", meta = (AllowPrivateAccess = "true"))
		float MinSpawnRange;

	//Max range for enemy to randomly be spawned from (from actor location)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner Stats", meta = (AllowPrivateAccess = "true"))
		float MaxSpawnRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner FX", meta = (AllowPrivateAccess = "true"))
	USoundCue* SpawnSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner FX", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* SpawnParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner FX", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ABaseEnemy> EnemySpawnType;

	float LastSpawnX;

	float LastSpawnY;

	

private:

	void TrySpawn();

	void AdjustSpawnLocation(float &adjust_value_x, float &adjust_value_y);

};
