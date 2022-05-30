// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkGenerator.h"
#include "KismetProceduralMeshLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
//#include "PWorld.h"
//#include "Kismet/GameplayStatics.h"
#include "GenerateThread.h"

// Sets default values
AChunkGenerator::AChunkGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	this->NumCalculations = 1;
	this->bIsDone = false;
	this->PowerValue = 1;
	
}

// Called every frame
void AChunkGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AChunkGenerator::PrintCalcData()
{
}


void AChunkGenerator::InitCalculations(int32 calculations)
{
	if (calculations > 0)
	{
		this->bIsDone = false;
		GenerateThread = new AGenerateThread(calculations, this);
		CurrentRunningThread = FRunnableThread::Create(GenerateThread, TEXT("Calculation Thread"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Calcs must be greater than 0: %d"), ProcessedCalculation);
	}
}

void AChunkGenerator::GetGenerationData(TArray<FVector>& out_vertices, TArray<int32>& out_triangles, TArray<FLinearColor>& out_vertex_colors, TArray<FVector>& out_normals, TArray<FVector2D>& out_uv0, TArray<FProcMeshTangent>& out_tangents)
{
	out_vertices = this->Vertices;
	out_triangles = this->Triangles;
	out_vertex_colors = this->VertexColors;
	out_normals = this->Normals;
	out_tangents = this->Tangents;
	out_uv0 = this->UV0;

}

void AChunkGenerator::GetFoliageSpawnLocations(TArray<FVector>& water_foliage_spawn_locations, TArray<FVector>& meadow_foliage_spawn_locations, TArray<FVector>& forest_foliage_spawn_locations, TArray<FVector>& foothill_foliage_spawn_locations, TArray<FVector>& mountain_foliage_spawn_locations)
{
	water_foliage_spawn_locations = WaterFoliageSpawnVertices;
	meadow_foliage_spawn_locations = MeadowFoliageSpawnVertices;
	forest_foliage_spawn_locations = ForestFoliageSpawnVertices;
	foothill_foliage_spawn_locations = FootHillFoliageSpawnVertices;
	mountain_foliage_spawn_locations = MountainFoliageSpawnVertices;
}

void AChunkGenerator::GenerateTerrain()
{
	bIsDone = false;
	//UE_LOG(LogTemp, Warning, TEXT("Called CreateMesh"));
	this->ResetArrays();

	this->Vertices.SetNum(PlainSize * PlainSize);
	//this->VerticeBiomeMap.Reserve(PlainSize * PlainSize);
	this->UV0.SetNum(PlainSize * PlainSize);
	this->VertexColors.SetNum(PlainSize * PlainSize);
	this->Triangles.SetNum(((PlainSize -1) * (PlainSize - 1)) * 6);

	if (PWorldOwner)
	{
		if (PWorldOwner->NoiseFilters.Num() > 1)
		{
			this->GenerateTerrainLayered();
		}
		else
		{
			this->GenerateTerrainSingle();
		}
	}
	else
	{
		this->GenerateTerrainSingle();
	}
	
	
	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UV0, Normals, Tangents);
	//for (int i = 0; i < Normals.Num(); i++)
	//{
		//UE_LOG(LogTemp, Warning, TEXT("normal uproccomp: %d = %f, %f, %f"), i, Normals[i].X, Normals[i].Y, Normals[i].Z)
	//}
	this->CalculateNormals();
	//for (int i = 0; i < Normals.Num(); i++)
	//{
		//UE_LOG(LogTemp, Warning, TEXT("normal manual: %d = %f, %f, %f"), i, Normals[i].X, Normals[i].Y, Normals[i].Z)
	//}

	GenerateFoliageSpawnLocations();
}

void AChunkGenerator::GenerateTerrainLayered()
{
	int idx = 0;
	float min = 0;
	float max = 1;
	float topLeftX = (PlainSize - 1) / -2.f;
	float topLeftY = (PlainSize - 1) / 2.f;


	for (int x = 0; x < PlainSize; x++)
	{
		for (int y = 0; y < PlainSize; y++)
		{

			float perlin_value = 0;
			float biome_sort = 0.f;
			int sc = TerrainScale;
			int temp0 = (SpawnLocation.X + this->Seed) / (sc);
			int temp1 = (SpawnLocation.Y + this->Seed) / (sc);

			float sampleX = (x + temp0);
			float sampleY = (y + temp1);
			FVector2D sample(sampleX, sampleY);

			float noise_first_layer = 0.f;

			if (PWorldOwner->NoiseFilters[0].bUseDomainDistortion)
			{
				noise_first_layer = PWorldOwner->NoiseFilters[0].EvaluatePointDomainDistorted(sample);
			}
			else
			{
				noise_first_layer = PWorldOwner->NoiseFilters[0].EvaluatePoint(sample);
			}
		

			if (PWorldOwner->NoiseFilters[0].bIsEnabled)
			{
				perlin_value = noise_first_layer;
			}

			for (int i = 1; i < PWorldOwner->NoiseFilters.Num(); i++)
			{
				sample = FVector2D(sampleX, sampleY);
				float mask = 1.f;
				//check if noise filter is enabled (don't add to height if not)
				if (PWorldOwner->NoiseFilters[i].bIsEnabled)
				{
					
					if (PWorldOwner->NoiseFilters[i].bUsePreviousLayerAsMask)
					{
						if (i - 1 > -1)
						{
							if (PWorldOwner->NoiseFilters[i].bUseDomainDistortion) //Check should use domain warp
							{
								noise_first_layer = PWorldOwner->NoiseFilters[i - 1].EvaluatePointDomainDistorted(sample);
								//UE_LOG(LogTemp, Warning, TEXT("Evaluate point distored: %f"), noise_first_layer);
							}
							else
							{
								noise_first_layer = PWorldOwner->NoiseFilters[i - 1].EvaluatePoint(sample);
							}

							mask = noise_first_layer;
						}
					}
					
					if (PWorldOwner->NoiseFilters[i].bUseDomainDistortion) //Check should use domain warp
					{
						biome_sort = PWorldOwner->NoiseFilters[i].EvaluatePointDomainDistorted(sample);
						//UE_LOG(LogTemp, Warning, TEXT("Evaluate point BIOME distored: %f"), biome_sort);
					}
					else
					{
						biome_sort += PWorldOwner->NoiseFilters[i].EvaluatePoint(sample);
					}
					// total up height value
					perlin_value += biome_sort * (PWorldOwner->NoiseFilters[i].NoiseSetting.HeightMultiplier * PWorldOwner->GetTerrainScale()) 
					* mask;
					
				}
			}

			//UE_LOG(LogTemp, Warning, TEXT("BIOMEEEEEE: % f"), biome_sort);

			Vertices[idx] = FVector(x * this->TerrainScale, y * this->TerrainScale, perlin_value);
			VerticeBiomeMap.Add(Vertices[idx] + SpawnLocation, SortBiome(biome_sort)); //sort raw perlin sample output
			VertexColors[idx] = FLinearColor(0.f, 0.f, 0.f, perlin_value);
			UV0[idx] = FVector2D((x * this->TerrainScale) / (float)PlainSize, (y * this->TerrainScale) / (float)PlainSize);

			if (x < PlainSize - 1 && y < PlainSize - 1)
			{
				AddTriangle(idx, idx + PlainSize + 1, idx + PlainSize);
				AddTriangle(idx + PlainSize + 1, idx, idx + 1);
			}

			/*FVector2D(x * (TerrainScale * UVScale) / this->TerrainScale, 
				y * (TerrainScale * UVScale) / this->TerrainScale);*/
			idx++;

		}
	}
}

void AChunkGenerator::GenerateTerrainSingle()
{
	int idx = 0;
	for (int x = 0; x < PlainSize; x++)
	{
		for (int y = 0; y < PlainSize; y++)
		{
			float amplitude = 1;
			float frequency = 1;
			float noiseHeight = 0;
			float perlin_value = 0;

			int sc = TerrainScale;
			int temp0 = (SpawnLocation.X + this->Seed) / (sc);
			int temp1 = (SpawnLocation.Y + this->Seed) / (sc);

			for (int i = 0; i < this->Octaves; i++)
			{
				float sampleX = ((x + temp0) * frequency) / this->Scale;
				float sampleY = ((y + temp1) * frequency) / this->Scale;

				float perlin_generated = FMath::PerlinNoise2D(FVector2D(sampleX, sampleY));

				noiseHeight += perlin_generated * amplitude;
				amplitude *= this->Persistence;
				frequency *= this->Lacunarity;
				perlin_value = noiseHeight;
			}

			perlin_value = FMath::Pow(perlin_value, this->PowerValue);
			float generated_z(0.f);
			float height_mult_adj(1.f);

			if (this->HeightAdjustmentCurve)
			{
				height_mult_adj = this->HeightAdjustmentCurve->GetFloatValue(perlin_value) * this->HeightMultiplier;
			}
			else
			{
				generated_z = perlin_value * this->HeightMultiplier;
				// UE_LOG(LogTemp, Warning, TEXT("Height: %f"), generated_z);
			}

			Vertices[idx] = FVector((x * this->TerrainScale), (y * this->TerrainScale), generated_z);
			VertexColors[idx] = FLinearColor(0, 0, 0, perlin_value);
			UV0[idx] = FVector2D(x * (TerrainScale * UVScale) / this->TerrainScale, y * (TerrainScale * UVScale) / this->TerrainScale);
			idx++;
		}
	}
}

void AChunkGenerator::GenerateFoliageSpawnLocations()
{
	//int FoliageSpawnAdjuster = 32;
	//int NumberVerticesToGenerateAround = this->Vertices.Num() / FoliageSpawnAdjuster;
	int water_idx(0), meadow_idx(0), forest_idx(0), foothill_idx(0), mountain_idx(0);
	int water_idx_t(128), meadow_idx_t(65), forest_idx_t(40), foothill_idx_t(90), mountain_idx_t(118);
	/*for (int i = 0; i < VerticeBiomeMap.Num(); i++)
	{
		VerticeBiomeMap.
	}*/
	for (auto& vertice : VerticeBiomeMap)
	{
		
			
			switch (vertice.Value)
			{
			case EBiome::Biome_Water:
				if (water_idx < water_idx_t)
				{
					water_idx++;
				}
				else
				{
					LocateBasedOnBiome(this->WaterFoliageSpawnVertices, vertice.Key);
					water_idx = 0;
				}
				
				break;
			case EBiome::Biome_Meadow:
				if (meadow_idx < meadow_idx_t)
				{
					meadow_idx++;
				}
				else
				{
					LocateBasedOnBiome(this->WaterFoliageSpawnVertices, vertice.Key);
					meadow_idx = 0;
				}
				break;
			case EBiome::Biome_Forest:
				if (forest_idx < forest_idx_t)
				{
					forest_idx++;
				}
				else
				{
					LocateBasedOnBiome(this->WaterFoliageSpawnVertices, vertice.Key);
					forest_idx = 0;
				}
				break;
			case EBiome::Biome_FootHills:
				if (foothill_idx < foothill_idx_t)
				{
					foothill_idx++;
				}
				else
				{
					LocateBasedOnBiome(this->WaterFoliageSpawnVertices, vertice.Key);
					foothill_idx = 0;
				};
				break;
			case EBiome::Biome_Mountains:
				if (mountain_idx < mountain_idx_t)
				{
					mountain_idx++;
				}
				else
				{
					LocateBasedOnBiome(this->WaterFoliageSpawnVertices, vertice.Key);
					mountain_idx = 0;
				}
				break;
			case EBiome::Biome_Default:

				break;
			default:

				break;
			}
	
	}
}

void AChunkGenerator::LocateBasedOnBiome(TArray<FVector>& biome_foliage_spawn_array, FVector vertice_point)
{
	//UE_LOG(LogTemp, Warning, TEXT("LOC location: (%f, %f, %f) "), vertice_point.X, vertice_point.Y, vertice_point.Z);

	
	float base_probe_length = 25.f;
	float scale = FMath::FRandRange(1.5f, 4.f);
	float x_adj = FMath::FRandRange(-range, range);
	float y_adj = FMath::FRandRange(-range, range);
	if (x_adj < bounds && x_adj > 0) { x_adj = bounds; }
	else if (x_adj > -bounds && x_adj <= 0) { x_adj = -bounds; }
	if (y_adj < bounds && y_adj > 0) { y_adj = bounds; }
	else if (y_adj > -bounds && y_adj <= 0) { y_adj = -bounds; }

	FVector end_loc((vertice_point.X + (base_probe_length * x_adj)), (vertice_point.Y + (base_probe_length * y_adj)), vertice_point.Z);
	//DrawDebugSphere(GetWorld(), end_loc, 16.f, 4, FColor::Red, false, 8.f);
	//UE_LOG(LogTemp, Warning, TEXT("end location: (%f, %f, %f) "), end_loc.X, end_loc.Y, end_loc.Z);
	biome_foliage_spawn_array.Add(end_loc);

}

void AChunkGenerator::EndGeneration()
{
	this->bIsDone = true;
	//UE_LOG(LogTemp, Warning, TEXT("Ended generation chunk"));
}

void AChunkGenerator::ResetArrays()
{
	//vertices matrix (x,y,z)
	Vertices.Empty();

	//order in which vertices should be joined together creating a mesh of triangles
	Triangles.Empty();
	VertexColors.Empty();
	Normals.Empty();
	UV0.Empty();
	Tangents.Empty();

	WaterFoliageSpawnVertices.Empty();
	MeadowFoliageSpawnVertices.Empty();
	ForestFoliageSpawnVertices.Empty();
	FootHillFoliageSpawnVertices.Empty();
	MountainFoliageSpawnVertices.Empty();

	this->VerticeBiomeMap.Empty();
	TriangleIdx = 0;
}

EBiome AChunkGenerator::SortBiome(float in_val)
{
	
		if (in_val < 0.009 && in_val >= 0.007)
		{
			return EBiome::Biome_Water;
		}
		else if (in_val >= 0.02 && in_val < 0.04)
		{
			return EBiome::Biome_Meadow;
		}
		else if (in_val >= 0.04 && in_val < 0.7)
		{
			return EBiome::Biome_Forest;
		}
		else if (in_val >= 0.7 && in_val < 0.10f)
		{
			return EBiome::Biome_FootHills;
		}
		else if (in_val >= 0.12 && in_val < 1.f)
		{
			return EBiome::Biome_Mountains;
		}
		else
		{
			return EBiome::Biome_Default;
			UE_LOG(LogTemp, Warning, TEXT("FNoiseFilter::EvaluatedPointBiome()__ Biome set to Default because evluated point was not between 0 and 1 "))
		}
	
}

void AChunkGenerator::CalculateNormals()
{
	this->Normals.Empty();
	this->Normals.SetNum(this->Vertices.Num());

	UE_LOG(LogTemp, Warning, TEXT("vertice coount: %d"), Vertices.Num());
	int tri_count = this->Triangles.Num() / 3;
	UE_LOG(LogTemp, Warning, TEXT("tri count: %d"), tri_count);

	for (int i = 0; i < tri_count; i++)
	{
		int norm_tri_idx = i * 3;
		int vert_idx_a = this->Triangles[norm_tri_idx];
		int vert_idx_b = this->Triangles[norm_tri_idx + 1];
		int vert_idx_c = this->Triangles[norm_tri_idx + 2];

		FVector tri_norm = SurfaceNormalFromPoint(vert_idx_a, vert_idx_b, vert_idx_c);
		Normals[vert_idx_a] = tri_norm;
		Normals[vert_idx_b] = tri_norm;
		Normals[vert_idx_c] = tri_norm;
	}
	
	for (int i = 0; i < Normals.Num(); i++)
	{
		Normals[i].Normalize();
	}

}

FVector AChunkGenerator::SurfaceNormalFromPoint(int point_a, int point_b, int point_c)
{
	FVector pointA = Vertices[point_a];
	FVector pointB = Vertices[point_b];
	FVector pointC = Vertices[point_c];

	const FVector sideAB = pointC - pointA ;
	const FVector sideAC = pointB - pointA ;
	FVector norm = (sideAB ^ sideAC).GetSafeNormal();
	//UE_LOG(LogTemp, Warning, TEXT("normal manual _ pre normalize: %f, %f, %f"), norm.X, norm.Y, norm.Z)
	//norm.Normalize();
	//UE_LOG(LogTemp, Error, TEXT("normal manual _ normalized: %f, %f, %f"), norm.X, norm.Y, norm.Z)
	return norm;
}

void AChunkGenerator::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (CurrentRunningThread && GenerateThread)
	{
		CurrentRunningThread->Suspend(true);
		GenerateThread->bStopThread = true;
		CurrentRunningThread->Suspend(false);
		CurrentRunningThread->Kill(false);
		CurrentRunningThread->WaitForCompletion();
		delete GenerateThread;
	}
}

