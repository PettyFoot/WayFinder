#include "InventorySystem.h"
#include "InventorySystem.h"
// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySystem.h"
#include "WayFinderCharacter.h"
#include "Consumable.h"


// Sets default values for this component's properties
UInventorySystem::UInventorySystem()
{
	this->InventoryCapacity = 20;
	this->CurrentAvailableIndex = 0;
	this->CurrentWeight = 0.f;
}


// Called when the game starts
void UInventorySystem::BeginPlay()
{
	Super::BeginPlay();
	this->CurrentAvailableIndex = 0;
	this->CurrentWeight = 0.f;
	//for (auto& item : this->DefaultItems)
	//{
	//	this->AddItem(item);
	//}
	
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
	this->Inventory.AddNum(1);
	this->Inventory.AddWeight(item_to_add->ItemWeight); 
	OnInventoryUpdated.Broadcast();
	return true;
}


bool UInventorySystem::AddItem(AItem* item_to_add, int32 slot_index)
{
	//check if inventory has enough weight capacitity to carry new item
	if (this->CurrentAvailableIndex + item_to_add->ItemWeight > this->MaxWeight)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventorySystem::AddItem__ Inventory is full cannot add item to inventory"))
		return false;
	}

	this->SortIncoming(item_to_add);


	//Inventory has capacity and item_to_add exists
	if (this->Items.Num() < this->InventoryCapacity && item_to_add )
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

		this->FinalAdd(item_to_add, item_to_add->ItemCurrentStack);

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



void UInventorySystem::SortIncoming(AItem* item_to_sort)
{
	this->IncomingItem = item_to_sort;
	if (!IsValid(this->IncomingItem)) { return; } //Not a valid item return

	//Check item stackable to try and stack incoming item
	//Skip checking weapons as they aint ever stackable...EVER!

	//Check if inventory is full (via weight or slots)
	if (this->Inventory.GetNum() >= this->InventoryCapacity || this->Inventory.Weight + item_to_sort->ItemWeight > this->InventoryMaxWeight ) { return; }

	if (this->CheckStackable(item_to_sort))
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventorySystem::SortIncoming__ Item Stacked"));
	}
	else
	{
		//Item is not stackable for a variety of reasons
		//Attempt to add item to inventory singularly
		//Weapons
		//Armor
		//Misc/Junk
	}


	this->IncomingItem = nullptr;
}


bool UInventorySystem::CheckStackable(AItem* item_to_check)
{
	//check if item is stackable... very important for a stacking item to be stackable... duh!
	if (!item_to_check->bCanBeStacked) { return false; }

	if (Inventory.GetNum() > 0)
	{
		switch (item_to_check->ItemClass)
		{
		case EItemClass::IC_Weapon:
			return false; ///weapons never stack
			break;
		case EItemClass::IC_Consumable:
			return this->StackConsumable(this->IncomingItem->ConsumableEffectType);
			break;
		case EItemClass::IC_Armor:
			return false; //Consider making certain armors stackable? all armors? idk
			break;
		case EItemClass::IC_QuestItem:
			//TODO, no quest items yet anyways
			break;
		case EItemClass::IC_Readable:
			//TODO, no readables yet anyways
			return false;
			break;
		case EItemClass::IC_Default:
			UE_LOG(LogTemp, Warning, TEXT("UInventorySystem::CheckStackable__ Reached default of switch case due to default item class enum"))
				break;
		default:
			break;
		}
	}
	
	UE_LOG(LogTemp, Warning, TEXT("UInventorySystem::CheckStackable__ Inventory does not have more than 1 item, nothing to stack with"));
	return false;
}

