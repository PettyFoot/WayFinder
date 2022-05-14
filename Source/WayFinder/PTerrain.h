// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "GameFramework/Actor.h"
#include "PTerrain.generated.h"

class UProceduralMeshComponent;
class UCurveFloat;
class UMaterial;
class UBoxComponent;
class APWorld;

UENUM(BlueprintType)
enum class ETerrainScale : uint8
{
	TS_Low,
	TS_Medium,
	TS_High,
	TS_UltraHigh,
	TS_Default
};


USTRUCT(BlueprintType)
struct FNoiseMap
{
	GENERATED_BODY()

	TArray<float> Y;
	TArray<float> X;

	FNoiseMap() {}

	FNoiseMap(int32 map_width, int32 map_height)
	{
		this->X.SetNum(map_width);
		this->Y.SetNum(map_height);
	}

};

UCLASS()
class WAYFINDER_API APTerrain : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APTerrain();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	
	bool bIsDestroyed;

	void DestroyTerrain();

	APWorld* PWorld;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* WaterBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters", meta = (AllowPrivateAccess = "true"))
		UMaterial* WaterMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters", meta = (AllowPrivateAccess = "true"))
	FVector WorldOffset;

	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters")
	FVector TerrainWorldLocation;


	/*void GenerateMeshFromWorld(TArray<FVector> vertices, TArray<int32> triangles, TArray<FLinearColor> vertex_colors, TArray<FVector> normals, TArray<FVector2D> uv0, TArray<FProcMeshTangent>tangents);*/
	void GenerateMeshFromWorld(TArray<FVector> vertices, TArray<int32>triangles, TArray<FLinearColor>vertex_colors, TArray<FVector>normals, TArray<FVector2D>uv0, TArray<FProcMeshTangent>tangents);
	
	void GenerateFoliageSpawns(TArray<FVector> water_foliage_spawn_locations, TArray<FVector> meadow_foliage_spawn_locations, TArray<FVector> forest_foliage_spawn_locations,
		TArray<FVector> foothill_foliage_spawn_locations, TArray<FVector> mountain_foliage_spawn_locations, FVector spawn_loc_world_offset);

	void SetMeshMaterial(UMaterial* material);

	void SetTerrainVectors(TArray<FVector> vertices, TArray<int32>triangles, TArray<FLinearColor>vertex_colors, TArray<FVector>normals, TArray<FVector2D>uv0, TArray<FProcMeshTangent>tangents);

	void SetTerrainParams(int uv_scale, int plain_size, float terrain_scale, int seed, float scale, float power_value, int octaves, float persistence, float lacunarity,
		float height_multiplier, UCurveFloat* height_adjustment_curve = nullptr);

protected:

	virtual void OnConstruction(const FTransform& Transform) override;

	void CreateMesh();

	
	void UpdateMesh();
	void PostActorCreated();
	void PostLoad();

	void ResetArrays();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters")

	float Length_mesh;

	UFUNCTION(BlueprintCallable)
		void GenerateNoise(bool bShouldClearCurrent = false);

	void ClearMesh();

	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
		UCurveFloat* HeightAdjustmentCurve;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters")
	ETerrainScale TerrianFidelity;

	

	int SizePlane;

	void SetTerrainScale();

	void CreateMesh(UProceduralMeshComponent* meshcomp);


private:

	UPROPERTY(VisibleAnywhere)
		UProceduralMeshComponent* mesh;


	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
		FVector SpawnLocation;

	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
		int Seed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters", meta = (AllowPrivateAccess = "true"))
		int PlainSize;

	//Adjusts the length between vertices (more land cover less vertices) 
	//who can complain?
	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
		float TerrainScale;


	//Adjusts scale of heightmap samples (bigger mounds & bigger lakes)
	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
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

	//Not currently used
	//will use seed to make unique landscape
	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
		int OffsetX;

	//Not currently used
	//will use seed to make unique landscape
	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
		int OffsetY;

	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
		int UVScale;

	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
		int UVDivisor;

	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
	bool bIsUsingDivisor;

	



};
