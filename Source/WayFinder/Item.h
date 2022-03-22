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

UENUM(BlueprintType)
enum class EItemClass : uint8
{
	IC_Weapon UMETA(DisplayName = "Weapon"),
	IC_Consumable UMETA(DisplayName = "Consumable"),
	IC_Armor UMETA(DisplayName = "Armor")
	
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

UCLASS()
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FText UseActionText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EItemClass ItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AItem> ItemSubClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FText ItemDisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FText ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UTexture2D* ItemImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	float ItemWeight;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	uint8 ItemCurrentStack;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	uint8 ItemMaxStack;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	uint8 ItemIndex;




	


};
