// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingPlatform.generated.h"

UCLASS()
class WAYFINDER_API AMovingPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMovingPlatform();

	void EnablePlatformMovement(bool b_is_movement_enabled) { this->bIsAbleToMove = b_is_movement_enabled; }


public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components", meta = (MakeEditWidget = "true"))
		FVector TargetLocationOne;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
		FVector TargetLocationTwo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
		FVector TargetLocationThree;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MovePlatform(float DeltaTime);

private:

	//Start Location
	UPROPERTY(EditAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
		FVector StartingLocation;

	/* Location of 3 targets for platform to move to */
	UPROPERTY(EditAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
		FVector StartingTargetLocationOne;

	UPROPERTY(EditAnywhere, Category = "Components")
		FVector StartingTargetLocationTwo;

	UPROPERTY(EditAnywhere, Category = "Components")
		FVector StartingTargetLocationThree;

	/* Speed adjuster */
	UPROPERTY(EditAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
		float Speed;

	UPROPERTY(VisibleAnywhere, Category = "State")
		bool bIsAbleToMove;


private:

	void InitReplication();

	void InitPlatformTargetLocations();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
