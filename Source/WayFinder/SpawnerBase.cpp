// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnerBase.h"
#include "BaseEnemy.h"
#include "Components/SphereComponent.h"
#include "Item.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
#include "WayFinderCharacter.h"


// Sets default values
ASpawnerBase::ASpawnerBase():
	CurrentSpawnCount(0),
	SpawnCountTarget(1),
	SpawnerTimerTime(5.f),
	NumPlayersOverlapping(0),
	MinSpawnRange(-1000.f),
	MaxSpawnRange(1000.f)
{

	this->SpawnerStartCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("StartCollisionSphere"));
	this->SpawnerStartCollisionSphere->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void ASpawnerBase::BeginPlay()
{
	Super::BeginPlay();
	
	this->SpawnerStartCollisionSphere->SetGenerateOverlapEvents(true);
	this->SpawnerStartCollisionSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	this->SpawnerStartCollisionSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	this->SpawnerStartCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ASpawnerBase::OnStartSphereOverlap);
	
}


void ASpawnerBase::ReduceSpawnCount(int32 spawn_reduction_amount)
{
}

void ASpawnerBase::OnStartSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AWayFinderCharacter* player = Cast<AWayFinderCharacter>(OtherActor);
	if (player)
	{
		if (this->CurrentSpawnCount < this->SpawnCountTarget)
		{
			
			GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ASpawnerBase::TrySpawn, this->SpawnerTimerTime);
			

		}
	}
	
}

void ASpawnerBase::Spawn()
{
	//Random spawn location
	//Need to work out how to calculate z position (for places like spawning on slopes high above spawner
	float x = FMath::FRandRange(this->MinSpawnRange, this->MaxSpawnRange);
	float y = FMath::FRandRange(this->MinSpawnRange, this->MaxSpawnRange);
	
	this->AdjustSpawnLocation(x, y);

	this->LastSpawnX = x;
	this->LastSpawnY = y;

	FVector spawn_loc(GetActorLocation() + FVector(x, y, 500.f));

	//Spawn Enemy
	ABaseEnemy* spawned_enemy = GetWorld()->SpawnActor<ABaseEnemy>(this->EnemySpawnType, spawn_loc, GetActorRotation());

	if (spawned_enemy)
	{
		int32 enemy_level =  FMath::RandRange(this->SpawnerLevel - 2, this->SpawnerLevel + 2);
		spawned_enemy->SetEnemyLevel(enemy_level);
		spawned_enemy->SetEnemySpawner(this);
		this->CurrentSpawnCount++;
	}
}

void ASpawnerBase::TrySpawn()
{
	if (this->CurrentSpawnCount < this->SpawnCountTarget)
	{

		GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ASpawnerBase::TrySpawn, this->SpawnerTimerTime);
		this->Spawn();

	}
}

void ASpawnerBase::AdjustSpawnLocation(float &adjust_value_x, float &adjust_value_y)
{
	//don't let number be too close to zero
	if (adjust_value_x < 150)
	{
		adjust_value_x = 150;

	}
	else if (adjust_value_x > -150)
	{
		adjust_value_x = -150;
	}

	//don't let number be too close to zero
	if (adjust_value_y < 150)
	{
		adjust_value_y = 150;

	}
	else if (adjust_value_y > -150)
	{
		adjust_value_y = -150;
	}


	if (adjust_value_x < 0)
	{
		if (LastSpawnX < 0 && LastSpawnX != 0)
		{
			adjust_value_x = -adjust_value_x;
		}
	}
	else if (adjust_value_x > 0)
	{
		if (LastSpawnX > 0 && LastSpawnX != 0)
		{
			adjust_value_x = -adjust_value_x;
		}
	}

	if (adjust_value_y < 0)
	{
		if (LastSpawnY < 0 && LastSpawnY != 0)
		{
			adjust_value_y = -adjust_value_y;
		}
	}
	else if (adjust_value_y > 0 )
	{
		if (LastSpawnY > 0 && LastSpawnY != 0)
		{
			adjust_value_y = -adjust_value_y;
		}
	}
}

