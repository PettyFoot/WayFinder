// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "ItemInfo.h"
#include "Item.generated.h"


class USphereComponent;
class UWidgetComponent;
class USkeletalMeshComponent;
class UBoxComponent;
class AWayFinderCharacter;
class UItemInfo;
struct FItemInfoStruct;



UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_Constructed,
	EIS_InWorld,
	EIS_InInventory,
	EIS_Equipped,
	EIS_Equipping,
	EIS_Attacking,
	EIS_Using,
	EIS_Default

};



UCLASS()
class WAYFINDER_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

	virtual void SpawnFromRarityTable(int32 level_to_spawn);

	virtual void OnConstruction(const FTransform& Transform) override;

public:

	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return this->ItemPickupWidget; }


	//Called from loot table to populate item with item info from enemies loot table
	//Sets item based on item info uobject (used for loot table item spawns)
	UFUNCTION(BlueprintCallable)
	virtual void InitWithItemInfo(FItemInfoStruct iteminfo);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void ItemBeingOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void ItemEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USphereComponent* ItemTraceEnableSphere;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* ItemPickupWidget;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ItemSkeletalMeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USkeletalMesh* ItemSkeletalMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ITemStaticMeshComponent;

	UStaticMesh* ItemStaticMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* ItemPickupInteractArea;


public:

	UPROPERTY(Transient)
	class UWorld* World;

	//virtual class UWorld* GetWorld() const { return this->World; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FString UseActionText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EItemClass ItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AItem> ItemSubClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FString ItemDisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FString ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UTexture2D* ItemImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	float ItemWeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	uint8 ItemCurrentStack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	uint8 ItemMaxStack;

	//-1 if not inside an inventory (Default state!)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	int32 InventorySlotIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	bool bCanBeStacked;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item Owner", meta = (AllowPrivateAccess = "true"))
	AWayFinderCharacter* PlayerOwner;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EItemState ItemState;

	UPROPERTY()
	class UInventorySystem* OwningInventory;

	//Used to set certain aspects of the item's capabilities
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	int32 ItemLevel;


public:

	
	virtual void UseItem(class AWayFinderCharacter* player);

	UFUNCTION(BlueprintImplementableEvent)
	void OnItemUse(class AWayFinderCharacter* player);

	//Called when item is added to inventory to destroy spawned item object
	//if player is not nullptr then item will be hidden and collision turned off (Make sure to override this in child classes to hide all their components too)
	virtual void AddedToInventory(AWayFinderCharacter* player);

	//always make sure to call this parent function in any derived class 
	virtual void SetItemState(EItemState state_to_set_to);

	void CopyItemStats(AItem* item_to_copy);



};