bool UInventorySystem::AddItemInv()
{


	switch (this->IncomingItem->ItemClass)
	{
	case EItemClass::IC_Weapon:
		return false; ///weapons never stack
		break;
	case EItemClass::IC_Consumable:
		return this->StackConsumable(this->IncomingItem->ConsumableEffectType);
		break;
	case EItemClass::IC_Armor:
		return false; //Consider making certain armors stackable? all armors? idk
		break;
	case EItemClass::IC_QuestItem:
		//TODO, no quest items yet anyways
		break;
	case EItemClass::IC_Readable:
		//TODO, no readables yet anyways
		return false;
		break;
	case EItemClass::IC_Default:
		UE_LOG(LogTemp, Warning, TEXT("UInventorySystem::CheckStackable__ Reached default of switch case due to default item class enum"))
			break;

	return false;
}



bool UInventorySystem::StackConsumable(EConsumableEffectType consume_effect_type)
{

	switch (consume_effect_type)
	{
	case EConsumableEffectType::CET_Potion:
		this->SortPotion(this->IncomingItem->ConsumableType.PotionType, true);
		break;
	case EConsumableEffectType::CET_FoodDrink:
		//TODO, need to implement food/drink
		break;
	case EConsumableEffectType::CET_Buff:
		//TODO, buffs are implemented alrea
		break;
	case EConsumableEffectType::CET_Default:
		UE_LOG(LogTemp, Warning, TEXT("UInventorySystem::StackConsumable__ Could not find ConsumableEffectType on incoming item"));
		break;
	default:
		break;
	}
	return false;
	
}

bool UInventorySystem::SortPotion(EPotionType potion, bool bshould_b_stacked)
{
	switch (potion)
	{
	case EPotionType::PT_Heal:
		this->AddToExistent(this->Inventory.Consumable.Potions.Health);
		break;
	case EPotionType::PT_Ult:
		this->AddToExistent(this->Inventory.Consumable.Potions.Ult);
		break;
	case EPotionType::PT_Stamina:
		this->AddToExistent(this->Inventory.Consumable.Potions.Stamina);
		break;
	case EPotionType::PT_Magica:
		this->AddToExistent(this->Inventory.Consumable.Potions.Magica);
		break;
	case EPotionType::PT_Poison:
		this->AddToExistent(this->Inventory.Consumable.Potions.Poison);
		break;
	case EPotionType::PT_Fire:
		this->AddToExistent(this->Inventory.Consumable.Potions.Fire);
		break;
	case EPotionType::PT_Bleed:
		this->AddToExistent(this->Inventory.Consumable.Potions.Bleed);
		break;
	case EPotionType::PT_Default:
		UE_LOG(LogTemp, Warning, TEXT("UInventorySystem::SortPotion__ Could not sort potion, set to EPotionType::PT_Default"));
		break;
	default:
		break;
	}
	return false;
}

bool UInventorySystem::SortFoodDrink(EFoodDrinkType food_drink, bool bshould_b_stacked)
{
	switch (food_drink)
	{
	case EFoodDrinkType::FD_Food:
		this->AddToExistent(this->Inventory.Consumable.FoodDrink.Food);
		break;
	case EFoodDrinkType::FD_Drink:
		this->AddToExistent(this->Inventory.Consumable.FoodDrink.Drink);
		break;
	case EFoodDrinkType::FD_Default:
		UE_LOG(LogTemp, Warning, TEXT("UInventorySystem::SetFoodDrink__ Default food drink enum"))
		break;
	default:
		break;
	}


	return false;
}

bool UInventorySystem::SortBuff(EBuffType buff, bool bshould_b_stacked)
{
	switch (buff)
	{
	case EBuffType::BUFF_Fortify:
		this->AddToExistent(this->Inventory.Consumable.Buffs.Fortify);
		break;
	case EBuffType::BUFF_Swift:
		this->AddToExistent(this->Inventory.Consumable.Buffs.Swift);
		break;
	case EBuffType::BUFF_Enrage:
		this->AddToExistent(this->Inventory.Consumable.Buffs.Enrage);
		break;
	case EBuffType::BUFF_Invigorate:
		this->AddToExistent(this->Inventory.Consumable.Buffs.Invigorate);
		break;
	case EBuffType::BUFF_Shield:
		this->AddToExistent(this->Inventory.Consumable.Buffs.Shield);
		break;
	case EBuffType::BUFF_Default:
		UE_LOG(LogTemp, Warning, TEXT("UInventorySystem::SetBuff__ Default buff enum"));
		break;
	default:
		break;
	}

	return false;
}



bool UInventorySystem::AddToExistent(TArray<AItem*> inventory_array, bool bshould_b_stacked)
{
	return false;
}

bool UInventorySystem::AddToExistent(TArray<AConsumable*> inventory_array)
{
	AConsumable* incoming_consume = Cast<AConsumable>(this->IncomingItem);
	if (!incoming_consume) { return false; }

	//Loop through array and find stackable match
	for (auto& consumable : inventory_array)
	{
		if (this->CheckInventoryMatch(incoming_consume, consumable))
		{
			//if inventory is a match add to item stack
			if (consumable->ItemCurrentStack < consumable->ItemMaxStack) //Check to see stack is not already full
			{
				//Is item's count + amount to increa greater than max stack size?
				if (consumable->ItemCurrentStack + incoming_consume->ItemCurrentStack > consumable->ItemMaxStack)
				{
					//Split stack
					uint8 next_stack_leftover = (consumable->ItemCurrentStack + incoming_consume->ItemCurrentStack) - consumable->ItemMaxStack; //left over after maxing stack
					consumable->ItemCurrentStack = consumable->ItemMaxStack; //Set stack to max

					//True if added to item araray, false otherwise
					// will only return false if inventoryowener is invalid (nullptr)
					return this->FinalAdd(incoming_consume, next_stack_leftover); //TODO need to update this bc it has deprecated container usage etc.

				}
				else
				{
					//Add item_to_add's item amount to inventory item found's stack
					consumable->ItemCurrentStack += incoming_consume->ItemCurrentStack;
					incoming_consume->AddedToInventory(nullptr); //Destroy item to add out in the world (Not right now though, bc actor spawn issue)
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

bool UInventorySystem::CheckInventoryMatch(AItem* incoming_item, AItem* inventory_item)
{
	if (incoming_item->ItemDisplayName == inventory_item->ItemDisplayName)
	{
		if (incoming_item->ItemLevel == inventory_item->ItemLevel)
		{
			if (incoming_item->ItemImage == inventory_item->ItemImage)
			{
					return true;
			}
		}
	}
	return false;
}

bool UInventorySystem::CheckInventoryMatch(AConsumable* incoming_consumanle, AConsumable* inventory_consumable)
{
	if (incoming_consumanle->ItemDisplayName == inventory_consumable->ItemDisplayName)
	{
		if (incoming_consumanle->EffectDuration == inventory_consumable->EffectDuration)
		{
			if (incoming_consumanle->ItemLevel == inventory_consumable->ItemLevel)
			{
				if (incoming_consumanle->ItemImage == inventory_consumable->ItemImage)
				{
					return true;
				}
	
			}
		}
	}
	return false;
}

bool UInventorySystem::StackItem()
{
	return false;
}




