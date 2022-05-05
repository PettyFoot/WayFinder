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
	out_vertices = this->vertices;
	out_triangles = this->Triangles;
	out_vertex_colors = this->vertexColors;
	out_normals = this->normals;
	out_tangents = this->tangents;
}

void AChunkGenerator::GenerateTerrain()
{
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

			for (size_t i = 0; i < this->Octaves; i++)
			{
				float sampleX = x / this->Scale * frequency;
				float sampleY = y / this->Scale * frequency;
				float perlin_generated = FMath::PerlinNoise2D(FVector2D(sampleX, sampleY));
				noiseHeight += perlin_generated * amplitude;

				amplitude *= this->Persistence;
				frequency *= this->Lacunarity;
				perlin_value = perlin_generated;
			}

			float height_mult_adj(1.f);
			if (this->HeightAdjustmentCurve)
			{
				height_mult_adj = this->HeightAdjustmentCurve->GetFloatValue(perlin_value) * this->HeightMultiplier;
			}

			float generated_z = perlin_value * height_mult_adj;

			vertices.Add(FVector((x * 10), (y * 10), generated_z));
			vertexColors.Add(FLinearColor(0, 0, 0, perlin_value));
			UV0.Add(FVector2D((x) / this->PlainSize, (y) / this->PlainSize));

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
		FVector one = this->vertices[this->Triangles[i + 1]] - vertices[Triangles[i]];
		FVector two = vertices[Triangles[i + 2]] - vertices[Triangles[i]];

		FVector norm = UKismetMathLibrary::Cross_VectorVector(one, two);
		norm.Normalize();
		normals.Add(norm);
	}

	tangents.Add(FProcMeshTangent(0, 1, 0));
	tangents.Add(FProcMeshTangent(0, 1, 0));
	tangents.Add(FProcMeshTangent(0, 1, 0));
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

