// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapZone.generated.h"

class UBoxComponent;

UCLASS()
class WAYFINDER_API AMapZone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMapZone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "GameMode Zones", meta = (AllowPrivateAccess = "true"))
		UBoxComponent* ZoneOne;
	UPROPERTY(EditAnywhere, Category = "GameMode Zones", meta = (AllowPrivateAccess = "true"))
		UBoxComponent* ZoneTwo;
	UPROPERTY(EditAnywhere, Category = "GameMode Zones", meta = (AllowPrivateAccess = "true"))
		UBoxComponent* ZoneThree;
	UPROPERTY(EditAnywhere, Category = "GameMode Zones", meta = (AllowPrivateAccess = "true"))
		UBoxComponent* ZoneFour;

};
