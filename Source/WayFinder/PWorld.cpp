// Fill out your copyright notice in the Description page of Project Settings.


#include "PWorld.h"
#include "ChunkGenerator.h"
#include "PTerrain.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstance.h"

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
	PowerValue = 1.f;
	UVScale = 2;

}


// Called when the game starts or when spawned
void APWorld::BeginPlay()
{
	Super::BeginPlay();

	

	this->ChunkGenerator = GetWorld()->SpawnActor<AChunkGenerator>(AChunkGenerator::StaticClass());
	if (!ChunkGenerator)
	{
		UE_LOG(LogTemp, Warning, TEXT("No chunk generator"));
	}
	else {
		this->ChunkGenerator->PWorldOwner = this;
		this->ChunkGenerator->range = this->range; //for foliage spawn max random spawn scale
		this->ChunkGenerator->bounds = this->bounds; //for foliage spawn random spawn min bounds scale
	}



	GetWorldTimerManager().SetTimer(this->GetPlayersLocationTimer, this, &APWorld::GetWorldPlayers, this->GetPlayersLocationTimerTime);

	if (HeightAdjustmentCurve)
	{
		this->ChunkGenerator->SetGeneratorParams( UVScale,PlainSize, TerrainScale, Seed, Scale, PowerValue, Octaves, Persistence, Lacunarity, HeightMultiplier, HeightAdjustmentCurve);
		UE_LOG(LogTemp, Warning, TEXT("Nchunk_generator_plainsize: %d"), this->ChunkGenerator->PlainSize);
	}
	else
	{
		this->ChunkGenerator->SetGeneratorParams(UVScale, PlainSize, TerrainScale, Seed, Scale, PowerValue, Octaves, Persistence, Lacunarity, HeightMultiplier);
		UE_LOG(LogTemp, Warning, TEXT("Nchunk_generator_plainsize: %d"), this->ChunkGenerator->PlainSize);
	}

	
	
	

	//GetWorldTimerManager().SetTimer(this->UpdatePlayersLocationTimer, this, &APWorld::CheckShouldGenerateTerrain, this->UpdatePlayersLocationTimerTime);



}


// Called every frame
void APWorld::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//If level has opened but not players are loaded in (mainly targeted towards multiplayer situations)
	//if (!this->bShouldStartGenerationCheck)
	//{
	//	GetWorldPlayers();
	//}

	//if no terrain to generate check players location to check if generation is needed
	if (!this->bShouldGenerateTerrain)
	{
		this->CheckShouldGenerateTerrain();
	}
	
	//If terrain should be generated generate terrain
	if (bShouldGenerateTerrain)
	{
		this->GenerateTerrain();
	}

	//If should try deleting chunks and is not still generating chunks try to clean up
	if (bShouldCleanUp && !bIsGenerating)
	{
		this->CleanUpTerrain();
		this->bShouldCleanUp = false; //Only one iteration is necessary, reset for after next generation event
	}

}


void APWorld::CheckShouldGenerateTerrain()
{
	//enabled when a player is found by PWorld, otherwise no generation checks should happen
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
	if (players_location.X < 0) { x_pos -= 1; } //To adjust for origin of chunks being bottom right corner
	if (players_location.Y < 0) { y_pos -= 1; } //To adjust for origin of chunks being bottom right corner
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
			//UE_LOG(LogTemp, Warning, TEXT("player: %f , %f"), this->PlayersInGameLastLocation[playeridx].X, this->PlayersInGameLastLocation[playeridx].Y);
			this->Generating(this->TerrainGenerationOrderStruct.Up, playeridx);

			break;
		case EDirection::D_UpLeft:
			this->Generating(this->TerrainGenerationOrderStruct.UpLeft, playeridx);

			break;
		case EDirection::D_Left:
			this->Generating(this->TerrainGenerationOrderStruct.Left, playeridx);

			break;
		case EDirection::D_DownLeft:
			this->Generating(this->TerrainGenerationOrderStruct.DownLeft, playeridx);

			break;
		case EDirection::D_Down:
		//	UE_LOG(LogTemp, Warning, TEXT("player: %f , %f"), this->PlayersInGameLastLocation[playeridx].X, this->PlayersInGameLastLocation[playeridx].Y);
			this->Generating(this->TerrainGenerationOrderStruct.Down, playeridx);

			break;
		case EDirection::D_DownRight:
			this->Generating(this->TerrainGenerationOrderStruct.DownRight, playeridx);

			break;
		case EDirection::D_Right:
			//UE_LOG(LogTemp, Warning, TEXT("player: %f , %f"), this->PlayersInGameLastLocation[playeridx].X, this->PlayersInGameLastLocation[playeridx].Y);
			this->Generating(this->TerrainGenerationOrderStruct.Right, playeridx);

			break;
		case EDirection::D_UpRight:
			this->Generating(this->TerrainGenerationOrderStruct.UpRight, playeridx);

			break;
		case EDirection::D_Starting:
			this->Generating(this->TerrainGenerationOrderStruct.Start, playeridx);

			break;
		default:
			break;
		}
		playeridx++;
	}
			
}


