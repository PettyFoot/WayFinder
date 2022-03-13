// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemyControllerAI.h"
#include "BaseEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"


ABaseEnemyControllerAI::ABaseEnemyControllerAI()
{
	this->BlackboardComponentBase = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BehaviorTreeComponentBase"));
	check(this->BlackboardComponentBase);
	
	this->BehaviorTreeComponentBase = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BlackboardComponentBase"));
	check(this->BehaviorTreeComponentBase);
}

void ABaseEnemyControllerAI::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!InPawn) { return; }

	// Attempt cast of inpawn to ABasenemy, check if valid
	ABaseEnemy* enemy = Cast <ABaseEnemy> (InPawn);
	if (enemy) 
	{
		if (enemy->GetBehaviorTree())
		{
			this->BlackboardComponentBase->InitializeBlackboard(*(enemy->GetBehaviorTree()->BlackboardAsset));
		}
	}

}
