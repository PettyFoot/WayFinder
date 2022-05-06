// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkGenerator.h"
#include "Kismet/KismetMathLibrary.h"
//#include "Kismet/GameplayStatics.h"
#include "GenerateThread.h"

// Sets default values
AChunkGenerator::AChunkGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	this->NumCalculations = 1;
	this->bIsDone = false;
	
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

}

void AChunkGenerator::GenerateTerrain()
{
	bIsDone = false;
	//UE_LOG(LogTemp, Warning, TEXT("Called CreateMesh"));
	this->ResetArrays();

	for (size_t x = 0; x < PlainSize; x++)
	{
		for (size_t y = 0; y < PlainSize; y++)
		{
			float topLeftW = (PlainSize - 1) / -2.f;
			float topLeftH = (PlainSize - 1) / 2.f;

			float amplitude = 1;
			float frequency = 1;
			float noiseHeight = 0;
			float perlin_value = 0;
			int sc = TerrainScale;

			int temp0 = SpawnLocation.X / (sc);
			int temp1 = SpawnLocation.Y / (sc);

			for (size_t i = 0; i < this->Octaves; i++)
			{

				float sampleX = (x + temp0) / this->Scale * frequency;
				//UE_LOG(LogTemp, Warning, TEXT("sample x: %d"), temp0);

				float sampleY = (y + temp1) / this->Scale * frequency;
				//UE_LOG(LogTemp, Warning, TEXT("sample y: %d"), (temp1));
				float perlin_generated = FMath::PerlinNoise2D(FVector2D(sampleX, sampleY));
				noiseHeight += perlin_generated * amplitude;

				amplitude *= this->Persistence;
				frequency *= this->Lacunarity;
				perlin_value = perlin_generated;
			}

			float generated_z(0.f);
			float height_mult_adj(1.f);
			if (this->HeightAdjustmentCurve)
			{
				height_mult_adj = this->HeightAdjustmentCurve->GetFloatValue(perlin_value) * this->HeightMultiplier;
			}
			else
			{
				 generated_z = perlin_value * this->HeightMultiplier;
				 UE_LOG(LogTemp, Warning, TEXT("Height: %f"), this->HeightMultiplier);
			}

			



			Vertices.Add(FVector((x * this->TerrainScale), (y * this->TerrainScale), generated_z));
			VertexColors.Add(FLinearColor(0, 0, 0, perlin_value));
			UV0.Add(FVector2D((x * (TerrainScale * 4)) / this->PlainSize, (y * (TerrainScale * 4)) / this->PlainSize));
			/*UE_LOG(LogTemp, Warning, TEXT("sample x: %d"), x);
			UE_LOG(LogTemp, Warning, TEXT("sample y: %d"), y);*/
		}
	}


	int32 iterations = (PlainSize) * (PlainSize - 1);
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
	for (int i = 0; i < Triangles.Num() / 6; i++)
	{
		FVector one = Vertices[Triangles[i + 1]] - Vertices[Triangles[i]];
		FVector two = Vertices[Triangles[i + 2]] - Vertices[Triangles[i]];

		FVector norm = UKismetMathLibrary::Cross_VectorVector(one, two);
		norm.Normalize();
		Normals.Add(norm);
	}

	Tangents.Add(FProcMeshTangent(0, 1, 0));
	Tangents.Add(FProcMeshTangent(0, 1, 0));
	Tangents.Add(FProcMeshTangent(0, 1, 0));

	//UE_LOG(LogTemp, Warning, TEXT("Size_Thread: %d"), Vertices.Num());
	//UE_LOG(LogTemp, Warning, TEXT("Size_Thread: %d"), Triangles.Num());
	//UE_LOG(LogTemp, Warning, TEXT("Size_Thread: %d"), Normals.Num());
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

void AChunkGenerator::SetGeneratorParams( int plain_size, float terrain_scale, int seed, float scale, int octaves, float persistence, float lacunarity, float height_multiplier, UCurveFloat* height_adjustment_curve)
{
	this->PlainSize = plain_size;
	this->TerrainScale = terrain_scale;
	this->Seed = seed;
	this->Scale = scale;
	this->Octaves = octaves;
	this->Persistence = persistence;
	this->Lacunarity = lacunarity;
	this->HeightMultiplier = height_multiplier;
	this->HeightAdjustmentCurve = height_adjustment_curve;
}

void AChunkGenerator::SetGeneratorLocation(FVector spawn_location)
{
	this->SpawnLocation = spawn_location;
}


