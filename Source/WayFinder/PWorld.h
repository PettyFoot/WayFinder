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
class UMaterialInstance;

UENUM(BlueprintType)
enum class EBiome : uint8
{
	Biome_Water = 0,
	Biome_Meadow,
	Biome_Forest,
	Biome_FootHills,
	Biome_Mountains,
	Biome_Default

};

USTRUCT(BlueprintType)
struct FBiome
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
	EBiome Biome;

	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
	int BiomeFoliageDensity;


};

struct TerrainGenerationOrder {
	TArray<FVector2D> Up;
	TArray<FVector2D> UpLeft;
	TArray<FVector2D> Left;
	TArray<FVector2D> DownLeft;
	TArray<FVector2D> Down;
	TArray<FVector2D> DownRight;
	TArray<FVector2D> Right;
	TArray<FVector2D> UpRight;
	TArray<FVector2D> Start;

	TerrainGenerationOrder()
	{
		Up.SetNum(3);
		Up[0] = FVector2D(1, 1);
		Up[1] = FVector2D(0, 1);
		Up[2] = FVector2D(-1, 1);

		UpLeft.SetNum(5);
		UpLeft[0] = FVector2D(-1, 1);
		UpLeft[1] = FVector2D(0, 1);
		UpLeft[2] = FVector2D(1, 1);
		UpLeft[3] = FVector2D(1, 0);
		UpLeft[4] = FVector2D(1, -1);

		Left.SetNum(4);
		Left[0] = FVector2D(1, -1);
		Left[1] = FVector2D(1, 0);
		Left[2] = FVector2D(1, 1);
		Left[3] = FVector2D(0, 2);

		DownLeft.SetNum(5);
		DownLeft[0] = FVector2D(1, 1);
		DownLeft[1] = FVector2D(1, 0);
		DownLeft[2] = FVector2D(1, -1);
		DownLeft[3] = FVector2D(0, -1);
		DownLeft[4] = FVector2D(-1, -1);

		Down.SetNum(3);
		Down[0] = FVector2D(-1, -1);
		Down[1] = FVector2D(0, -1);
		Down[2] = FVector2D(1, -1);

		DownRight.SetNum(5);
		DownRight[0] = FVector2D(1, -1);
		DownRight[1] = FVector2D(0, -1);
		DownRight[2] = FVector2D(1, -1);
		DownRight[3] = FVector2D(-1, 0);
		DownRight[4] = FVector2D(-1, 1);

		Right.SetNum(4);
		Right[0] = FVector2D(-1, 1);
		Right[1] = FVector2D(-1, 0);
		Right[2] = FVector2D(-1, -1);
		Left[3] = FVector2D(0, 2);

		UpRight.SetNum(5);
		UpRight[0] = FVector2D(1, -1);
		UpRight[1] = FVector2D(1, 0);
		UpRight[2] = FVector2D(1, 1);
		UpRight[3] = FVector2D(0, 1);
		UpRight[4] = FVector2D(-1, 1);

		Start.SetNum(9);
		Start[0] = FVector2D(0, 0);
		Start[1] = FVector2D(0, 1);
		Start[2] = FVector2D(-1, 1);
		Start[3] = FVector2D(-1, 0);
		Start[4] = FVector2D(-1, -1);
		Start[5] = FVector2D(0, -1);
		Start[6] = FVector2D(1, -1);
		Start[7] = FVector2D(1, 0);
		Start[8] = FVector2D(1, 1);
		
		
	}

};



USTRUCT(BlueprintType)
struct FNoiseSetting 
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Paramaters", meta = (AllowPrivateAccess = "true"))
		int Seed;

	//Adjusts Z (up) component of each vertice
	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
		float HeightMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters", meta = (AllowPrivateAccess = "true"))
		float Scale;

	//Adjusts the length between vertices (more land cover less vertices) 
	//who can complain?
	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
		float TerrainScale;

	//Adjusts definition of noise sample
	//adds more bumps
	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters", meta = (ClampMin = 1, ClampMax = 11))
		int Octaves;

	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters", meta = (ClampMin = 0.0001, ClampMax = 0.9999))
		float Persistence;

	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters", meta = (ClampMin = 1.0, ClampMax = 10.0))
		float Lacunarity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters", meta = (AllowPrivateAccess = "true"))
		float PowerValue;

	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
		UCurveFloat* HeightAdjustmentCurve;

	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
		int UVScale;


};

