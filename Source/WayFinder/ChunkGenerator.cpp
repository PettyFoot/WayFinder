// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkGenerator.h"
#include "KismetProceduralMeshLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "PWorld.h"
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

void AChunkGenerator::GenerateTerrain()
{
	bIsDone = false;
	//UE_LOG(LogTemp, Warning, TEXT("Called CreateMesh"));
	this->ResetArrays();


	int idx = 0;
	this->Vertices.SetNum(PlainSize * PlainSize);
	this->UV0.SetNum(PlainSize * PlainSize);
	this->VertexColors.SetNum(PlainSize * PlainSize);

	
	if (PWorldOwner)
	{
		if (PWorldOwner->NoiseFilters.Num() > 1)
		{
			for (int x = 0; x < PlainSize; x++)
			{
				for (int y = 0; y < PlainSize; y++)
				{
					
					float perlin_value = 0;
					int sc = TerrainScale;
					int temp0 = (SpawnLocation.X + this->Seed) / (sc);
					int temp1 = (SpawnLocation.Y + this->Seed) / (sc);

					float sampleX = (x + temp0);
					//UE_LOG(LogTemp, Warning, TEXT("sample x: %d"), temp0);
					float sampleY = (y + temp1);
					FVector2D sample(sampleX, sampleY);

					float noise_first_layer = PWorldOwner->NoiseFilters[0].EvaluatePoint(sample);
					if (PWorldOwner->NoiseFilters[0].bIsEnabled)
					{
						perlin_value = noise_first_layer;
					}
				

					for (int i = 1; i < PWorldOwner->NoiseFilters.Num(); i++)
					{
						sample = FVector2D(sampleX, sampleY);

						if (PWorldOwner->NoiseFilters[i].bIsEnabled)
						{
							float mask = 1.f;
							if (PWorldOwner->NoiseFilters[i].bUseFirstLayerAsMask)
							{
								if (i - 1 > -1)
								{
									noise_first_layer = PWorldOwner->NoiseFilters[i - 1].EvaluatePoint(sample);
									mask = noise_first_layer;
								}
							}
							perlin_value += PWorldOwner->NoiseFilters[i].EvaluatePoint(sample) * mask;
							//UE_LOG(LogTemp, Warning, TEXT("perlin chunkgenerator: %f"), perlin_value);
						}
						

					}
					/*float amplitude = 1;
						float frequency = 1;
						float noiseHeight = 0;
						//float perlin_value = 0;
						

						for (int i = 0; i < this->Octaves; i++)
						{

							float sampleX = ((x + temp0) * frequency) / this->Scale;
							//UE_LOG(LogTemp, Warning, TEXT("sample x: %d"), temp0);
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
						}*/

					Vertices[idx] = FVector((x * this->TerrainScale), (y * this->TerrainScale), perlin_value);
					VertexColors[idx] = FLinearColor(0, 0, 0, perlin_value);
					UV0[idx] = FVector2D(x * (TerrainScale * UVScale) / this->TerrainScale, y * (TerrainScale * UVScale) / this->TerrainScale);
					idx++;
					//Vertices.Add(FVector((x * this->TerrainScale), (y * this->TerrainScale), generated_z));
					//VertexColors.Add(FLinearColor(0, 0, 0, perlin_value));
					//UV0.Add(FVector2D(x * (TerrainScale * UVScale) / this->TerrainScale, y * (TerrainScale * UVScale) / this->TerrainScale));
				}
			}
		}
		else
		{
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
						//UE_LOG(LogTemp, Warning, TEXT("sample x: %d"), temp0);
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
					//Vertices.Add(FVector((x * this->TerrainScale), (y * this->TerrainScale), generated_z));
					//VertexColors.Add(FLinearColor(0, 0, 0, perlin_value));
					//UV0.Add(FVector2D(x * (TerrainScale * UVScale) / this->TerrainScale, y * (TerrainScale * UVScale) / this->TerrainScale));
				}
			}

		}
	}
	else
	{
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
					//UE_LOG(LogTemp, Warning, TEXT("sample x: %d"), temp0);
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
	
	int32 iterations = (PlainSize) * (PlainSize - 1);

/*	for (int i = 0; i < this->Vertices.Num(); i++)
	{
		Triangles.Add(i);
		Triangles.Add(i + PlainSize + 1);
		Triangles.Add(i + PlainSize);

		//Triangles.Add(i);
		//Triangles.Add(i + 1);
		//Triangles.Add(i + PlainSize + 1);

	}*/

	for (int i = 0; i < iterations; i++)
	{
		if ((i + 1) % PlainSize != 0) {
			Triangles.Add(i);
			Triangles.Add(i + PlainSize + 1);
			Triangles.Add(i + PlainSize);

			Triangles.Add(i);
			Triangles.Add(i + 1);
			Triangles.Add(i + PlainSize + 1);
		}
	}
	Normals.SetNum(this->Vertices.Num());
	/*for (int i = 0; i < Vertices.Num(); i++)
	{
		FVector one = Vertices[i+1] - Vertices[i];
		FVector two = Vertices[i + PlainSize] - Vertices[i];

		FVector norm = UKismetMathLibrary::Cross_VectorVector(one, two);
		norm.Normalize();
		Normals.Add(norm);
	}*/

	Tangents.Add(FProcMeshTangent(0, 1, 0));
	Tangents.Add(FProcMeshTangent(0, 1, 0));
	Tangents.Add(FProcMeshTangent(0, 1, 0));

	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UV0, Normals, Tangents);

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


