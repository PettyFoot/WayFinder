// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Engine/Texture2D.h"
#include "Item.generated.h"


class USphereComponent;
class UWidgetComponent;
class USkeletalMeshComponent;
class UBoxComponent;
class AWayFinderCharacter;

UENUM(BlueprintType)
enum class EItemClass : uint8
{
	IC_Weapon UMETA(DisplayName = "Weapon"),
	IC_Consumable UMETA(DisplayName = "Consumable"),
	IC_Armor UMETA(DisplayName = "Armor"),
	IC_QuestItem UMETA(DisplayName = "Quest Item"),
	IC_Readable UMETA(DisplayName = "Readable")
	
};

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

USTRUCT(BlueprintType)
struct FItemInfo : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EItemClass DTItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<AItem> DTItemSubClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName DTItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture2D* DTItemImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		uint8 DTItemCurrentStack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		uint8 DTItemMaxStack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		uint8 DTItemIndex;


};

UCLASS(BlueprintType, Blueprintable)
class WAYFINDER_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

public:

	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return this->ItemPickupWidget; }

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* ItemPickupWidget;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ItemMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true", MultiLine = "true"))
	FString ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UTexture2D* ItemImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true", ClampMin = 0.f))
	float ItemWeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	uint8 ItemCurrentStack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	uint8 ItemMaxStack;

	//-1 if not inside an inventory (Default state!)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	int32 InventorySlotIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	bool bCanBeStacked;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item Owner", meta = (AllowPrivateAccess = "true"))
	AWayFinderCharacter* PlayerOwner;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EItemState ItemState;

	UPROPERTY()
	class UInventorySystem* OwningInventory;


public:

	virtual void UseItem(class AWayFinderCharacter* player);

	UFUNCTION(BlueprintImplementableEvent)
	void OnItemUse(class AWayFinderCharacter* player);

	//Called when item is added to inventory to destroy spawned item object
	//if player is not nullptr then item will be hidden and collision turned off (Make sure to override this in child classes to hide all their components too)
	virtual void AddedToInventory(AWayFinderCharacter* player);

	//always make sure to call this parent function in any derived class 
	virtual void SetItemState(EItemState state_to_set_to);

};
