// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "PWorld.generated.h"

class APTerrain;
class AChunkGenerator;
class AWayFinderCharacter;

UCLASS()
class WAYFINDER_API APWorld : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APWorld();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//True or false based on if player has moved enough to have new terrain generate
	bool bShouldGenerateTerrain;

private:

	void CheckShouldGenerateTerrain();

	void CheckPlayerLocation(FVector players_location);

	void GenerateTerrain();

	void CleanUpTerrain();

protected:

	TArray<APTerrain*> GeneratedTerrainChunks;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Paramaters", meta = (AllowPrivateAccess = "true"))
	int PlainSize;

	//Determines how many "chunks" out will render/spawn
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Parameters", meta = (AllowPrivateAccess = "true"))
	int GenerateDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Parameters", meta = (AllowPrivateAccess = "true"))
	int MeshFidelity;

	//Distance player has to be from new terrain chunk to generate chunk
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Parameters", meta = (AllowPrivateAccess = "true"))
	int GenerateDistanceThreshold;

	AChunkGenerator* ChunkGenerator;

	TArray<AWayFinderCharacter*> PlayersInGame;

	TSubclassOf<AWayFinderCharacter> CharacterClass;


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

};