USTRUCT(BlueprintType)
struct FNoiseFilter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Noise Settings")
	FNoiseSetting NoiseSetting;

	UPROPERTY(EditAnywhere, Category = "Noise Settings")
	bool bIsEnabled;

	UPROPERTY(EditAnywhere, Category = "Noise Settings")
	bool bUsePreviousLayerAsMask;

	UPROPERTY(EditAnywhere, Category = "Noise Settings")
	bool bUseDomainDistortion;

	float EvaluatePoint(FVector2D grid_loc)
	{

		float noise_val = 0;
		float frequency = 1;
		float amplitude = 1.f;
		for (int w = 0; w < NoiseSetting.Octaves; w++)
		{
			float x_samp = ((grid_loc.X + NoiseSetting.Seed) * frequency) / NoiseSetting.Scale; // NoiseSetting.Scale;
			float y_samp = ((grid_loc.Y + NoiseSetting.Seed) * frequency) / NoiseSetting.Scale; // NoiseSetting.Scale;
			//UE_LOG(LogTemp, Warning, TEXT("perlin Noisefilter X: %f"), x_samp);
			//UE_LOG(LogTemp, Warning, TEXT("perlin Noisefilter Y: %f"), y_samp);
			float val = 0.f;
			val = FMath::PerlinNoise2D(FVector2D(x_samp, y_samp));
			
			noise_val += FMath::Clamp(val * amplitude, 0.f, 1.f); //(val + 1) * .5f * amplitude;
			frequency *= NoiseSetting.Lacunarity;
			amplitude *= NoiseSetting.Persistence;
 		}
		//Lowers base heigt (creates a floor of sorts)
		noise_val = FMath::Pow(noise_val, NoiseSetting.PowerValue);
		//UE_LOG(LogTemp, Warning, TEXT("perlin Noisefilter: %f"), noise_val * NoiseSetting.HeightMultiplier);
		return noise_val;
	}

	float EvaluatePointDomainDistorted(FVector2D point)
	{
		FVector2D aa = FVector2D(EvaluatePoint(point + FVector2D(0.0, 0.0)), EvaluatePoint(point + FVector2D(5.2, 1.3)));

		return EvaluatePoint(point + 90.f * aa);
	}


};

enum class EDirection {

	D_UP,
	D_UpLeft,
	D_Left,
	D_DownLeft,
	D_Down,
	D_DownRight,
	D_Right,
	D_UpRight,
	D_Starting
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Paramaters")
		TArray<FNoiseFilter> NoiseFilters;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Foliage Paramaters")
	TSubclassOf<AActor> FoliageClass;

private:

	void CheckShouldGenerateTerrain();

	FVector2D CheckPlayerLocation(FVector players_location);

	void GenerateTerrain();

	void GenerateTerrainStart();

	void CleanUpTerrain();

	void GetWorldPlayers();

	void FindPlayerDirection(FVector2D player_loc_last);

	void SetGenerationOrder();

	void Generating(TArray<FVector2D> generation_order,int player_idx);

public:


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Foliage Paramaters")
		TArray<UStaticMesh*> WaterFoliageSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Foliage Paramaters")
		TArray<UStaticMesh*> MeadowFoliageSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Foliage Paramaters")
		TArray<UStaticMesh*> ForestFoliageSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Foliage Paramaters")
		TArray<UStaticMesh*> FootHillFoliageSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Foliage Paramaters")
		TArray<UStaticMesh*> MountainFoliageSpawn;

private:

	//Small delay to allow for players to spawn and map to find locale (player location) of terrain chunk generation
	FTimerHandle GetPlayersLocationTimer;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Paramaters", meta = (AllowPrivateAccess = "true"))
	float GetPlayersLocationTimerTime;

	//A timer hand to check for player location update I believe this will help prevent tick cycles
	FTimerHandle UpdatePlayersLocationTimer;
	float UpdatePlayersLocationTimerTime;

protected:

