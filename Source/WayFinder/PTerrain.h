// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "GameFramework/Actor.h"
#include "PTerrain.generated.h"

class UProceduralMeshComponent;
class UCurveFloat;


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


protected:

	virtual void OnConstruction(const FTransform& Transform) override;

	void CreateMesh();
	void UpdateMesh();
	void PostActorCreated();
	void PostLoad();

	void ResetArrays();

	UFUNCTION(BlueprintCallable)
		void GenerateNoise(bool bShouldClearCurrent = false);

	void ClearMesh();

	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
		UCurveFloat* HeightAdjustmentCurve;

	//vertices matrix (x,y,z)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters")
	TArray<FVector> vertices;

	//order in which vertices should be joined together creating a mesh of triangles
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters")
	TArray<int32> Triangles;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters")
	TArray<FLinearColor> vertexColors;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters")
	TArray<FVector> normals;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters")
	TArray<FVector2D> UV0;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters")
	TArray<FProcMeshTangent> tangents;


private:

	UPROPERTY(VisibleAnywhere)
		UProceduralMeshComponent* mesh;



	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
		int Seed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh Paramaters", meta= (AllowPrivateAccess = "true"))
		int PlainSize;
	int PreviousPlainSize;

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
		int PlainWidth;


	UPROPERTY(EditAnywhere, Category = "Mesh Paramaters")
		int PlainHeight;


	FNoiseMap NoiseMap;


};
