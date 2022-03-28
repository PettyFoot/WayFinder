// Fill out your copyright notice in the Description page of Project Settings.


#include "LootTable.h"
#include "WayFinderCharacter.h"
#include "Item.h"
#include "Consumable.h"
#include "BaseMeleeWeapon.h"
#include "Weapon.h"
#include "ItemInfo.h"
#include "Engine/SkeletalMesh.h"
#include "BaseEnemy.h"
#include "WayFinderGameMode.h"


// Sets default values for this component's properties
ULootTable::ULootTable():
	NumberItemsToGenerate(1),
	MinNumberItemsToGenerate(1)
{

}


// Called when the game starts
void ULootTable::BeginPlay()
{
	Super::BeginPlay();
	this->InitRarityDropTableDT();

}

void ULootTable::InitRarityDropTableDT()
{
	if (!this->LootTableOwner) { return; } //No enemy don't do anything
	//Path to WaveStats data table 
	FString rarity_loottable_dt_path(TEXT("DataTable'/Game/Game/ItemDropChances_DT.ItemDropChances_DT'"));

	UDataTable* dt_obj = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *rarity_loottable_dt_path));
	if (dt_obj)
	{
		UE_LOG(LogTemp, Warning, TEXT("data table found"));
		FItemDropChances* rarity_row = nullptr;

		switch (this->LootTableOwner->LootTableRarityPercentageEnum)
		{
		case ELootTableAdjuster::EIR_A:
			rarity_row = dt_obj->FindRow<FItemDropChances>(FName("A"), TEXT(""));
			break;
		case ELootTableAdjuster::EIR_B:
			rarity_row = dt_obj->FindRow<FItemDropChances>(FName("B"), TEXT(""));
			break;
		case ELootTableAdjuster::EIR_C:
			rarity_row = dt_obj->FindRow<FItemDropChances>(FName("C"), TEXT(""));
			break;
		case ELootTableAdjuster::EIR_D:
			rarity_row = dt_obj->FindRow<FItemDropChances>(FName("D"), TEXT(""));
			break;
		case ELootTableAdjuster::EIR_E:
			rarity_row = dt_obj->FindRow<FItemDropChances>(FName("E"), TEXT(""));
			break;
		default:
			break;
		}

		if (rarity_row)
		{
			rarity_percent_A = rarity_row->rarity_percent_A_DT;
			rarity_percent_B = rarity_row->rarity_percent_B_DT;
			rarity_percent_C = rarity_row->rarity_percent_C_DT;
			rarity_percent_D = rarity_row->rarity_percent_D_DT;
			rarity_percent_E = rarity_row->rarity_percent_E_DT;
			rarity_percent_ItemSpawnNumberA = rarity_row->rarity_percent_ItemSpawnNumberA_DT;
			rarity_percent_ItemSpawnNumberB = rarity_row->rarity_percent_ItemSpawnNumberB_DT;
			rarity_percent_ItemSpawnNumberC = rarity_row->rarity_percent_ItemSpawnNumberC_DT;
		}

	}
}

void ULootTable::SpawnLoot()
{
	if (!this->LootTableOwner) { return; }
	this->GenerateItemsToDrop();

	//for (auto& info : this->ItemsToSpawn)
	//{
		//this->LootTableOwner->SpawnItem(&info);
	//}
	
}

void ULootTable::GenerateItemsToDrop()
{
	if (!this->LootTableOwner) { return; }

	this->CalculateSpawnParams();

	for (int i = 0; i < this->NumberItemsToGenerate; i++)
	{
		this->GenerateItem();

	}
}

void ULootTable::ThrowItem(AItem* item_to_toss)
{/* Get the yaw rotation of weapon, zero out the pitch and roll, set weapon's rotation to this */
	FRotator mesh_rot(0.f, item_to_toss->ITemStaticMeshComponent->GetComponentRotation().Yaw, 0.f);
	item_to_toss->ITemStaticMeshComponent->SetWorldRotation(mesh_rot, false, NULL, ETeleportType::TeleportPhysics);
	/* Get forward and right vector */

	
	const FVector mesh_for(item_to_toss->ITemStaticMeshComponent->GetForwardVector());
	const FVector mesh_right(item_to_toss->ITemStaticMeshComponent->GetRightVector());
	/* direction to throw the weapon */
	FVector impulse_dir = mesh_right.RotateAngleAxis(-20.f, mesh_for);
	

	/* random impulse rotation, create impulse direction and magnitude */
	float random_rot(FMath::FRandRange(25.f, 35.f));
	impulse_dir = impulse_dir.RotateAngleAxis(random_rot, FVector(0.f, 0.f, 1.f));
	impulse_dir *= 6000;

	/* Add the impulse */
//	GetItemMesh()->AddImpulse(impulse_dir);
	item_to_toss->ITemStaticMeshComponent->AddImpulse(impulse_dir);
}

