// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/DataTable.h"
#include "BaseEnemy.h"
#include "WayFinderGameMode.generated.h"

UENUM()
enum class EWaveDifficulty : uint8
{
	WD_DifficultyEasy,
	WD_DifficultyMedium,
	WD_DifficultyHard,
	WD_Test,
	WD_DifficultyDefault

};

UENUM(BlueprintType)
enum class EWaveState : uint8
{
	WS_Pending UMETA(DisplayName = "Pending"),
	WS_Started UMETA(DisplayName = "Started"),
	WS_WaveOver UMETA(DisplayName = "WaveOver"),
	WS_InWaveSpawning UMETA(DisplayName = "InWaveSpawning"),
	WS_InWaveNotSpawning UMETA(DisplayName = "InWaveNotSpawning"),
	WS_BetweenWaves UMETA(DisplayName = "BetweenWaves"),
	WS_Boss UMETA(DisplayName = "Boss"),
	WS_Paused UMETA(DisplayName = "Paused"),
	WS_FinishingUp UMETA(DisplayName = "FinishingUp"),
	WS_Compelete UMETA(DisplayName = "Complete")

};

USTRUCT(BlueprintType)
struct FWaveStats : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float DTBotTimerTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float DTUntilNextWaveTimerTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DTUntilNextWaveTimeModifier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 DTNumBotsToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 DTWaveCountTarget;

};

UCLASS(minimalapi)
class AWayFinderGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AWayFinderGameMode();

	virtual void StartPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	//Used to get the current wave state to print to gui
	UFUNCTION(BlueprintCallable)
	FORCEINLINE EWaveState GetCurrentWaveState() const { return this->WaveState; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetCurrentBotsAlive() const { return this->NumBotsAlive; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetNumBotsToSpawn() const { return this->NumBotsToSpawn; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetWaveCountCurrent() const { return this->WaveCountCurrent; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetWaveCountTarget() const { return this->WaveCountTarget; }
	

protected:

	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void SpawnNewBot();

	void UpdateBotSpawner();

	void CheckWaveState();

	void CheckBossState();

	//Starts bot spawning 
	void StartWave();

	//Stops bot spawning
	void EndWave();

	//Timer for next wave 
	void PrepareForNextWave();

	bool CheckAnyPlayerAlive();

	bool CheckAnyEnemyAlive();

	bool CheckBossAlive();

	void FindBoss();

	void InitWaveDataTable();

	void LastWaveReached();

	void BeginFinalWave();

	void FinishUpGame();

	//True for yes false for no
	void GameOver(bool GameWon);


private:


	// Handle to manage timer between bot spawns
	FTimerHandle BotTimerHandle;

	// Time between each bot spawn 
	UPROPERTY(EditAnywhere, Category = "GameMode", meta = (AllowPrivateAccess = "true"))
	float BotTimerTime;

	//Handle to manage timer between waves
	FTimerHandle UntilNextWaveHandle;

	//Time between each wave
	UPROPERTY(EditAnywhere, Category = "GameMode", meta = (AllowPrivateAccess = "true"))
	float UntilNextWaveTimerTime;

	// Value to adjust the time between each wave as the waves progress
	UPROPERTY(EditAnywhere, Category = "GameMode", meta = (AllowPrivateAccess = "true"))
	float UntilNextWaveTimeModifier;

	//Num of bots to spawn into current wave 
	UPROPERTY(EditAnywhere, Category = "GameMode", meta = (AllowPrivateAccess = "true"))
	int32 NumBotsToSpawn;

	//Tracker of wave current bot count (this is per wave)
	UPROPERTY(VisibleAnywhere, Category = "GameMode")
	int32 NumBotsAlive;

	//Current Wave
	UPROPERTY(EditAnywhere, Category = "GameMode", meta = (AllowPrivateAccess = "true"))
	int32 WaveCountCurrent;

	//Num of waves gone through
	UPROPERTY(EditAnywhere, Category = "GameMode", meta = (AllowPrivateAccess = "true"))
	int32 WaveCountTarget;

	UPROPERTY(EditAnywhere, Category = "GameMode", meta = (AllowPrivateAccess = "true"))
	EWaveDifficulty WaveDifficultyLevel;

	UPROPERTY(VisibleAnywhere, Category = "Boss")
	ABaseEnemy* BossEnemy;

	UPROPERTY(VisibleAnywhere, Category = "GameMode")
	EWaveState WaveState;

	TSubclassOf<AWayFinderCharacter> PlayerClass;
	TSubclassOf<ABaseEnemy> EnemyClass;

	bool bIsGameWon;

	bool bIsBossBattle;

	//TArray<AActor*> PlayersInGame;
	//TArray<ABaseEnemy*> EnemiesInGame;


};



