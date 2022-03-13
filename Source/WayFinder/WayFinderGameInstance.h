// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "WayFinderGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class WAYFINDER_API UWayFinderGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UWayFinderGameInstance(const FObjectInitializer& ObjectInitializer);

	virtual void Init();

	UFUNCTION(Exec, BlueprintCallable)
		void Host();

	UFUNCTION(Exec, BlueprintCallable)
		void Join(const FString& address);
};
