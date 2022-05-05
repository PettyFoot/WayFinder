// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "GameFramework/Actor.h"
#include "PTerrain.generated.h"

class UProceduralMeshComponent;
class UCurveFloat;

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
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters")
	FVector TerrainWorldLocation;


protected:

	virtual void OnConstruction(const FTransform& Transform) override;

	void CreateMesh();
	UFUNCTION(BlueprintCallable)
	void CreateMesh(float x_pos, float y_pos);
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
	TArray<FVector> vertices;

	//order in which vertices should be joined together creating a mesh of triangles
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters")
	TArray<int32> Triangles;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters")
	TArray<FLinearColor> vertexColors;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters")
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters")
	TArray<FVector> normals;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters")
	TArray<FVector2D> UV0;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters")
	TArray<FProcMeshTangent> tangents;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters")
	ETerrainScale TerrianFidelity;

	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
	float TerrainScale;

	int SizePlane;

	void SetTerrainScale();

	void CreateMesh(UProceduralMeshComponent* meshcomp);

	void CreateMeshOld(UProceduralMeshComponent* meshcomp);


private:

	UPROPERTY(VisibleAnywhere)
		UProceduralMeshComponent* mesh;
	UPROPERTY(VisibleAnywhere)
		UProceduralMeshComponent* meshother_test;

	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
		int Seed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters", meta = (AllowPrivateAccess = "true"))
		int PlainSize;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters", meta = (AllowPrivateAccess = "true", ClampMin = 1, ClampMax = 5))
		int MeshFidelity;

	//Keep at whole numbers
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters", meta = (AllowPrivateAccess = "true", ClampMin = 1, ClampMax = 6))
		int UVScale;

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
		int OffsetX;


	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
		int OffsetY;



	//FNoiseMap NoiseMap;


};
