// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryStructs.h"
#include "InventorySystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

//class AItem;
class AWayFinderCharacter;



USTRUCT(BlueprintType)
struct FInventory
{
	GENERATED_BODY()
	FIWeapons Weapons;
	FIWearables Wearable;
	FIConsumables Consumable;
	TArray<AItem*> Quest; //Don't need to divide up quest items bc they don't have stat variability
	TArray<AItem*> Misc; //For junk and readables?

	int32 GetNum() const { return Num; }
	void SetNum(int32 num) { Num = num; }
	void AddNum(int32 num_to_add) { Num += num_to_add; }
private:

	int32 Num;



};




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

	//Inventory struct that divides inventory into sections
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory")
	FInventory Inventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory Stats")
		TArray<AItem*> Items;

	//UPROPERTY(EditDefaultsOnly)
	//TArray<AItem*> DefaultItems;

	UPROPERTY(BlueprintAssignable, Category = "Inventory Stats")
	FOnInventoryUpdated OnInventoryUpdated;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory Stats")
	int32 InventoryCapacity;

	UPROPERTY(VisibleAnywhere, Category = "Inventory Stats")
	AWayFinderCharacter* InventoryOwner;


protected:

	virtual void BeginPlay() override;

	bool TryAddItemToExistentItemInInventory(AItem* item_to_add, bool binventoryfull = false);

	bool FinalAdd(AItem* item_to_add, int32 amount_to_add = 1);


private:

	UPROPERTY(VisibleAnywhere, Category = "Inventory Stats")
		uint8 CurrentAvailableIndex;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true", MultiLine = "true"))
		float MaxWeight;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true", MultiLine = "true"))
		float CurrentWeight;

	uint8 NextStackLeftOver;

	AItem* SelectedItem;

	AItem* IncomingItem;

	//Used if player wants to swap item inventory spot with another item inventory spot
	AItem* ItemToSwapWith;

private:

	//Initializes sorting of incoming item
	//Will first try to stack item if stackable (will also handle overflow if stackable), will try to add if not stackable
	void SortIncoming(AItem* item_to_sort);

	//returns true if item was stacked, false otherwise
	bool CheckStackable(AItem* item_to_check);

	//Return true if consumable was stacked in invetory, false else
	bool StackConsumable(EConsumableEffectType consume_effect_type);


	bool SortPotion(EPotionType potion, bool bshould_b_stacked);
	bool SortFoodDrink(EFoodDrinkType food_drink, bool bshould_b_stacked);
	bool SortBuff(EBuffType buff, bool bshould_b_stacked);
	
	

	//Pass in desired inventory array from FInventory struct
	//Loops through that list and attempts to "stack" item
	//Returns true if stacked, otherwise false
	//@param bool - set to true if needed to stack, otherwise don't set and use defualt false will not stack item
	bool AddToExistent(TArray<AItem*> inventory_array, bool bshould_b_stacked = false);

	//Overloaded for consumables
	//same return params as above
	bool AddToExistent(TArray<AConsumable*> inventory_array);

	bool CheckInventoryMatch(AItem* incoming_item, AItem* inventory_item);
	bool CheckInventoryMatch(AConsumable* incoming_item, AConsumable* inventory_item);

	bool StackItem();
	


		
};
