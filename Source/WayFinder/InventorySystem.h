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

	bool AddItem(AItem* item_to_add, int32 slot_index = -1);

	//Removes item from inventory returns it, to be spawned back to world or destroyed
	//-1 for destoy component
	//0 everything else
	UFUNCTION(BlueprintCallable)
	int32 RemoveItem(uint8 index, uint8 amount_to_remove = 1);

	//If item exists, then attempt to split stack
	UFUNCTION(BlueprintCallable)
	bool SplitItem(uint8 index);

	UFUNCTION(BlueprintCallable)
	bool UseItemAtIndex(uint8 index);

	void SetPlayerOwner(AWayFinderCharacter* player_owner);

	//Used once inventory is added to or shrunk to adjust all items invetory indexs
	void UpdateInventory();

public:

	TSubclassOf<AItem> ItemClass;

	UPROPERTY(EditDefaultsOnly)
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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true", MultiLine = "true"))
	float MaxWeight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory Stats")
	TArray<AItem*> Items;

	AItem* SelectedItem;

	//Used if player wants to swap item inventory spot with another item inventory spot
	AItem* ItemToSwapWith;

protected:

	virtual void BeginPlay() override;

	bool TryAddItemToExistentItemInInventory(AItem* item_to_add, bool binventoryfull = false);

	bool FinalAdd(AItem* item_to_add, int32 amount_to_add = 1);

		
};