	/* Default Terrain noise settings, if NoiseFilter array is less than 2 elements long */

	//Adjusts the length between vertices (more land cover less vertices) 
	//who can complain?
	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
		float TerrainScale;

	//Scale of UVs (multiples of 2s work better)
	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
		int UVScale;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Paramaters", meta = (AllowPrivateAccess = "true"))
		int PlainSize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Paramaters", meta = (AllowPrivateAccess = "true"))
		int Seed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters", meta = (AllowPrivateAccess = "true"))
		float Scale;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters", meta = (AllowPrivateAccess = "true"))
		float PowerValue;

	//for foliage spawn max random spawn scale
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Foliage Paramaters", meta = (AllowPrivateAccess = "true"))
	float range;

	//for foliage spawn random spawn min bounds scale
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Foliage Paramaters", meta = (AllowPrivateAccess = "true"))
	float bounds;

	//Adjusts definition of noise sample
	//adds more bumps
	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
		int Octaves;

	//Adjusts height effect per octave
	//Like LOD percent of triangles each LOD (except LOD is replaced with octaves here)
	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters", meta = (ClampMin = 0.0001, ClampMax = 0.9999))
		float Persistence;

	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters", meta = (ClampMin = 1.0, ClampMax = 10.0))
		float Lacunarity;

	//Adjusts Z (up) component of each vertice
	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
		float HeightMultiplier;

	//Curve to adjust for height values
	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
		UCurveFloat* HeightAdjustmentCurve;

	

	//Distance player has to be from new terrain chunk to generate chunk
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Parameters", meta = (AllowPrivateAccess = "true"))
		int GenerateDistanceThreshold;

	//Determines how many "chunks" out will render/spawn
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Parameters", meta = (AllowPrivateAccess = "true"))
		int GenerateDistance;

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "World Parameters", meta = (AllowPrivateAccess = "true"))
	TArray<APTerrain*> GeneratedTerrainChunks;

	//Current chunk generator, may need a seperate chunk generator for each player.
	AChunkGenerator* ChunkGenerator;

	//List of players in game, to generate terrain around
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters", meta = (AllowPrivateAccess = "true"))
	TArray<AWayFinderCharacter*> PlayersInGame;

	//List of current player grid locations
	TArray<FVector2D> PlayersInGameLastLocation;

	//Struct holding look up chunk generation grid locations
	TerrainGenerationOrder TerrainGenerationOrderStruct;

	//direction a player is travelling in
	EDirection Direction;

	bool bIsGenerating;

	int GenerationCurrent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Paramaters", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AWayFinderCharacter> CharacterClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters")
	int y_pos;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters")
	int x_pos;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Paramaters", meta = (AllowPrivateAccess = "true"))
	UMaterial* LandscapeMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters", meta = (AllowPrivateAccess = "true"))
		UMaterial* WaterMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters", meta = (AllowPrivateAccess = "true"))
	UMaterialInstance* WaterMatInst;

	//clear later
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters", meta = (AllowPrivateAccess = "true"))
		FVector WorldOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters", meta = (AllowPrivateAccess = "true"))
		UStaticMesh* WaterBox;

	bool bShouldCleanUp;

	//vertices matrix (x,y,z)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "World Parameters", meta = (AllowPrivateAccess = "true"))
	TArray<FVector> Vertices;

	UPROPERTY(BlueprintReadOnly, Category = "Mesh Paramaters")
	TArray<FVector> WaterFoliageSpawnVertices;

	UPROPERTY(BlueprintReadOnly, Category = "Mesh Paramaters")
		TArray<FVector> MeadowFoliageSpawnVertices;

	UPROPERTY(BlueprintReadOnly, Category = "Mesh Paramaters")
		TArray<FVector> ForestFoliageSpawnVertices;

	UPROPERTY(BlueprintReadOnly, Category = "Mesh Paramaters")
		TArray<FVector> FootHillFoliageSpawnVertices;

	UPROPERTY(BlueprintReadOnly, Category = "Mesh Paramaters")
		TArray<FVector> MountainFoliageSpawnVertices;



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
