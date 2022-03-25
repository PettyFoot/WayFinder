// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WayFinderHealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WAYFINDER_API UWayFinderHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWayFinderHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FORCEINLINE float GetCurrentHealth() const { return this->CurrentHealth; }
	FORCEINLINE float GetMaxHealth() const { return this->MaxHealth; }

	FORCEINLINE void SetCurrentHealth(float health_adjustment) { this->CurrentHealth += health_adjustment; if (this->CurrentHealth > this->MaxHealth) { this->CurrentHealth = this->MaxHealth; } }
	FORCEINLINE void SetMaxHealth(float max_health_adjustment)  { this->MaxHealth += max_health_adjustment; }


	void HealthTakeDamage(float Damage);


private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health Stats", meta = (AllowPrivateAccess = "True"))
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Stats", meta = (AllowPrivateAccess = "True"))
	float CurrentHealth;

	AActor* OwningActor;

	

		
};
