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
						this->UpdateInventory();
						OnInventoryUpdated.Broadcast();
						return true;
					}

					item_to_add->ItemCurrentStack = next_stack_leftover; //Set in item's count to leftover stack amount
					item_to_add->OwningInventory = this; //Set item's owning inventory to this one
					item_to_add->InventorySlotIndex = this->CurrentAvailableIndex; //Set item to add's inventory index

					this->Items.Add(item_to_add); //Add item to items array 
					if (this->InventoryOwner)
					{
						item_to_add->AddedToInventory(this->InventoryOwner);
						this->Items[this->CurrentAvailableIndex]->World = this->InventoryOwner->GetWorld(); //Set item's world to this inventory's world
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("Invalid inventory owner AddItem::InventorySystem"));
					}

					CurrentAvailableIndex++; //Increment inventory index
					this->UpdateInventory();
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


bool UInventorySystem::AddItem(AItem* item_to_add, int32 slot_index)
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
		this->UpdateInventory();
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

int32 UInventorySystem::RemoveItem(uint8 index, uint8 amount_to_remove)
{
	if (!this->Items[index])
	{
		return -2;
	}
	
	if (this->Items[index]->ItemCurrentStack - amount_to_remove <= 0)
	{
		AItem* item_point = this->Items[index];
		this->Items.RemoveAt(index);
		this->CurrentAvailableIndex--;
		this->UpdateInventory();
		OnInventoryUpdated.Broadcast();
		return -1;
	}
	else
	{
		AItem* item_point = this->Items[index];
		this->Items[index]->ItemCurrentStack -= amount_to_remove;
		this->CurrentAvailableIndex--;
		this->UpdateInventory();
		OnInventoryUpdated.Broadcast();
		return 0;
	}
	
	
	return 0;
}

bool UInventorySystem::SplitItem(uint8 index)
{
	//Item does not exist or can't be stacked so don't split
	if (!this->Items[index] || !this->Items[index]->bCanBeStacked || this->Items.Num() == this->InventoryCapacity || this->Items[index]->ItemCurrentStack <= 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Item does not exists or can't be split or inventory is too full to split SplitItem::InventorySystem"));
		return false;
	}

	uint8 test_modulo = this->Items[index]->ItemCurrentStack % 2;
	uint8 stack_size(0);
	bool bIsEven = false;
	if (test_modulo == 0) //current item stack can be evenly split
	{
		stack_size = this->Items[index]->ItemCurrentStack / 2;
		bIsEven = true;
	}
	else
	{
		this->Items[index]->ItemCurrentStack += 1;
		stack_size = this->Items[index]->ItemCurrentStack / 2;
		bIsEven = false;
	}

	//This hopefully spawns a new item with 
	AItem* new_item = this->Items[index]->GetWorld()->SpawnActor<AItem>(this->Items[index]->GetClass());
	new_item->SetItemState(EItemState::EIS_InInventory);
	this->Items[index]->ItemCurrentStack = stack_size; //Set stack current to other half of split stack
	new_item->CopyItemStats(this->Items[index]); //Copy stats from 
	if (!bIsEven) { new_item->ItemCurrentStack -= 1; } //adjustment if stack size is not even

	 //Add item to inventory at next available position
	this->Items.Add(new_item);


	//Add new item to inventory
	this->Items[this->CurrentAvailableIndex]->OwningInventory = this; //Set item's owning inventory to this one
	if (this->InventoryOwner)
	{
		this->Items[this->CurrentAvailableIndex]->AddedToInventory(this->InventoryOwner);
		this->Items[this->CurrentAvailableIndex]->World = this->InventoryOwner->GetWorld(); //Set item's world to this inventory's world
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid inventory owner AddItem::InventorySystem_____This is bad"));
	}
	
	this->Items[this->CurrentAvailableIndex]->InventorySlotIndex = this->CurrentAvailableIndex;
	CurrentAvailableIndex++;
	OnInventoryUpdated.Broadcast();
	return true;
}

bool UInventorySystem::UseItemAtIndex(uint8 index)
{
	if (!this->Items[index] || !this->InventoryOwner)
	{
		return false;
	}
	
	this->Items[index]->UseItem(this->Items[index]->PlayerOwner);
	return true;
}

void UInventorySystem::SetPlayerOwner(AWayFinderCharacter* player_owner)
{
	if (player_owner)
	{
		this->InventoryOwner = player_owner;
		UE_LOG(LogTemp, Warning, TEXT("Set owner of inventory to: %s"), *player_owner->GetName());
	}
	
}

void UInventorySystem::UpdateInventory()
{
	for (int i = 0; i < this->Items.Num(); i++)
	{
		this->Items[i]->InventorySlotIndex = i;
	}
}
