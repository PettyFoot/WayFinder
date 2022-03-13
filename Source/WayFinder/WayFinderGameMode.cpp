// Copyright Epic Games, Inc. All Rights Reserved.

#include "WayFinderGameMode.h"
#include "BaseEnemy.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "WayFinderCharacter.h"
#include "WayFinderHealthComponent.h"

//TODO make a data table to control all the different statistics of the wave mehcanics

AWayFinderGameMode::AWayFinderGameMode() :
	WaveDifficultyLevel(EWaveDifficulty::WD_DifficultyEasy),
	WaveCountCurrent(0),
	NumBotsAlive(0),
	WaveState(EWaveState::WS_Pending),
	bIsGameWon(false),
	bIsBossBattle(false)
	//bShouldSetAutoKillTimer(true),
	//TestEnemyKillTimer(10.f)
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.f;
}

void AWayFinderGameMode::StartPlay()
{
	Super::StartPlay();
	
	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsOfClass(this, ABaseEnemy::StaticClass(), Enemies);
	for (AActor* enemy : Enemies)
	{
		UE_LOG(LogTemp, Error, TEXT("Enemy found is: %s"), *enemy->GetName());
		ABaseEnemy* enemy_ai = Cast<ABaseEnemy>(enemy);
		if (enemy_ai)
		{
			UE_LOG(LogTemp, Error, TEXT("Boss Set to: %s"), *enemy_ai->GetName());
			this->FindBoss(enemy_ai);
		}
	}

}

void AWayFinderGameMode::Tick(float DeltaSeconds)
{

	Super::Tick(DeltaSeconds);

	switch (this->WaveState)
	{
	case EWaveState::WS_Pending:

		UE_LOG(LogTemp, Error, TEXT("NO BOSS"));

		break;
	case EWaveState::WS_Started:

		if (this->BossEnemy->GetIsEventTriggered())
		{
			this->StartWave();
		}

		break;
	case EWaveState::WS_InWaveSpawning:

		//this->UpdateBotSpawner(); 
		UE_LOG(LogTemp, Error, TEXT("IN WAVE SPAWNING"));

		break;
	case EWaveState::WS_InWaveNotSpawning:

		this->CheckWaveState();

		break;
	case EWaveState::WS_WaveOver:

		this->PrepareForNextWave();

		break;
	case EWaveState::WS_BetweenWaves:

		UE_LOG(LogTemp, Error, TEXT("BETWEEN WAVES"));

		break;
	case EWaveState::WS_Boss:

		this->LastWaveReached();
		this->CheckBossState();

		break;
	case EWaveState::WS_FinishingUp:

		this->FinishUpGame();

		break;
	case EWaveState::WS_Compelete:

		UE_LOG(LogTemp, Error, TEXT("GAME OVER :)"));

		break;
	default:
		break;
	}

}


void AWayFinderGameMode::UpdateBotSpawner()
{
	//If spawning enemies is enabled
	if (this->WaveState == EWaveState::WS_InWaveSpawning)
	{
		//If bot needs to be spawned spawn and update count
		if (this->NumBotsAlive < this->NumBotsToSpawn)
		{
			UE_LOG(LogTemp, Warning, TEXT("Spawned Bot"));
			this->SpawnNewBot(); //Called in blueprint does a query for areas to spawn bot 
			this->NumBotsAlive++;
		}

		if (this->NumBotsAlive >= this->NumBotsToSpawn)
		{
			this->WaveState = EWaveState::WS_InWaveNotSpawning; //turn off spawning if num bots to spawn for current wave is reached 
			UE_LOG(LogTemp, Warning, TEXT("Stopped Spawning Bots"));
		}
	}	
	//UE_LOG(LogTemp, Warning, TEXT("Tried to updatewave, but not current spawning bots  "));
}

void AWayFinderGameMode::CheckWaveState()
{

	

	if (!this->CheckAnyEnemyAlive())
	{
		this->WaveState = EWaveState::WS_WaveOver;
		
	}

	if (!this->CheckAnyPlayerAlive())
	{
		this->WaveState = EWaveState::WS_FinishingUp;
	}
	
}

void AWayFinderGameMode::CheckBossState()
{
	//To condense functions, this is only called when the boss mode is enabled 
	if (this->WaveState == EWaveState::WS_Boss && !this->CheckBossAlive())
	{
		this->bIsGameWon = true;
		this->WaveState = EWaveState::WS_FinishingUp;
	}

	if (!this->CheckAnyPlayerAlive())
	{
		this->WaveState = EWaveState::WS_FinishingUp;

	}
}

//Starts bot spawning 
void AWayFinderGameMode::StartWave()
{
	UE_LOG(LogTemp, Warning, TEXT("Called StartWave"));
	this->WaveCountCurrent++;
	this->NumBotsToSpawn = this->NumBotsToSpawn * 1.5;
	GetWorldTimerManager().SetTimer(this->BotTimerHandle, this, &AWayFinderGameMode::UpdateBotSpawner, this->BotTimerTime, true, this->BotTimerTime);
	this->WaveState = EWaveState::WS_InWaveSpawning;
}

//Stops bot spawning
void AWayFinderGameMode::EndWave() 
{
	UE_LOG(LogTemp, Warning, TEXT("Called EdnWave"));
	this->WaveState = EWaveState::WS_WaveOver;
	this->PrepareForNextWave();
	GetWorldTimerManager().ClearTimer(this->BotTimerHandle);
}

