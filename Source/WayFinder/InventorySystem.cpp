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
	//Iterate through items in the items array
	for (auto& item : this->Items)
	{
		if (item->ItemDisplayName == item_to_add->ItemDisplayName)//see if item name's match
		{
			//item we are trying to add is already in inventory attempt to increment item's count in inventory
			if (item->ItemCurrentStack < item->ItemMaxStack) //Check to see stack is not already full
			{
				//Is item's count + amount to increa greater than max stack size?
				if (item->ItemCurrentStack + item_to_add->ItemCurrentStack > item->ItemMaxStack)
				{
					//Split stack
					uint8 next_stack_leftover = (item->ItemCurrentStack + item_to_add->ItemCurrentStack) - item->ItemMaxStack; //left over after maxing stack
					item->ItemCurrentStack = item->ItemMaxStack; //Set stack to max

					if (binventoryfull) //if inventory is full but found matching item with enough space to add to
					{
						item_to_add->ItemCurrentStack = next_stack_leftover;
						this->UpdateInventory(); //Ensure all items in inventory have the proper index
						OnInventoryUpdated.Broadcast();
						return true;
					}

					//True if added to item araray, false otherwise
					return this->FinalAdd(item_to_add, next_stack_leftover);

				}
				else
				{
					//Add item_to_add's item amount to inventory item found's stack
					item->ItemCurrentStack += item_to_add->ItemCurrentStack;
					item_to_add->AddedToInventory(nullptr); //Destroy item to add out in the world (Not right now though, bc actor spawn issue)
					OnInventoryUpdated.Broadcast();
					return true;
				}

			}else {}
		}else {}
	}

	return false;
}

bool UInventorySystem::FinalAdd(AItem* item_to_add, int32 amount_to_add)
{
	item_to_add->ItemCurrentStack = amount_to_add; //Set in item's count to leftover stack amount
	item_to_add->OwningInventory = this; //Set item's owning inventory to this one
	item_to_add->InventorySlotIndex = this->CurrentAvailableIndex; //Set item to add's inventory index

	if (this->InventoryOwner)
	{
		this->Items.Add(item_to_add); //Add item to items array 
		item_to_add->AddedToInventory(this->InventoryOwner);
		CurrentAvailableIndex++; //Increment inventory index
		UE_LOG(LogTemp, Warning, TEXT("UInventorySystem::FinalAdd__ Added item to inventory: %s"), *item_to_add->ItemDisplayName);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UInventorySystem::FinalAdd__  Could not find inventory owner, did not add to inventory"));
		return false;
	}
	
	//Update inventory slot indices and broadcast
	this->UpdateInventory(); 
	OnInventoryUpdated.Broadcast();
	return true;
}


bool UInventorySystem::AddItem(AItem* item_to_add, int32 slot_index)
{
	
	//Inventory has capacity and item_to_add exists
	if (this->Items.Num() < this->InventoryCapacity && item_to_add)
	{
		//Can item be stacked?
		if (item_to_add->bCanBeStacked)
		{
			//Return true if item was added to inventory
			if (this->TryAddItemToExistentItemInInventory(item_to_add)) //try to stack item if item is stackable
			{
				UE_LOG(LogTemp, Warning, TEXT("Added to inventory"));
				return true;
			}
		}

		this->FinalAdd(item_to_add);

		return true;
	}
	else
	{
		//Check inventory one last time to see if the item trying to add exists and add to it
		return this->TryAddItemToExistentItemInInventory(item_to_add, true); //2nd param true because inventory slot's are full and trying to "top off" any stackable items in inventory
	}
	
	return false; //did not add item
}

int32 UInventorySystem::RemoveItem(uint8 index, uint8 amount_to_remove)
{
	if (!IsValid(this->Items[index])) 
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
	if (!IsValid(this->Items[index]) || !this->Items[index]->bCanBeStacked || this->Items.Num() == this->InventoryCapacity || this->Items[index]->ItemCurrentStack <= 1)
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
	if (IsValid(this->Items[index]))
	{
		AItem* new_item = this->Items[index]->GetWorld()->SpawnActor<AItem>(this->Items[index]->GetClass());
		new_item->SetItemState(EItemState::EIS_InInventory);
		this->Items[index]->ItemCurrentStack = stack_size; //Set stack current to other half of split stack
		new_item->CopyItemStats(this->Items[index]); //Copy stats from 
		if (!bIsEven) { new_item->ItemCurrentStack -= 1; } //adjustment if stack size is not even

		 //Add item to inventory at next available position
		this->Items.Add(new_item);
	}
	else { UE_LOG(LogTemp, Error, TEXT("UInventorySystem::SplitItem__ Could not find item to split in inventory")); }

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
	if (!IsValid(this->Items[index]) || !this->InventoryOwner)
	{
		UE_LOG(LogTemp, Error, TEXT("UInventorySystem::UseItem__  Failed to use item:: no item exists at index: %d or no inventory owner"), index);
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
		if (IsValid(this->Items[i]))
		{
			this->Items[i]->InventorySlotIndex = i;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("UInventorySystem::UpdateInventory__  Failed inventory item indice update:: no item at index %d"), i);
		}
		
	}
}
