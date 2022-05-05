// Fill out your copyright notice in the Description page of Project Settings.


#include "PWorld.h"
#include "PTerrain.h"
#include "ChunkGenerator.h"
#include "Kismet/GameplayStatics.h"
#include "WayFinderCharacter.h"

// Sets default values
APWorld::APWorld()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GenerateDistance = 1;

}

// Called when the game starts or when spawned
void APWorld::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(this, CharacterClass, actors);
	for (auto& actor : actors)
	{
		AWayFinderCharacter* player_char = Cast<AWayFinderCharacter>(actor);
		if (player_char)
		{
			this->PlayersInGame.Add(player_char);
		}
	}

	if (PlayersInGame.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("APWorld::BeginPlay__ No players found, need player to generate world"));
	}

	
}

// Called every frame
void APWorld::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	this->CheckShouldGenerateTerrain();
	if (bShouldGenerateTerrain)
	{
		this->GenerateTerrain();
	}

}

void APWorld::CheckShouldGenerateTerrain()
{
	for (auto& player : PlayersInGame)
	{
		this->CheckPlayerLocation(player->GetActorLocation());
	}
}

void APWorld::CheckPlayerLocation(FVector players_location)
{
	

}

void APWorld::GenerateTerrain()
{
	
}

void APWorld::CleanUpTerrain()
{
}

