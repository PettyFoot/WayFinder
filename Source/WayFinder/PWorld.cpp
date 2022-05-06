// Fill out your copyright notice in the Description page of Project Settings.


#include "PWorld.h"
#include "ChunkGenerator.h"
#include "PTerrain.h"
#include "Kismet/GameplayStatics.h"
#include "WayFinderCharacter.h"

// Sets default values
APWorld::APWorld()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GenerateDistance = 1;
	bShouldStartGenerationCheck = false;
	bIsGenerating = false;
	GenerationCurrent = 0;
	bShouldCleanUp = false;

}

// Called when the game starts or when spawned
void APWorld::BeginPlay()
{
	Super::BeginPlay();

	
	this->SetGenerationOrder();

	this->ChunkGenerator = GetWorld()->SpawnActor<AChunkGenerator>(AChunkGenerator::StaticClass());
	if (!ChunkGenerator)
	{
		UE_LOG(LogTemp, Warning, TEXT("No chunk generator"));
	}


	GetWorldTimerManager().SetTimer(this->GetPlayersLocationTimer, this, &APWorld::GetWorldPlayers, this->GetPlayersLocationTimerTime);

	if (HeightAdjustmentCurve)
	{
		this->ChunkGenerator->SetGeneratorParams(PlainSize, TerrainScale, Seed, Scale, Octaves, Persistence, Lacunarity, HeightMultiplier, HeightAdjustmentCurve);
		UE_LOG(LogTemp, Warning, TEXT("Nchunk_generator_plainsize: %d"), this->ChunkGenerator->PlainSize);
	}
	else
	{
		this->ChunkGenerator->SetGeneratorParams(PlainSize, TerrainScale, Seed, Scale, Octaves, Persistence, Lacunarity, HeightMultiplier);
		UE_LOG(LogTemp, Warning, TEXT("Nchunk_generator_plainsize: %d"), this->ChunkGenerator->PlainSize);
	}

	
	
	

	//GetWorldTimerManager().SetTimer(this->UpdatePlayersLocationTimer, this, &APWorld::CheckShouldGenerateTerrain, this->UpdatePlayersLocationTimerTime);



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
	if (bShouldCleanUp)
	{
		this->CleanUpTerrain();
		this->bShouldCleanUp = false;
	}

}

void APWorld::CheckShouldGenerateTerrain()
{
	if (!this->bShouldStartGenerationCheck) { return; }

	int playeridx = 0;
	for (auto& player : PlayersInGame)
	{

		if(this->CheckPlayerLocation(player->GetActorLocation()) != this->PlayersInGameLastLocation[playeridx])
		{
			this->FindPlayerDirection(this->PlayersInGameLastLocation[playeridx]);

			this->PlayersInGameLastLocation[playeridx] = FVector2D(x_pos, y_pos);
			//UE_LOG(LogTemp, Warning, TEXT("XPOSiton: %d"), x_pos);
			//UE_LOG(LogTemp, Warning, TEXT("YPos: %d"), y_pos);
			this->bShouldGenerateTerrain = true;
		}
		playeridx++;
	}
	
}

FVector2D APWorld::CheckPlayerLocation(FVector players_location)
{
	x_pos = players_location.X / (((this->PlainSize - 1)) * this->TerrainScale);
	y_pos = players_location.Y / (((this->PlainSize - 1)) * this->TerrainScale);
	//if (players_location.X < 0) { x_pos -= 1; }
	//if (players_location.Y < 0) { y_pos -= 1; }
	return FVector2D(x_pos, y_pos);

}

void APWorld::GenerateTerrain()
{
	int playeridx = 0;
	for (auto& player : PlayersInGame)
	{
		switch (Direction)
		{
		case EDirection::D_UP:
			
			UE_LOG(LogTemp, Warning, TEXT("player: %f , %f"), this->PlayersInGameLastLocation[playeridx].X, this->PlayersInGameLastLocation[playeridx].Y);
			this->Generating(this->TerrainGenerationOrderStruct.Up, playeridx);
			
			break;
		case EDirection::D_Left:
		

			UE_LOG(LogTemp, Warning, TEXT("player: %f , %f"), this->PlayersInGameLastLocation[playeridx].X, this->PlayersInGameLastLocation[playeridx].Y);
			this->Generating(this->TerrainGenerationOrderStruct.Left, playeridx);
			break;
		case EDirection::D_Down:
		
			
			
			UE_LOG(LogTemp, Warning, TEXT("player: %f , %f"), this->PlayersInGameLastLocation[playeridx].X, this->PlayersInGameLastLocation[playeridx].Y);
			this->Generating(this->TerrainGenerationOrderStruct.Down, playeridx);
			
			break;
		case EDirection::D_Right:
			
			
			UE_LOG(LogTemp, Warning, TEXT("player: %f , %f"), this->PlayersInGameLastLocation[playeridx].X, this->PlayersInGameLastLocation[playeridx].Y);
			this->Generating(this->TerrainGenerationOrderStruct.Right, playeridx);
			break;
		default:
			break;
		}

		playeridx++;
	}
}