void ULootTable::GenerateItem()
{
	if (!this->LootTableOwner) { return; }

	TArray<FLootTableTierByLevel> LootTableLevel;

	//Get loot table's level tier
	if (this->LootTableOwner->GetEnemyLevel() >= 1 && this->LootTableOwner->GetEnemyLevel() <= 20)
	{
		LootTableLevel = this->LootTableItemDB->LootTable_1_20_DT;
	}
	else if (this->LootTableOwner->GetEnemyLevel() >= 21 && this->LootTableOwner->GetEnemyLevel() <= 30)
	{
		LootTableLevel = this->LootTableItemDB->LootTable_21_30_DT;
	}
	else if (this->LootTableOwner->GetEnemyLevel() >= 31 && this->LootTableOwner->GetEnemyLevel() <= 40)
	{
		LootTableLevel = this->LootTableItemDB->LootTable_31_40_DT;
	}
	else if (this->LootTableOwner->GetEnemyLevel() >= 41 && this->LootTableOwner->GetEnemyLevel() <= 50)
	{
		LootTableLevel = this->LootTableItemDB->LootTable_41_50_DT;
	}
	else if (this->LootTableOwner->GetEnemyLevel() >= 51 && this->LootTableOwner->GetEnemyLevel() <= 60)
	{
		LootTableLevel = this->LootTableItemDB->LootTable_51_60_DT;
	}
	else
	{

	}

	//Find item to generate
	float generated_rarity = FMath::RandRange(0.f, 100.f);

	if (generated_rarity < this->rarity_percent_A)
	{
		int32 ItemIndex = FMath::RandRange(0, LootTableLevel[0].LootTable_TierA[0].LootA.Num()); //Get number of items in this tier of items
		int i = 0;
		for (auto& it : LootTableLevel[0].LootTable_TierA[0].LootA)
		{
			if (i == ItemIndex)
			{
				this->ItemsToSpawn.Add(it.Value); //This is the item that was generate to return add to iteminfo items to return array
				break;
			}
			i++;

		}

	}
	else if (generated_rarity < this->rarity_percent_B)
	{
		int32 ItemIndex = FMath::RandRange(0, LootTableLevel[0].LootTable_TierB[0].LootA.Num()); //Get number of items in this tier of items
		int i = 0;
		for (auto& it : LootTableLevel[0].LootTable_TierB[0].LootA)
		{
			if (i == ItemIndex)
			{
				this->ItemsToSpawn.Add(it.Value); //This is the item that was generate to return add to iteminfo items to return array
				break;
			}
			i++;
		}

	}
	else if (generated_rarity < this->rarity_percent_C)
	{
		int32 ItemIndex = FMath::RandRange(0, LootTableLevel[0].LootTable_TierC[0].LootA.Num()); //Get number of items in this tier of items
		int i = 0;
		for (auto& it : LootTableLevel[0].LootTable_TierC[0].LootA)
		{
			if (i == ItemIndex)
			{
				this->ItemsToSpawn.Add(it.Value); //This is the item that was generate to return add to iteminfo items to return array
				break;
			}
			i++;
		}

	}
	else if (generated_rarity < this->rarity_percent_D)

	{
		int32 ItemIndex = FMath::RandRange(0, LootTableLevel[0].LootTable_TierD[0].LootA.Num()); //Get number of items in this tier of items
		int i = 0;
		for (auto& it : LootTableLevel[0].LootTable_TierD[0].LootA)
		{
			if (i == ItemIndex)
			{
				this->ItemsToSpawn.Add(it.Value); //This is the item that was generate to return add to iteminfo items to return array
				break;
			}
			i++;
		}

	}
	else if (generated_rarity < this->rarity_percent_E) //this should be the guranteed one always set to 100 or just remove or set to default
	{
		int32 ItemIndex = FMath::RandRange(0, LootTableLevel[0].LootTable_TierE[0].LootA.Num()); //Get number of items in this tier of items
		int i = 0;
		for (auto& it : LootTableLevel[0].LootTable_TierE[0].LootA)
		{
			if (i == ItemIndex)
			{
				this->ItemsToSpawn.Add(it.Value); //This is the item that was generate to return add to iteminfo items to return array
				break;
			}
			i++;
		}

	}
	else
	{

	}
}


void ULootTable::CalculateSpawnParams()
{

	//Find item to generate
	float generated_rarity = FMath::RandRange(0.f, 100.f);
	if (generated_rarity < this->rarity_percent_ItemSpawnNumberA)
	{
		this->NumberItemsToGenerate += 5;
	}
	else if (generated_rarity < this->rarity_percent_ItemSpawnNumberA)
	{
		this->NumberItemsToGenerate += 3;
	}
	else if (generated_rarity < this->rarity_percent_ItemSpawnNumberA)
	{
		this->NumberItemsToGenerate += 2;
	}
	else
	{

	}



}


