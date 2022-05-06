// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ChunkGenerator.generated.h"

class AGenerateThread;

UCLASS()
class WAYFINDER_API AChunkGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChunkGenerator();

public:	

	//print current calcs in queue
	void PrintCalcData();

	//Get current calcs from queue to print
	int32 ProcessedCalculation;

	class AGenerateThread* GenerateThread = nullptr;

	//contains the current runnable
	FRunnableThread* CurrentRunningThread = nullptr;

	TQueue<int32> ThreadCalcQueue;

public:
	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		void InitCalculations(int32 calculations);

	//vertices matrix (x,y,z)
	UPROPERTY(BlueprintReadOnly, Category = "Mesh Paramaters")
		TArray<FVector> Vertices;

	//order in which vertices should be joined together creating a mesh of triangles
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters")
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

	void GetGenerationData(TArray<FVector>& out_vertices, TArray<int32>& out_triangles, TArray<FLinearColor>& out_vertex_colors, TArray<FVector>& out_normals, TArray<FVector2D>& out_uv0,
		TArray<FProcMeshTangent>& out_tangents);

	

	void GenerateTerrain();

	void EndGeneration();

	bool bIsDone;

	void ResetArrays();




private:

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:


	void SetGeneratorParams(int plain_size, float terrain_scale, int seed, float scale, int octaves, float persistence, float lacunarity, float height_multiplier, UCurveFloat* height_adjustment_curve = nullptr);

	void SetGeneratorLocation(FVector spawn_location);

	//number of calculations to eprform. No need for more than 1
	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
	int NumCalculations;
	void SetNumCalculations(int num_calcs) { if (num_calcs > 0) { NumCalculations = num_calcs; } }

	//Location to spawn new Terrain
	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
	FVector SpawnLocation;

	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
		int PlainSize;

	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
	float TerrainScale;

	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
		int Seed;

	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
	float Scale;


	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
	int Octaves;


	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters", meta = (ClampMin = 0.0001, ClampMax = 0.9999))
	float Persistence;


	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters", meta = (ClampMin = 1.0, ClampMax = 10.0))
	float Lacunarity;


	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
	float HeightMultiplier;


	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
	float OffsetX;


	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
	float OffsetY;


	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
		UCurveFloat* HeightAdjustmentCurve;


	//FNoiseMap NoiseMap;

};