void APWorld::CleanUpTerrain()
{
	int player_idx = 0;
	for (auto& location : this->PlayersInGameLastLocation)
	{
		for (auto& generatedterrainchunk : GeneratedTerrainChunks)
		{
			if (generatedterrainchunk)
			{
				int x = (generatedterrainchunk->GetActorLocation().X / ((this->PlainSize - 1) * this->TerrainScale)) - location.X;
				int y = (generatedterrainchunk->GetActorLocation().Y / ((this->PlainSize - 1) * this->TerrainScale)) - location.Y;
				if (x < -1 || x > 1 || y < -1 || y > 1)
				{

					generatedterrainchunk->Destroy();


				}
			}
			
		}
	}
	
}

void APWorld::GetWorldPlayers()
{
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(this, CharacterClass, actors);
	this->PlayersInGameLastLocation.SetNum(actors.Num());
	int playeridx = 0;
	for (auto& actor : actors)
	{
		AWayFinderCharacter* player_char = Cast<AWayFinderCharacter>(actor);
		if (player_char)
		{
			this->PlayersInGame.Add(player_char);
			int x = player_char->GetActorLocation().X / ((this->PlainSize - 1) * this->TerrainScale);
			int y = player_char->GetActorLocation().Y / ((this->PlainSize - 1) * this->TerrainScale);
			if (player_char->GetActorLocation().X < 0) { x_pos -= 1; }
			if (player_char->GetActorLocation().Y < 0) { y_pos -= 1; }

			this->PlayersInGameLastLocation[playeridx] = FVector2D(x, y);
			this->bShouldStartGenerationCheck = true;
		}
		playeridx++;
	}
	

	if (PlayersInGame.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("APWorld::BeginPlay__ No players found, need player to generate world"));
	}

}

void APWorld::FindPlayerDirection(FVector2D player_loc_last)
{
	FVector2D dif_vec = FVector2D(x_pos, y_pos) - player_loc_last;
	if (dif_vec.X == 0 && dif_vec.Y == 1)
	{
		Direction = EDirection::D_UP;
		UE_LOG(LogTemp, Warning, TEXT("Up"));
		UE_LOG(LogTemp, Warning, TEXT("Dif x: %f"), dif_vec.X);
		UE_LOG(LogTemp, Warning, TEXT("Dif x: %f"), dif_vec.Y);
	}
	else if (dif_vec.X == 1 && dif_vec.Y == 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("UpLeft"));
		UE_LOG(LogTemp, Warning, TEXT("Dif x: %f"), dif_vec.X);
		UE_LOG(LogTemp, Warning, TEXT("Dif x: %f"), dif_vec.Y);
	}
	else if (dif_vec.X == 1 && dif_vec.Y == 0)
	{
		Direction = EDirection::D_Left;
		UE_LOG(LogTemp, Warning, TEXT("Left"));
		UE_LOG(LogTemp, Warning, TEXT("Dif x: %f"), dif_vec.X);
		UE_LOG(LogTemp, Warning, TEXT("Dif x: %f"), dif_vec.Y);
	}
	else if (dif_vec.X == 1 && dif_vec.Y == -1)
	{
		UE_LOG(LogTemp, Warning, TEXT("DownLeft"));
		UE_LOG(LogTemp, Warning, TEXT("Dif x: %f"), dif_vec.X);
		UE_LOG(LogTemp, Warning, TEXT("Dif x: %f"), dif_vec.Y);
	}
	else if (dif_vec.X == 0 && dif_vec.Y == -1)
	{
		Direction = EDirection::D_Down;
		UE_LOG(LogTemp, Warning, TEXT("Down"));
		UE_LOG(LogTemp, Warning, TEXT("Dif x: %f"), dif_vec.X);
		UE_LOG(LogTemp, Warning, TEXT("Dif x: %f"), dif_vec.Y);
	}
	else if (dif_vec.X == -1 && dif_vec.Y == -1)
	{
		UE_LOG(LogTemp, Warning, TEXT("DownRight"));
		UE_LOG(LogTemp, Warning, TEXT("Dif x: %f"), dif_vec.X);
		UE_LOG(LogTemp, Warning, TEXT("Dif x: %f"), dif_vec.Y);
	}
	else if (dif_vec.X == -1 && dif_vec.Y == 0)
	{
		Direction = EDirection::D_Right;
		UE_LOG(LogTemp, Warning, TEXT("Right"));
		UE_LOG(LogTemp, Warning, TEXT("Dif x: %f"), dif_vec.X);
		UE_LOG(LogTemp, Warning, TEXT("Dif x: %f"), dif_vec.Y);
	}
	else if (dif_vec.X == -1 && dif_vec.Y == 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("UpRight"));
		UE_LOG(LogTemp, Warning, TEXT("Dif x: %f"), dif_vec.X);
		UE_LOG(LogTemp, Warning, TEXT("Dif x: %f"), dif_vec.Y);
	}else
	{ }
