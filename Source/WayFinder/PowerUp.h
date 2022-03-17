// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PowerUp.generated.h"


//TODO
//figure out what other types of power ups there will be and how to handle them/categorize them
//Add sounds once picked up

UCLASS()
class WAYFINDER_API APowerUp : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APowerUp();

	UFUNCTION(BlueprintCallable)
	float GetPowerUpAmount() { return this->PowerUpAmount; }


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PowerUp Properties")
	float PowerUpAmount;

private:

	
	
};
