// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFly.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AEnemyFly::AEnemyFly()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;



}

// Called when the game starts or when spawned
void AEnemyFly::BeginPlay()
{
	Super::BeginPlay();
	
	
}

// Called every frame
void AEnemyFly::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyFly::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

