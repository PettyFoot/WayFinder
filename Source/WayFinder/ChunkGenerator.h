// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "PWorld.h"
#include "ChunkGenerator.generated.h"

class AGenerateThread;
class APWorld;

struct Array
{
	//
	TArray<int32> Columns;
	int32 ColumnIdx = 0;

	Array() {}
	Array(int size) { if (size > 0) { Columns.SetNum(size); ColumnIdx = size - 1; } }

};

struct TArrayTwo
{
	//Array of Array structs
	TArray<Array> Rows;
	int32 RowIdx = 0;

	int GetElement(int row_idx, int col_idx)
	{
		if (row_idx >= 0 && row_idx <= RowIdx)
		{
			if (col_idx >= 0 && col_idx <= Rows[row_idx].ColumnIdx)
			{
				return Rows[row_idx].Columns[col_idx];
			}
		}
		
		return -10000000; //something large that I know would never be a value returned by and indice
		
	}

	//@param int - num rows to hold
	//@param bool - if true set columns to same size as rows
	void SetRows(int rows, bool bIsSquare)
	{
		//keep size to 1 and up
		if (rows > 0)
		{
			this->Rows.SetNum(rows);
			this->RowIdx = rows -1; //(0 based mf)
			
			if (bIsSquare) //Check should make column same size
			{
				for (auto& row : Rows)
				{
					row.Columns.SetNum(rows);
					row.ColumnIdx = rows - 1; //(0 based mf)
				}
			}
		}
	}

	//@param int - value to add to 2d array
	void AddElement(int val, int row_idx = -1, int col_idx = -1)
	{
		if (row_idx > -1 && col_idx > -1)
		{
			if (row_idx < this->Rows.Num())
			{
				if (col_idx < this->Rows[row_idx].Columns.Num())
				{
					this->Rows[row_idx].Columns[col_idx] = val;
				}
			}
		}
		else
		{
			if (Rows.Num() > 0)
			{
				this->Rows.Add(Array(Rows[0]));
			}
			else
			{
				this->Rows.Add(Array());
			}

			this->RowIdx++; //Increment 
			this->Rows[RowIdx].Columns.Add(val); //Get new column and add val to it
			this->Rows[RowIdx].ColumnIdx++;
			//Could add a default thing to do when adding new item without idx (i.e init the array to num 5)

			UE_LOG(LogTemp, Warning, TEXT("CHUNKGENERATOR::(struct)TArrayTwo::AddElement__ Added element to next avail idx Row: %d, Col: %d"), 
				RowIdx +1, Rows[row_idx].ColumnIdx + 1);
		}

	}

	//This doesn't delete the arrays but just clears them and resets row & column idxs
	void Empty()
	{

		if (Rows.Num() > 0) //check if data to remove
		{
			for (auto& row : Rows) 
			{
				if (row.Columns.Num() > 0) //check if data to remove
				{
					row.Columns.Empty(); //Clear cols
					row.ColumnIdx = 0;
				}
				
			}
			Rows.Empty(); //Clear rows
			this->RowIdx = 0;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("CHUNKGENERATOR::(struct)TArrayTwo::Empty__ no data to remove"));
		}
	}

	void PrintData()
	{
		int row_it = 0;
		int col_it = 0;
		
			for (auto& row : Rows)
			{

				for(auto& column: row.Columns)
				{

					UE_LOG(LogTemp, Warning, TEXT("( %d , %d ) = %d"), row_it, col_it, column);
					col_it++;
				}
				row_it++;
				col_it = 0;
			}
			
	}

};



UCLASS()
class WAYFINDER_API AChunkGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChunkGenerator();

public:	

	float range;
	float bounds;

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

	//Map to help sort out vertices biome, which in turn will help generate foliage locations
	TMap<FVector, EBiome> VerticeBiomeMap;

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

	void GetFoliageSpawnLocations(TArray<FVector>& water_foliage_spawn_locations, TArray<FVector>& meadow_foliage_spawn_locations, TArray<FVector>& forest_foliage_spawn_locations,
		TArray<FVector>& foothill_foliage_spawn_locations, TArray<FVector>& mountain_foliage_spawn_locations);


	APWorld* PWorldOwner;
	

	void GenerateTerrain();

	void EndGeneration();

	void CalculateNormals();

	FVector SurfaceNormalFromPoint(int point_a, int point_b, int point_c);

	void AddTriangle(int vert_a, int vert_b, int vert_c);

	bool bIsDone;

	void ResetArrays();

	EBiome SortBiome(float in_val);

	//void GenerateFoliageBasedOnBiome()

private:

	int TriangleIdx;

private:

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void GenerateTerrainLayered();

	void GenerateTerrainSingle();

	void GenerateFoliageSpawnLocations();

	void LocateBasedOnBiome(TArray<FVector>& biome_foliage_spawn_array, FVector vertice_point);



public:

	float rand_num;

	void SetGeneratorParams(int uv_scale, int plain_size, float terrain_scale, int seed, float scale, float power_value, int octaves, float persistence, float lacunarity, float height_multiplier, UCurveFloat* height_adjustment_curve = nullptr);

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
	float PowerValue;


	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
	int Octaves;

	int AltOctaves;


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
		int UVScale;


	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
		UCurveFloat* HeightAdjustmentCurve;


	//FNoiseMap NoiseMap;

};
