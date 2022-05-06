// Fill out your copyright notice in the Description page of Project Settings.


#include "PTerrain.h"
#include "Kismet/KismetMathLibrary.h"
#include "KismetProceduralMeshLibrary.h"
#include "Materials/Material.h"

// Sets default values
APTerrain::APTerrain()
{
	mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	meshother_test = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh_test"));
	RootComponent = mesh;
	mesh->bUseAsyncCooking = true;
	meshother_test->bUseAsyncCooking = true;
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PlainSize = 10;
	
	//HeightMultiplier = 500.0f;

	OffsetX = 0.0f;
	OffsetY = 0.0f;

	this->Scale = 20.f;
	this->Lacunarity = 1.2f;
	this->Persistence = 0.5f;
	this->Octaves = 3;
	this->TerrainScale = 10.f;
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


}

// Called when the game starts or when spawned
void APTerrain::BeginPlay()
{
	Super::BeginPlay();


	FTransform actor_trans;

	this->TerrainWorldLocation = UKismetMathLibrary::TransformLocation(actor_trans, GetActorLocation());
	this->TerrainWorldLocation = GetActorLocation();
/*	UE_LOG(LogTemp, Warning, TEXT("x: %f"), TerrainWorldLocation.X);
	UE_LOG(LogTemp, Warning, TEXT("y: %f"), TerrainWorldLocation.Y);
	UE_LOG(LogTemp, Warning, TEXT("z: %f"), TerrainWorldLocation.Z);*/
}

// Called every frame
void APTerrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APTerrain::OnConstruction(const FTransform& Transform)
{

	Super::OnConstruction(Transform);

	//this->TerrainWorldLocation = GetActorLocation();
/*	UE_LOG(LogTemp, Warning, TEXT("x: %f"), TerrainWorldLocation.X);
	UE_LOG(LogTemp, Warning, TEXT("y: %f"), TerrainWorldLocation.Y);
	UE_LOG(LogTemp, Warning, TEXT("z: %f"), TerrainWorldLocation.Z);*/
	//this->GenerateNoise();

}

void APTerrain::CreateMesh()
{
	this->CreateMesh(this->mesh);

	//this->CreateMeshOld(this->meshother_test);

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
	Vertices.Empty();

	//order in which vertices should be joined together creating a mesh of triangles
	Triangles.Empty();
	VertexColors.Empty();
	Normals.Empty();
	UV0.Empty();
	Tangents.Empty();
}

void APTerrain::SetMeshMaterial(UMaterial* material)
{
	this->mesh->SetMaterial(0, material);
}

void APTerrain::CreateMesh(float x_pos, float y_pos)
{
}

void APTerrain::GenerateNoise(bool bShouldClearCurrent)
{

	if (bShouldClearCurrent)
	{
		this->ClearMesh();
		this->CreateMesh();
		return;

	}
	else
	{
		this->ClearMesh();
		this->CreateMesh();
		return;

	}

}

void APTerrain::ClearMesh()
{

		mesh->ClearAllMeshSections();
		this->ResetArrays();
	

}

void APTerrain::CreateMesh(UProceduralMeshComponent* meshcomp)
{
	//this->SetTerrainScale();
	//int plane_size = (PlainSize / this->Scale) * 100;
	//UE_LOG(LogTemp, Warning, TEXT("Called CreateMesh"));

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

			int temp0 = TerrainWorldLocation.X / (sc);
			int temp1 = TerrainWorldLocation.Y / (sc);

			FVector some = TerrainWorldLocation / sc;

			for (size_t i = 0; i < this->Octaves; i++)
			{

				float sampleX = (x + temp0) / this->Scale * frequency;
			//	UE_LOG(LogTemp, Warning, TEXT("sample x: %d"), temp0);

				float sampleY = (y + temp1) / this->Scale * frequency;
				//UE_LOG(LogTemp, Warning, TEXT("sample y: %d"), (temp1));
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

	//UKismetProceduralMeshLibrary::CalculateTangentsForMesh(vertices, Triangles, UV0, normals, tangents);
	meshcomp->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UV0, VertexColors, Tangents, true);
	meshcomp->ContainsPhysicsTriMeshData(true);
}


void APTerrain::GenerateMeshFromWorld(TArray<FVector> vertices, TArray<int32>triangles, TArray<FLinearColor>vertex_colors, TArray<FVector>normals, TArray<FVector2D>uv0, TArray<FProcMeshTangent>tangents)
{
	this->mesh->CreateMeshSection_LinearColor(0, vertices, triangles, normals, uv0, vertex_colors, tangents, true);
		this->mesh->ContainsPhysicsTriMeshData(true);
}


void APTerrain::SetTerrainScale()
{
	

	int plain = PlainSize * 32;
	switch (TerrianFidelity)
	{
	case ETerrainScale::TS_Low:
		this->TerrainScale = plain / 2.f;
		SizePlane = plain/ this->TerrainScale;
		break;
	case ETerrainScale::TS_Medium:
		this->TerrainScale = plain / 4.f;
		SizePlane= plain / this->TerrainScale;
		break;
	case ETerrainScale::TS_High:
		this->TerrainScale = plain / 8.f;
		SizePlane = plain / this->TerrainScale;
		break;
	case ETerrainScale::TS_UltraHigh:
		this->TerrainScale = plain / 16.f;
		SizePlane = plain / this->TerrainScale;
		break;
	case ETerrainScale::TS_Default:
		this->TerrainScale = 500;
		break;
	default:
		break;
	}
}

