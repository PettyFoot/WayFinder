// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item.h"
#include "InventorySystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

//class AItem;
class AWayFinderCharacter;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WAYFINDER_API UInventorySystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventorySystem();

	bool AddItem(AItem* item_to_add);

	UFUNCTION(BlueprintCallable)
	bool RemoveItem(uint8 index);

	void SetPlayerOwner(AWayFinderCharacter* player_owner);

public:

	TSubclassOf<AItem> ItemClass;

	UPROPERTY(EditDefaultsOnly, Instanced)
	TArray<AItem*> DefaultItems;

	UPROPERTY(BlueprintAssignable, Category = "Inventory Stats")
	FOnInventoryUpdated OnInventoryUpdated;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory Stats")
	int32 InventoryCapacity;

	UPROPERTY(VisibleAnywhere, Category = "Inventory Stats")
	uint8 CurrentAvailableIndex;

	uint8 NextStackLeftOver;

	UPROPERTY(VisibleAnywhere, Category = "Inventory Stats")
	AWayFinderCharacter* InventoryOwner;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory Stats")
	TArray<AItem*> Items;

protected:

	virtual void BeginPlay() override;

	bool TryAddItemToExistentItemInInventory(AItem* item_to_add, bool binventoryfull = false);

		
};