/*	UE_LOG(LogTemp, Warning, TEXT("Dif x: %f"), dif_vec.X);
	UE_LOG(LogTemp, Warning, TEXT("Dif x: %f"), dif_vec.Y);*/


}

void APWorld::SetGenerationOrder()
{
	


}

void APWorld::Generating(TArray<FVector2D> generation_order, int player_idx)
{
	if (!this->bIsGenerating)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Began Generation"));
		
		float xx = (PlayersInGameLastLocation[player_idx].X + generation_order[GenerationCurrent].X) * ((this->PlainSize - 1) * this->TerrainScale);
		float yy = (PlayersInGameLastLocation[player_idx].Y + generation_order[GenerationCurrent].Y) * ((this->PlainSize - 1) * this->TerrainScale);

		this->ChunkGenerator->SetGeneratorLocation(FVector(xx, yy, 0));
		this->ChunkGenerator->InitCalculations(1);
		this->bIsGenerating = true;
	}

	if (this->ChunkGenerator->bIsDone)
	{
		this->ChunkGenerator->GetGenerationData(this->Vertices, this->Triangles, this->VertexColors, this->Normals, this->UV0, this->Tangents);
		//UE_LOG(LogTemp, Warning, TEXT("Size: %d"), Vertices.Num());
		//UE_LOG(LogTemp, Warning, TEXT("Size: %d"), Triangles.Num());
//UE_LOG(LogTemp, Warning, TEXT("Size: %d"), Normals.Num());


		float xx = (PlayersInGameLastLocation[player_idx].X + generation_order[GenerationCurrent].X) * ((this->PlainSize - 1) * this->TerrainScale);
		float yy = (PlayersInGameLastLocation[player_idx].Y + generation_order[GenerationCurrent].Y) * ((this->PlainSize - 1) * this->TerrainScale);
		//UE_LOG(LogTemp, Warning, TEXT("Scale x: %f Scale y: %f"), xx, yy);

		APTerrain* generated_actor = this->GetWorld()->SpawnActor<APTerrain>(APTerrain::StaticClass());

		if (generated_actor)
		{
			//UE_LOG(LogTemp, Warning, TEXT("ACTOR"));
			generated_actor->SetActorLocation(FVector(xx, yy, 0));
			generated_actor->GenerateMeshFromWorld(this->Vertices, this->Triangles, this->VertexColors, this->Normals, this->UV0, this->Tangents);
			if (this->LandscapeMaterial)
			{
				generated_actor->SetMeshMaterial(this->LandscapeMaterial);
			}
			//UE_LOG(LogTemp, Warning, TEXT("Size: %d"), Vertices.Num());
			//UE_LOG(LogTemp, Warning, TEXT("Size: %d"), Triangles.Num());
			//UE_LOG(LogTemp, Warning, TEXT("Size: %d"), Normals.Num());

		}
		this->GeneratedTerrainChunks.Add(generated_actor);
		this->GenerationCurrent++;
		this->bIsGenerating = false;
		//UE_LOG(LogTemp, Warning, TEXT("Generated One piece of terrain"));
	}

	if (GenerationCurrent >= 3)
	{
		bIsGenerating = false;
		this->bShouldGenerateTerrain = false;
		this->GenerationCurrent = 0;
		this->bShouldCleanUp = true;
		//UE_LOG(LogTemp, Warning, TEXT("End of generation"));
	}
}