void APWorld::GenerateTerrainStart()
{
	if (this->bShouldStartGenerationCheck)
	{
		this->bShouldGenerateTerrain = true;
		this->Direction = EDirection::D_Starting;
	}
}


void APWorld::CleanUpTerrain()
{
	//iterate through each player location in the world
	for (auto& location : this->PlayersInGameLastLocation)
	{
		//Check if any of the generated terrain chunks are further then the generation distanc threshold and delete if so
		//this will not work for players far apart because their chunks will be deleted
		//Could solve by each player having a list of terrain chunks they have generated and check with that list
		// would require more data to be held by player, but still not tons
		//Could have a min distance to be within to check terrain chunks for deletion
		for (int i = 0; i < this->GeneratedTerrainChunks.Num(); i++)
		{
			//See if terrain's isdestroyed flag has been enabled 
			//needed to do this so the index would be cleared and deleted, otherwise array never shrank and got very large (full of null actors)
			if (this->GeneratedTerrainChunks[i]->bIsDestroyed != true)
			{
				//compare terrain chunk location to current player location
				int x = (this->GeneratedTerrainChunks[i]->GetActorLocation().X / ((this->PlainSize - 1) * this->TerrainScale)) - location.X;
				int y = (this->GeneratedTerrainChunks[i]->GetActorLocation().Y / ((this->PlainSize - 1) * this->TerrainScale)) - location.Y;
				if (x < -GenerateDistanceThreshold || x > GenerateDistanceThreshold || y < -GenerateDistanceThreshold || y > GenerateDistanceThreshold)
				{
					
					//this->GeneratedTerrainChunkLocations.Remove(FVector2D(GeneratedTerrainChunks[i]->GetActorLocation().X, GeneratedTerrainChunks[i]->GetActorLocation().Y));
					this->GeneratedTerrainChunks[i]->DestroyTerrain();//Set terrains isDestroyed flag to true
				}
			}
			else
			{
				//Destroy actor and remove index from array
				this->GeneratedTerrainChunks[i]->Destroy();
				this->GeneratedTerrainChunks.RemoveAt(i);
			}
			
		}
		//Shrink array at end of each player location check
		this->GeneratedTerrainChunks.Shrink();
	}
	
}


void APWorld::GetWorldPlayers()
{
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(this, CharacterClass, actors);
	int playeridx = 0;
	for (auto& actor : actors)
	{
		AWayFinderCharacter* player_char = Cast<AWayFinderCharacter>(actor);
		if (player_char)
		{
			this->PlayersInGame.Add(player_char);
			x_pos = player_char->GetActorLocation().X / ((this->PlainSize - 1) * this->TerrainScale);
			y_pos  = player_char->GetActorLocation().Y / ((this->PlainSize - 1) * this->TerrainScale);
			if (player_char->GetActorLocation().X < 0) { x_pos -= 1; }
			if (player_char->GetActorLocation().Y < 0) { y_pos -= 1; }

			this->PlayersInGameLastLocation.Add(FVector2D(x_pos, y_pos));
			this->bShouldStartGenerationCheck = true;
		}
		playeridx++;
	}
	

	if (PlayersInGame.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("APWorld::BeginPlay__ No players found, need player to generate world"));
		GetWorldPlayers();
	}
	else
	{
		this->GenerateTerrainStart();
	}


}