void AChunkGenerator::AddTriangle(int vert_a, int vert_b, int vert_c)
{
	this->Triangles[TriangleIdx] = vert_a;
	this->Triangles[TriangleIdx + 1] = vert_b;
	this->Triangles[TriangleIdx + 2] = vert_c;
	TriangleIdx += 3;

}


void AChunkGenerator::SetGeneratorParams(int uv_scale, int plain_size, float terrain_scale, int seed, float scale, float power_value, int octaves, float persistence, float lacunarity, float height_multiplier, UCurveFloat* height_adjustment_curve )
{
	this->PlainSize = plain_size;
	this->TerrainScale = terrain_scale;
	this->Seed = seed;
	this->Scale = scale;
	this->PowerValue = power_value;
	this->Octaves = octaves;
	this->Persistence = persistence;
	this->Lacunarity = lacunarity;
	this->HeightMultiplier = height_multiplier;
	this->HeightAdjustmentCurve = height_adjustment_curve;
	this->UVScale = uv_scale;


	AltOctaves = this->Octaves;
}

void AChunkGenerator::SetGeneratorLocation(FVector spawn_location)
{
	this->SpawnLocation = spawn_location;
}



////////////////Old Stuff
/*/*int32 iterations = (PlainSize) * (PlainSize - 1);
	//Triangles.SetNum(iterations * 6);
	int tris = 0;
	for (int i = 0; i < iterations; i++)
	{
		if ((i + 1) % PlainSize != 0) {
			/*Triangles[tris] = i;
			Triangles[tris + 1] = i + PlainSize + 1;
			Triangles[tris + 2] = i + PlainSize;
			Triangles[tris + 3] = i;
			Triangles[tris + 4] = i + 1;
			Triangles[tris + 5] = i + PlainSize + 1;
			tris += 6;
	Triangles.Add(i);
	Triangles.Add(i + PlainSize + 1);
	Triangles.Add(i + PlainSize);
	Triangles.Add(i + PlainSize + 1);
	Triangles.Add(i);
	Triangles.Add(i + 1);

}
	}*/
	/*for (int i = 0; i < Vertices.Num(); i++)
	{
		FVector one = Vertices[i+1] - Vertices[i];
		FVector two = Vertices[i + PlainSize] - Vertices[i];

		FVector norm = UKismetMathLibrary::Cross_VectorVector(one, two);
		norm.Normalize();
		Normals.Add(norm);
	}*/