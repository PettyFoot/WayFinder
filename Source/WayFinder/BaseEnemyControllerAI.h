// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BaseEnemyControllerAI.generated.h"

/**
 * 
 */

class UBlackboardComponent;
class UBehaviorTreeComponent;

UCLASS()
class WAYFINDER_API ABaseEnemyControllerAI : public AAIController
{
	GENERATED_BODY()

public:
	ABaseEnemyControllerAI();

	virtual void OnPossess(APawn* InPawn) override;


public:

	FORCEINLINE  UBlackboardComponent* GetBlackboardComponent() const { return this->BlackboardComponentBase; }
	FORCEINLINE UBehaviorTreeComponent* GetBehaviorTreeComponent() const { return this->BehaviorTreeComponentBase; }

private:

	/* Blackboard component for this controller */
	UPROPERTY(BlueprintReadWrite, Category = "AI Behavior", meta = (AllowPrivateAccess = "true"))
	UBlackboardComponent* BlackboardComponentBase;

	/* Behavior component for this controller */
	UPROPERTY(BlueprintReadWrite, Category = "AI Behavior", meta = (AllowPrivateAccess = "true"))
	UBehaviorTreeComponent* BehaviorTreeComponentBase;
	
};
