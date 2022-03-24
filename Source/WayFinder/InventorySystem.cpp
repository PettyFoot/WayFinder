// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySystem.h"
#include "WayFinderCharacter.h"


// Sets default values for this component's properties
UInventorySystem::UInventorySystem()
{
	this->InventoryCapacity = 20;
	this->CurrentAvailableIndex = 0;
}


// Called when the game starts
void UInventorySystem::BeginPlay()
{
	Super::BeginPlay();

	for (auto& item : this->DefaultItems)
	{
		this->AddItem(item);
	}
	
}

bool UInventorySystem::TryAddItemToExistentItemInInventory(AItem* item_to_add, bool binventoryfull)
{
	
	for (auto& item : this->Items)
	{
		if (item->ItemDisplayName == item_to_add->ItemDisplayName)//see if item name's match
		{
			//item we are trying to add is already in inventory attempt to increment item's count in inventory
			if (item->ItemCurrentStack != item->ItemMaxStack) //Check to see stack is not already full, skip this if it is
			{
				//Check item to add's amount and current item's count are greater than the max__ Split stack if true
				if (item->ItemCurrentStack + item_to_add->ItemCurrentStack > item->ItemMaxStack)
				{
					//Split stack
					uint8 next_stack_leftover = (item->ItemCurrentStack + item_to_add->ItemCurrentStack) - item->ItemMaxStack; //left over after maxing stack
					item->ItemCurrentStack = item->ItemMaxStack; //Set stack to max
					if (binventoryfull) //if inventory is full but found item to add to add to it and return here
					{
						item_to_add->ItemCurrentStack = next_stack_leftover;
						return true;
					}
					item_to_add->ItemCurrentStack = next_stack_leftover; //Set in item's count to leftover stack amount

					item_to_add->OwningInventory = this; //Set item's owning inventory to this one
					item_to_add->World = GetWorld(); //Set item's world to this inventory's world
					item_to_add->InventorySlotIndex = this->CurrentAvailableIndex;

					this->Items.Add(item_to_add); //Add item to items array 
					if (this->InventoryOwner)
					{
						item_to_add->AddedToInventory(this->InventoryOwner);
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("Invalid inventory owner AddItem::InventorySystem"));
					}

					CurrentAvailableIndex++;

					OnInventoryUpdated.Broadcast();

					return true;

				}
				else
				{
					item->ItemCurrentStack += item_to_add->ItemCurrentStack;
					item_to_add->AddedToInventory(nullptr); //Destroy item to add out in the world
					OnInventoryUpdated.Broadcast();
					return true;

				}
			}
			else {}
		}
		else {}
	}

	return false;
}


bool UInventorySystem::AddItem(AItem* item_to_add)
{
	//Inventory has capacity and item_to_add exists
	if (this->Items.Num() < this->InventoryCapacity && item_to_add)
	{
		UE_LOG(LogTemp, Warning, TEXT("Added to inventory"));
		//If item can be stacked run through the inventory to see if item to add is existent in inventory and attempt to add there if possible
		if (item_to_add->bCanBeStacked)
		{
			//Return true if item was added to inventory
			if (this->TryAddItemToExistentItemInInventory(item_to_add))
			{
				return true;
			}
		}
		
		//inventory has space and incoming item can't be stacked with other item in inventory 
		item_to_add->OwningInventory = this; //Set item's owning inventory to this one
		item_to_add->World = GetWorld(); //Set item's world to this inventory's world
		item_to_add->InventorySlotIndex = this->CurrentAvailableIndex;
		this->Items.Add(item_to_add); //Add item to items array 
		if (this->InventoryOwner)
		{
			item_to_add->AddedToInventory(this->InventoryOwner);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid inventory owner AddItem::InventorySystem"));
		}
		CurrentAvailableIndex++; 
		OnInventoryUpdated.Broadcast();
		return true;
	}
	else
	{
		//Check inventory one last time to see if the item trying to add exists and add to it
		UE_LOG(LogTemp, Warning, TEXT("Inventory Full or item to add is nullptr"));
		return this->TryAddItemToExistentItemInInventory(item_to_add, true);
	}
	
	return false;
}

bool UInventorySystem::RemoveItem(uint8 index)
{
	if (index < 200)
	{
		//item_to_remove->OwningInventory = nullptr;
		//item_to_remove->World = nullptr;
		//item_to_remove->InventorySlotIndex = -1; //Reset item's inventory stats
		
		//CurrentAvailableIndex--;
		UE_LOG(LogTemp, Warning, TEXT("slot:  %d"), index)
		OnInventoryUpdated.Broadcast();
		
		return true;
	}
	return false;
}

void UInventorySystem::SetPlayerOwner(AWayFinderCharacter* player_owner)
{
	if (player_owner)
	{
		this->InventoryOwner = player_owner;
		UE_LOG(LogTemp, Warning, TEXT("Set owner of inventory to: %s"), *player_owner->GetName());
	}
	
}