//Timer for next wave 
void AWayFinderGameMode::PrepareForNextWave() 
{
	UE_LOG(LogTemp, Warning, TEXT("Called PrepareForNextWave"));
	if (this->WaveCountCurrent == this->WaveCountTarget)
	{
		//All Waves have been cleared, enable boss
		this->WaveState = EWaveState::WS_Boss;
		return;
	}
	
	this->NumBotsAlive = 0;
	GetWorldTimerManager().SetTimer(this->UntilNextWaveHandle, this, &AWayFinderGameMode::StartWave, this->UntilNextWaveTimerTime, false);
	this->WaveState = EWaveState::WS_BetweenWaves;
}

bool AWayFinderGameMode::CheckAnyPlayerAlive()
{
	TArray<AActor*> PlayersInGame;
	UGameplayStatics::GetAllActorsOfClass(this, AWayFinderCharacter::StaticClass(), PlayersInGame);
	for (AActor* player: PlayersInGame )
	{
		AWayFinderCharacter* player_char = Cast<AWayFinderCharacter>(player);
		if (player_char)
		{
			if (player_char->GetPlayerHealthComponent()->GetCurrentHealth() > 0)
			{
				//Player found with health return true
				return true;
			}
		}
	}
	//No player was found or with health, return false
	return false;
}

bool AWayFinderGameMode::CheckAnyEnemyAlive()
{

	//Loop through all actors of type ABaseenemy
	TArray<AActor*> EnemiesInGame;
	UGameplayStatics::GetAllActorsOfClass(this, ABaseEnemy::StaticClass(), EnemiesInGame);
	for (AActor* enemy : EnemiesInGame)
	{
		ABaseEnemy* enemy_ai = Cast<ABaseEnemy>(enemy);
		if (enemy_ai && enemy_ai != this->BossEnemy)
		{
			if (enemy_ai->GetEnemyHealthComponent()->GetCurrentHealth() > 0)
			{
				return true;
			}
			
		}
	}

	return false;
}

bool AWayFinderGameMode::CheckBossAlive()
{
	if (this->BossEnemy)
	{
		return true;
	}
	return false;
}

void AWayFinderGameMode::FindBoss(ABaseEnemy* boss)
{
	if (!boss) { return; }
	//This is the boss
	this->BossEnemy = boss;
	this->BossEnemy->ToggleEnemyInvulnverability(true); //Make boss invulnerable
	//Run boss behavior tree TODO
	//Init wave data from wavestats data table
	this->InitWaveDataTable();
	this->WaveState = EWaveState::WS_Started;
}

void AWayFinderGameMode::LastWaveReached()
{
	//Temp boss timer

	if (this->bIsBossBattle) { return; }
	GetWorldTimerManager().SetTimer(this->UntilNextWaveHandle, this, &AWayFinderGameMode::BeginFinalWave, this->UntilNextWaveTimerTime * 2.f, false);
	this->bIsBossBattle = true;
	UE_LOG(LogTemp, Warning, TEXT("About to enter boss phase..."));
	
}

void AWayFinderGameMode::BeginFinalWave()
{
	UE_LOG(LogTemp, Warning, TEXT("You have reached the final wave!"));
	this->BossEnemy->ToggleEnemyInvulnverability(false);
	this->BossEnemy->SetActorScale3D(FVector(3.f, 3.f, 3.f));
	this->BossEnemy->EnableBossParticles();
	
}

void AWayFinderGameMode::FinishUpGame()
{
	if (this->bIsGameWon)
	{
		UE_LOG(LogTemp, Error, TEXT("!!!!!!!!!!!!!!!!!!YOU WON!!!!!!!!!!!"));
		this->GameOver(this->bIsGameWon);
	}
	else
	{
		//Call the loose things
		UE_LOG(LogTemp, Error, TEXT("!!!!!!!!!!!!!!!!!!YOU LOST!!!!!!!!!!!"));
		this->GameOver(this->bIsGameWon);
	}
}

void AWayFinderGameMode::GameOver(bool GameWon)
{
	UE_LOG(LogTemp, Warning, TEXT("+++++++++++++++GAME OVER++++++++++++++"));
	this->WaveState = EWaveState::WS_Compelete;
}

void AWayFinderGameMode::InitWaveDataTable()
{
	//Path to WaveStats data table 
	FString wave_stats_dt_path(TEXT("DataTable'/Game/Game/DataTables/DT_WaveStatsDataTable.DT_WaveStatsDataTable'"));

	UDataTable* dt_obj = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *wave_stats_dt_path));
	if (dt_obj)
	{
		FWaveStats* rarity_row = nullptr;

		switch (this->WaveDifficultyLevel)
		{
		case EWaveDifficulty::WD_DifficultyEasy:
			rarity_row = dt_obj->FindRow<FWaveStats>(FName("Easy"), TEXT(""));
			break;
		case EWaveDifficulty::WD_DifficultyMedium:
			rarity_row = dt_obj->FindRow<FWaveStats>(FName("Medium"), TEXT(""));
			break;
		case EWaveDifficulty::WD_DifficultyHard:
			rarity_row = dt_obj->FindRow<FWaveStats>(FName("Hard"), TEXT(""));
			break;
		case EWaveDifficulty::WD_Test:
			rarity_row = dt_obj->FindRow<FWaveStats>(FName("Test"), TEXT(""));
			break;
		default:
			break;
		}

		if (rarity_row)
		{
			this->WaveCountTarget = rarity_row->DTWaveCountTarget;
			this->BotTimerTime = rarity_row->DTBotTimerTime;
			this->NumBotsToSpawn = rarity_row->DTNumBotsToSpawn;
			this->UntilNextWaveTimerTime = rarity_row->DTUntilNextWaveTimerTime;
			this->UntilNextWaveTimeModifier = rarity_row->DTUntilNextWaveTimeModifier;
		}

	}

}