// Fill out your copyright notice in the Description page of Project Settings.


#include "PTerrain.h"
#include "KismetProceduralMeshLibrary.h"

// Sets default values
APTerrain::APTerrain()
{
	mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	RootComponent = mesh;
	mesh->bUseAsyncCooking = true;
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PlainSize = 10;
	PreviousPlainSize = PlainSize;
	//HeightMultiplier = 500.0f;

	OffsetX = 0.0f;
	OffsetY = 0.0f;

	this->Scale = 20.f;
	this->Lacunarity = 1.2f;
	this->Persistence = 0.5f;
	this->Octaves = 3;
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


}

// Called when the game starts or when spawned
void APTerrain::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APTerrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APTerrain::OnConstruction(const FTransform& Transform)
{

	Super::OnConstruction(Transform);

	if (PreviousPlainSize != this->PlainSize)
	{
		//Need to resize mesh vertices array
		this->PreviousPlainSize = this->PlainSize;
		this->GenerateNoise();
	}
	else
	{
		this->GenerateNoise(true);
	}

}

void APTerrain::CreateMesh()
{
	this->NoiseMap = FNoiseMap(this->PlainWidth, this->PlainHeight);
	//FRandomStream random = FRandomStream(this->Seed);
	

	/*	for (size_t y = 0; y < this->NoiseMap.Y.Num(); y++)
	{
		for (size_t x = 0; x < this->NoiseMap.X.Num(); x++)
		{
			float amplitude = 1;
			float frequency = 1;
			float noiseHeight = 0;
			float perlin_value = 0;

			for (size_t i = 0; i < this->Octaves; i++)
			{
				float sampleX = x / this->Scale * frequency;
				float sampleY = y / this->Scale * frequency;
				float perlin_generated = FMath::PerlinNoise2D(FVector2D(sampleX, sampleY));
				//UE_LOG(LogTemp, Warning, TEXT("Perlin Value not adjusted for heigt= %f"), perlin_generated);
				noiseHeight += perlin_generated * amplitude;

				amplitude *= this->Persistence;
				frequency *= this->Lacunarity;
				perlin_value = perlin_generated;
			}

			this->NoiseMap.X[x] = noiseHeight;
			this->NoiseMap.Y[y] = noiseHeight;

		}
	}*/

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
				//UE_LOG(LogTemp, Warning, TEXT("__Z__"));
				//UE_LOG(LogTemp, Warning, TEXT("Z = %f"), perlin_generated);
				//UE_LOG(LogTemp, Warning, TEXT("Perlin Value not adjusted for heigt= %f"), perlin_generated);
				noiseHeight += perlin_generated * amplitude;

				amplitude *= this->Persistence;
				frequency *= this->Lacunarity;
				perlin_value = perlin_generated;
			}

			float height_mult_adj(1.f); 
			/*if (perlin_value <0.01 && perlin_value > -0.15)
			{
				//perlin_value = -0.08;
			}*/
			if (this->HeightAdjustmentCurve)
			{
				height_mult_adj = this->HeightAdjustmentCurve->GetFloatValue(perlin_value) * this->HeightMultiplier;
			}
			
			//UE_LOG(LogTemp, Warning, TEXT("Perlin Value = %f"), perlin_value);
			float generated_z = perlin_value * height_mult_adj;
			/*UE_LOG(LogTemp, Warning, TEXT("Generated z = %f"), generated_z * 500.0f);*/
			vertices.Add(FVector(topLeftW + (x * 1000), topLeftH + (y * 1000), generated_z * 5));
			//vertices.Add(FVector(x*100, y*100, FMath::RandRange(0.0f, 100.0f)));
			vertexColors.Add(FLinearColor(0, 0, 0, perlin_value));
			float width = this->PlainSize;
			UV0.Add(FVector2D(x / width, y / width));
			//UE_LOG(LogTemp, Warning, TEXT("U = %f"), x/width);
			//UE_LOG(LogTemp, Warning, TEXT("V = %f"), y/width);


		}
	}

	int32 iterations = (PlainSize - 1) * (PlainSize);
	//Triangles.SetNum(iterations);
	for (int i = 0; i < iterations; i++)
	{
		if ((i + 1) % PlainSize != 0) {
			Triangles.Add(i);
			Triangles.Add(i + 1);
			Triangles.Add(i + PlainSize);

			Triangles.Add(i + 1);
			Triangles.Add(i + PlainSize + 1);
			Triangles.Add(i + PlainSize);
			
			/*Triangles[i] = i;
			Triangles[i + 1] = i + 1;
			Triangles[i + 2] = i + PlainSize;

			Triangles[i + 3] = i + 1;
			Triangles[i + 4] = i + PlainSize + 1;
			Triangles[i + 5] = i + PlainSize;*/
		}
	}

	normals.Add(FVector(1, 0, 0));
	normals.Add(FVector(1, 0, 0));
	normals.Add(FVector(1, 0, 0));

	tangents.Add(FProcMeshTangent(0, 1, 0));
	tangents.Add(FProcMeshTangent(0, 1, 0));
	tangents.Add(FProcMeshTangent(0, 1, 0));

	mesh->CreateMeshSection_LinearColor(0, vertices, Triangles, normals, UV0, vertexColors, tangents, true);
	//	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(vertices, Triangles, UV0, normals, tangents);

		// Enable collision data
	mesh->ContainsPhysicsTriMeshData(true);
}

void APTerrain::UpdateMesh()
{
}

void APTerrain::PostActorCreated()
{
	Super::PostActorCreated();
	//this->GenerateNoise();
}

void APTerrain::PostLoad()
{
	Super::PostLoad();
	//this->GenerateNoise();
}

void APTerrain::ResetArrays()
{

	//vertices matrix (x,y,z)
	vertices.Empty();

	//order in which vertices should be joined together creating a mesh of triangles
	Triangles.Empty();
	vertexColors.Empty();
	normals.Empty();
	UV0.Empty();
	tangents.Empty();
}

void APTerrain::GenerateNoise(bool bShouldClearCurrent)
{

	if (bShouldClearCurrent)
	{
		this->ClearMesh();
		this->CreateMesh();

	}
	else
	{
		this->ClearMesh();
		this->CreateMesh();

	}

}

void APTerrain::ClearMesh()
{

		mesh->ClearAllMeshSections();
		this->ResetArrays();
	

}