void APWorld::FindPlayerDirection(FVector2D player_loc_last)
{
	FVector2D dif_vec = FVector2D(x_pos, y_pos) - player_loc_last;
	if (dif_vec.X == 0 && dif_vec.Y == 1)
	{
		Direction = EDirection::D_UP;
		UE_LOG(LogTemp, Warning, TEXT("Up"));
		//UE_LOG(LogTemp, Warning, TEXT("Dif x: %f"), dif_vec.X);
		//UE_LOG(LogTemp, Warning, TEXT("Dif x: %f"), dif_vec.Y);
	}
	else if (dif_vec.X == 1 && dif_vec.Y == 1)
	{
		Direction = EDirection::D_UpLeft;
		UE_LOG(LogTemp, Warning, TEXT("UpLeft"));
		//UE_LOG(LogTemp, Warning, TEXT("Dif x: %f"), dif_vec.X);
		//UE_LOG(LogTemp, Warning, TEXT("Dif x: %f"), dif_vec.Y);
	}
	else if (dif_vec.X == 1 && dif_vec.Y == 0)
	{
		Direction = EDirection::D_Left;
		UE_LOG(LogTemp, Warning, TEXT("Left"));
		//UE_LOG(LogTemp, Warning, TEXT("Dif x: %f"), dif_vec.X);
		//UE_LOG(LogTemp, Warning, TEXT("Dif x: %f"), dif_vec.Y);
	}
	else if (dif_vec.X == 1 && dif_vec.Y == -1)
	{
		Direction = EDirection::D_DownLeft;
		UE_LOG(LogTemp, Warning, TEXT("DownLeft"));
		//UE_LOG(LogTemp, Warning, TEXT("Dif x: %f"), dif_vec.X);
		//UE_LOG(LogTemp, Warning, TEXT("Dif x: %f"), dif_vec.Y);
	}
	else if (dif_vec.X == 0 && dif_vec.Y == -1)
	{
		Direction = EDirection::D_Down;
		UE_LOG(LogTemp, Warning, TEXT("Down"));
		//UE_LOG(LogTemp, Warning, TEXT("Dif x: %f"), dif_vec.X);
		//UE_LOG(LogTemp, Warning, TEXT("Dif x: %f"), dif_vec.Y);
	}
	else if (dif_vec.X == -1 && dif_vec.Y == -1)
	{
		Direction = EDirection::D_DownRight;
		UE_LOG(LogTemp, Warning, TEXT("DownRight"));
		//UE_LOG(LogTemp, Warning, TEXT("Dif x: %f"), dif_vec.X);
		//UE_LOG(LogTemp, Warning, TEXT("Dif x: %f"), dif_vec.Y);
	}
	else if (dif_vec.X == -1 && dif_vec.Y == 0)
	{
		Direction = EDirection::D_Right;
		UE_LOG(LogTemp, Warning, TEXT("Right"));
		//UE_LOG(LogTemp, Warning, TEXT("Dif x: %f"), dif_vec.X);
		//UE_LOG(LogTemp, Warning, TEXT("Dif x: %f"), dif_vec.Y);
	}
	else if (dif_vec.X == -1 && dif_vec.Y == 1)
	{
		Direction = EDirection::D_UpRight;
		UE_LOG(LogTemp, Warning, TEXT("UpRight"));
		//UE_LOG(LogTemp, Warning, TEXT("Dif x: %f"), dif_vec.X);
		//UE_LOG(LogTemp, Warning, TEXT("Dif x: %f"), dif_vec.Y);
	}else
	{
		UE_LOG(LogTemp, Warning, TEXT("Else"));
	}
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
		this->ChunkGenerator->rand_num = FMath::RandRange(0, 1);
		
		
		bool bShouldThread = true; //Set thread enabled (default)
		//Find terrain to be generated world location (grid location + generation order grid location * plainsize * terrain scale)
		float xx = (PlayersInGameLastLocation[player_idx].X + generation_order[GenerationCurrent].X) * ((this->PlainSize - 1) * this->TerrainScale);
		float yy = (PlayersInGameLastLocation[player_idx].Y + generation_order[GenerationCurrent].Y) * ((this->PlainSize - 1) * this->TerrainScale);
		/*for (auto& generatedchunks : this->GeneratedTerrainChunks)
		{
			//Attempt at a check to prevent duplicate spawning, needs tweaking
			if (FVector2D(generatedchunks->GetActorLocation().X, generatedchunks->GetActorLocation().Y) == FVector2D(xx, yy))
			{
				UE_LOG(LogTemp, Warning, TEXT("APWorld::Generating__ toGen: (%d)"), GenerationCurrent);
				UE_LOG(LogTemp, Warning, TEXT("APWorld::Generating__ toGen: (%f, %f) -> terrain: (%f, %f)"), xx, yy, generatedchunks->GetActorLocation().X, generatedchunks->GetActorLocation().Y);
				bShouldThread = false; // disable thread so terrain isn't generated
				this->GenerationCurrent++; //increase generated terrain count (didn't actually generate but this process ends by generation count == generation count target)
				UE_LOG(LogTemp, Warning, TEXT("APWorld::Generating__ should thread = false"));
			}
		}*/
		if (bShouldThread)
		{
			this->ChunkGenerator->SetGeneratorLocation(FVector(xx, yy, 0));
			this->ChunkGenerator->InitCalculations(1); //Start thread calculations
			this->bIsGenerating = true; //enable check for thread done
		}
		
	}

	if (this->ChunkGenerator->bIsDone && this->bIsGenerating)
	{
		//Get mesh data
		this->ChunkGenerator->GetGenerationData(this->Vertices, this->Triangles, this->VertexColors, this->Normals, this->UV0, this->Tangents);
		//Get foliage spawn location data
		this->ChunkGenerator->GetFoliageSpawnLocations(WaterFoliageSpawnVertices, MeadowFoliageSpawnVertices, ForestFoliageSpawnVertices, FootHillFoliageSpawnVertices, MountainFoliageSpawnVertices);

		//Find terrain to be generated world location (grid location + generation order grid location * plainsize * terrain scale)
		float xx = (PlayersInGameLastLocation[player_idx].X + generation_order[GenerationCurrent].X) * ((this->PlainSize - 1) * this->TerrainScale);
		float yy = (PlayersInGameLastLocation[player_idx].Y + generation_order[GenerationCurrent].Y) * ((this->PlainSize - 1) * this->TerrainScale);
		//UE_LOG(LogTemp, Warning, TEXT("Scale x: %f Scale y: %f"), xx, yy);
		//this->GeneratedTerrainChunkLocations.Add(FVector2D(xx, yy));
		APTerrain* generated_actor = this->GetWorld()->SpawnActor<APTerrain>(APTerrain::StaticClass()); //spawn terrain chunk

		if (generated_actor)
		{
			//UE_LOG(LogTemp, Warning, TEXT("ACTOR"));
			FVector spawn_loc(xx, yy, 0); 
			generated_actor->SetActorLocation(spawn_loc); //Set spawn location to correct position
			generated_actor->PWorld = this; //Set world owner of terrain
			//FVector world_off = (FVector(xx - (xx (this->PlainSize - 1) * this->TerrainScale, ,))
			generated_actor->WorldOffset = this->WorldOffset; //For water generation per chunk
			if(this->WaterMaterial){ generated_actor->WaterMaterial = this->WaterMaterial;} //Set water material
			if (this->WaterBox) { generated_actor->WaterBox->SetStaticMesh(WaterBox); } //Set water mesh
			if (this->WaterMatInst)
			{
				//generated_actor->SetMeshMaterialI(this->WaterMatInst);
				generated_actor->WaterBox->SetMaterial(0, this->WaterMatInst);
			}

			//Sets terrain chunks params
			generated_actor->SetTerrainParams(this->UVScale, this->PlainSize, this->TerrainScale, this->Seed, this->Scale, this->PowerValue, this->Octaves, this->Persistence, this->Lacunarity, this->HeightMultiplier, this->HeightAdjustmentCurve);
			//Generates terrain chunk with mesh data
			generated_actor->GenerateMeshFromWorld(this->Vertices, this->Triangles, this->VertexColors, this->Normals, this->UV0, this->Tangents);
			//Gives foliage spawn location arrays to terrain chunk (need to figure this system out more)
			generated_actor->GenerateFoliageSpawns(WaterFoliageSpawnVertices, MeadowFoliageSpawnVertices, ForestFoliageSpawnVertices, FootHillFoliageSpawnVertices, 
				MountainFoliageSpawnVertices, spawn_loc);
			//Apply landscape auto mat
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
		this->bIsGenerating = false; //disable is generating to allow for more terrain chunk generation
		//UE_LOG(LogTemp, Warning, TEXT("Generated One piece of terrain"));
	}


	//end of generation cycle
	if (GenerationCurrent >= generation_order.Num() * this->GenerateDistance)
	{
		//reset all flags and counters
		bIsGenerating = false; 
		this->bShouldGenerateTerrain = false;
		this->GenerationCurrent = 0;
		this->bShouldCleanUp = true; //Enable the deletion of terrain chunks
		//UE_LOG(LogTemp, Warning, TEXT("End of generation"));
	}
}

