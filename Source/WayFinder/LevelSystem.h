// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LevelSystem.generated.h"

class AWayFinderCharacter;
class ABaseEnemy;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WAYFINDER_API ULevelSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULevelSystem();

	FORCEINLINE int32 GetCurrentLevel() const { return this->CurrentLevel; }
	FORCEINLINE float GetExperienceCurrent() const { return this->ExperienceCurrent; }
	FORCEINLINE float GetExperienceToNextLevel() const { return this->ExperienceToNextLevel; }

	UFUNCTION(BlueprintCallable)
	void GainExperience(float experience_to_gain);


	void SetLevelSystemOwner(AWayFinderCharacter* player_owner);


	//Enemy level system stuff
	void SetLevelSystemOwner(ABaseEnemy* player_owner);

	void SetCurrentLevel(int32 level_to_set);


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats", meta = (AllowPrivateAccess = "true"))
	int32 CurrentLevel;

	int32 StartingLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats", meta = (AllowPrivateAccess = "true"))
	float ExperienceCurrent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats", meta = (AllowPrivateAccess = "true"))
	float ExperienceToNextLevel;


	AWayFinderCharacter* PlayerOwner;
	ABaseEnemy* EnemyOwner;



	void CalculateExperienceNextLevel();

	void LevelUp();

		
};
