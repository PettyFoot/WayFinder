// Fill out your copyright notice in the Description page of Project Settings.


#include "WayFinderGameInstance.h"

UWayFinderGameInstance::UWayFinderGameInstance(const FObjectInitializer& ObjectInitializer)
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance Conscturctor"));
}

void UWayFinderGameInstance::Init()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance Init"));
}

void UWayFinderGameInstance::Host()
{
	UEngine* Engine = GetEngine();
	if (!Engine) { return; }

	Engine->AddOnScreenDebugMessage(0, 5.f, FColor::Green, TEXT("Hosting"));

	UWorld* world = GetWorld();
	if (!world) { return; }
	
	world->ServerTravel("/Game/ThirdPersonCPP/Maps/Wayfinder_LevelOne_Cave?listen");
}

void UWayFinderGameInstance::Join(const FString& address)
{
	UEngine* Engine = GetEngine();
	if (!Engine) { return; }

	Engine->AddOnScreenDebugMessage(0, 8.f, FColor::Green, FString::Printf(TEXT("Joined %s"), *address));

	APlayerController* player_controller = GetFirstLocalPlayerController();
	if (!player_controller) { return; }

	player_controller->ClientTravel(address, ETravelType::TRAVEL_Absolute);
}