// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Materials/Material.h"
#include "PWorld.generated.h"

class APTerrain;
class AChunkGenerator;
class AWayFinderCharacter;

struct TerrainGenerationOrder {
	TArray<FVector2D> Up;
	TArray<FVector2D> UpLeft;
	TArray<FVector2D> Left;
	TArray<FVector2D> DownLeft;
	TArray<FVector2D> Down;
	TArray<FVector2D> DownRight;
	TArray<FVector2D> Right;
	TArray<FVector2D> UpRight;

	TerrainGenerationOrder()
	{
		Up.SetNum(3);
		Up[0] = FVector2D(1, 1);
		Up[1] = FVector2D(0, 1);
		Up[2] = FVector2D(-1, 1);

		UpLeft.SetNum(5);
		UpLeft[0] = FVector2D(-2, 0);
		UpLeft[1] = FVector2D(-2, 1);
		UpLeft[2] = FVector2D(-2, 2);
		UpLeft[3] = FVector2D(-1, 2);
		UpLeft[4] = FVector2D(0, 2);

		Left.SetNum(3);
		Left[0] = FVector2D(1, -1);
		Left[1] = FVector2D(1, 0);
		Left[2] = FVector2D(1, 1);

		DownLeft.SetNum(5);
		DownLeft[0] = FVector2D(0, -2);
		DownLeft[1] = FVector2D(-1, -2);
		DownLeft[2] = FVector2D(-2, -2);
		DownLeft[3] = FVector2D(-2, -1);
		DownLeft[4] = FVector2D(-2, 0);

		Down.SetNum(3);
		Down[0] = FVector2D(-1, -1);
		Down[1] = FVector2D(0, -1);
		Down[2] = FVector2D(1, -1);

		DownRight.SetNum(5);
		DownRight[0] = FVector2D(2, 0);
		DownRight[1] = FVector2D(2, -1);
		DownRight[2] = FVector2D(2, -2);
		DownRight[3] = FVector2D(1, -2);
		DownRight[4] = FVector2D(0, -2);

		Right.SetNum(3);
		Right[0] = FVector2D(-1, 1);
		Right[1] = FVector2D(-1, 0);
		Right[2] = FVector2D(-1, -1);

		UpRight.SetNum(5);
		UpRight[0] = FVector2D(0, 2);
		UpRight[1] = FVector2D(1, 2);
		UpRight[2] = FVector2D(2, 2);
		UpRight[3] = FVector2D(2, 1);
		UpRight[4] = FVector2D(2, 0);
		
	}

};

enum class EDirection {

	D_UP,
	D_Left,
	D_Down,
	D_Right
};

UCLASS()
class WAYFINDER_API APWorld : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APWorld();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//True or false based on if player has moved enough to have new terrain generate
	bool bShouldGenerateTerrain;
	bool bShouldStartGenerationCheck;

private:

	void CheckShouldGenerateTerrain();

	FVector2D CheckPlayerLocation(FVector players_location);

	void GenerateTerrain();

	void CleanUpTerrain();

	void GetWorldPlayers();

	void FindPlayerDirection(FVector2D player_loc_last);

	void SetGenerationOrder();

	void Generating(TArray<FVector2D> generation_order,int player_idx);

private:

	//Small delay to allow for players to spawn and map to find locale (player location) of terrain chunk generation
	FTimerHandle GetPlayersLocationTimer;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Paramaters", meta = (AllowPrivateAccess = "true"))
	float GetPlayersLocationTimerTime;

	//A timer hand to check for player location update I believe this will help prevent tick cycles
	FTimerHandle UpdatePlayersLocationTimer;
	float UpdatePlayersLocationTimerTime;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "World Parameters", meta = (AllowPrivateAccess = "true"))
	TArray<APTerrain*> GeneratedTerrainChunks;

	AChunkGenerator* ChunkGenerator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Paramaters", meta = (AllowPrivateAccess = "true"))
	int PlainSize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Paramaters", meta = (AllowPrivateAccess = "true"))
		int Seed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Paramaters", meta = (AllowPrivateAccess = "true"))
	float Scale;

	//Adjusts definition of noise sample
	//adds more bumps
	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
		int Octaves;

	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters", meta = (ClampMin = 0.0001, ClampMax = 0.9999))
		float Persistence;

	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters", meta = (ClampMin = 1.0, ClampMax = 10.0))
		float Lacunarity;

	//Adjusts Z (up) component of each vertice
	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
		float HeightMultiplier;

	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
		UCurveFloat* HeightAdjustmentCurve;

	//Determines how many "chunks" out will render/spawn
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Parameters", meta = (AllowPrivateAccess = "true"))
	int GenerateDistance;

	//Adjusts the length between vertices (more land cover less vertices) 
	//who can complain?
	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
		float TerrainScale;

	//Distance player has to be from new terrain chunk to generate chunk
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Parameters", meta = (AllowPrivateAccess = "true"))
	int GenerateDistanceThreshold;

	TArray<AWayFinderCharacter*> PlayersInGame;

	TArray<FVector2D> PlayersInGameLastLocation;

	TerrainGenerationOrder TerrainGenerationOrderStruct;

	EDirection Direction;

	bool bIsGenerating;

	int GenerationCurrent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Paramaters", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AWayFinderCharacter> CharacterClass;

	UPROPERTY(VisibleAnywhere, Category = "Mesh Paramaters")
	int y_pos;
	UPROPERTY(VisibleAnywhere, Category = "Mesh Paramaters")
	int x_pos;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Paramaters", meta = (AllowPrivateAccess = "true"))
	UMaterial* LandscapeMaterial;

	bool bShouldCleanUp;


	

	//vertices matrix (x,y,z)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "World Parameters", meta = (AllowPrivateAccess = "true"))
	TArray<FVector> Vertices;
	//order in which vertices should be joined together creating a mesh of triangles
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters")
	TArray<int32> Triangles;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters")
	TArray<FLinearColor> VertexColors;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters")
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters")
	TArray<FVector> Normals;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters")
	TArray<FVector2D> UV0;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters")
	TArray<FProcMeshTangent> Tangents;

};
