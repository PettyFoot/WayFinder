// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "Engine/DataAsset.h"
#include "ItemInfo.h"
#include "Item.h"
#include "LootTable.generated.h"

class ABaseEnemy;
class AItem;
class ABaseMeleeWeapon;
class AConsumable;
//enum class ELootTableAdjuster;


USTRUCT(BlueprintType)
struct FItemDropChances : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float rarity_percent_A_DT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float rarity_percent_B_DT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float rarity_percent_C_DT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float rarity_percent_D_DT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float rarity_percent_E_DT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float rarity_percent_ItemSpawnNumberA_DT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float rarity_percent_ItemSpawnNumberB_DT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float rarity_percent_ItemSpawnNumberC_DT;

};





UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WAYFINDER_API ULootTable : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULootTable();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LootTable Stats", meta = (AllowPrivateAccess = "true"))
	ABaseEnemy* LootTableOwner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UItemDatabase* LootTableItemDB;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FItemInfoStruct> ItemsToSpawn;

	UFUNCTION(BlueprintCallable)
	void SpawnLoot();

	UFUNCTION(BlueprintCallable)
	void GenerateItemsToDrop();

	UFUNCTION(BlueprintCallable)
	void ThrowItem(AItem* item_to_toss);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		AItem* spawned_item;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ABaseMeleeWeapon* spawned_weapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	AConsumable* spawned_consumable;


private:

	//PErcent chances for each tier of drops (based on enemies loot table chance severity enum modifier thingy)
	UPROPERTY(VisibleAnywhere)
	float rarity_percent_A;

	UPROPERTY(VisibleAnywhere)
	float rarity_percent_B;

	UPROPERTY(VisibleAnywhere)
	float rarity_percent_C;

	UPROPERTY(VisibleAnywhere)
	float rarity_percent_D;

	UPROPERTY(VisibleAnywhere)
	float rarity_percent_E;


	//PErcent chances for multiple item drops (based on enemies loot table chance severity enum modifier thingy)
	UPROPERTY(VisibleAnywhere)
	float rarity_percent_ItemSpawnNumberA;

	UPROPERTY(VisibleAnywhere)
	float rarity_percent_ItemSpawnNumberB;

	UPROPERTY(VisibleAnywhere)
	float rarity_percent_ItemSpawnNumberC;

	//Calculate number of items to generate
	void CalculateSpawnParams();

	//Number items to generate
	int32 NumberItemsToGenerate;

	//Mnin Number items to generate
	int32 MinNumberItemsToGenerate;

	//Item Database by level range index (TArray)
	int32 LootTable_Level;

	//Item Database by tier index (TArray)
	int32 LootTable_Tier;

	//Item database item index (TMap)
	int32 LootTable_Item;

private:

	void GenerateItem();

	//Get loot table rarity chances
	void InitRarityDropTableDT();

};
