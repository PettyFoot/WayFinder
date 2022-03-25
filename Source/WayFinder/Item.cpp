// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "WayFinder.h"
#include "InventorySystem.h"
#include "WayFinderCharacter.h"



// Sets default values
AItem::AItem():
	ItemState(EItemState::EIS_Default),
	UseActionText(TEXT("Default Action Text")),
	ItemDisplayName(TEXT("Item Default Type")),
	ItemDescription(TEXT("Default Item Description")),
	ItemCurrentStack(1),
	ItemMaxStack(20),
	bCanBeStacked(true)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	this->ItemMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(this->ItemMeshComponent);
	//this->ItemMeshComponent

	//Item pick up widget
	this->ItemPickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	this->ItemPickupWidget->SetupAttachment(GetRootComponent());

	//Create item trace enabling collision sphere 
	this->ItemTraceEnableSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ItemTraceEnableSphere"));
	this->ItemTraceEnableSphere->SetupAttachment(GetRootComponent());
	//

	//Box collision for player pick up capabilities
	this->ItemPickupInteractArea = CreateDefaultSubobject<UBoxComponent>(TEXT("ItemPickupInteractArea"));
	this->ItemPickupInteractArea->SetupAttachment(GetRootComponent());
	

	//this->ItemTraceEnableSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	//this->SetItemState(EItemState::EIS_Constructed);
	
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	this->ItemPickupInteractArea->SetGenerateOverlapEvents(true);
	this->ItemTraceEnableSphere->SetGenerateOverlapEvents(true);

	this->ItemTraceEnableSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);


	this->ItemTraceEnableSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::ItemBeingOverlapped);
	this->ItemTraceEnableSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::ItemEndOverlap);

	//called to set item's initial state to be in world (overridden by spawn defualt weapon in player class)
	this->SetItemState(EItemState::EIS_InWorld);
	
}

void AItem::ItemBeingOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AWayFinderCharacter* player_overlapped = Cast<AWayFinderCharacter>(OtherActor);
		if (player_overlapped)
		{
			player_overlapped->AdjustOverlappedItems(1);
			UE_LOG(LogTemp, Warning, TEXT("Called item sphere overlap:::: %s"), *player_overlapped->GetName());
		}
		//UE_LOG(LogTemp, Warning, TEXT("Called item sphere overlap:::: %s"), *OtherActor->GetName());
	}

}

void AItem::ItemEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AWayFinderCharacter* player_overlapped = Cast<AWayFinderCharacter>(OtherActor);
		if (player_overlapped)
		{
			player_overlapped->AdjustOverlappedItems(-1);
			UE_LOG(LogTemp, Warning, TEXT("Called item sphere end overlap"));
		}
		//UE_LOG(LogTemp, Warning, TEXT("Called item sphere end overlap"));
	}
}
	

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItem::UseItem(AWayFinderCharacter* player)
{
}

void AItem::AddedToInventory(AWayFinderCharacter* player)
{
	if (player)
	{
		this->PlayerOwner = player;

		this->SetItemState(EItemState::EIS_InInventory);

	}
	else
	{
		this->Destroy();
	}
	
}

void AItem::SetItemState(EItemState state_to_set_to)
{
	this->ItemState = state_to_set_to;


	switch (state_to_set_to)
	{
	case EItemState::EIS_Constructed:

		//Turn off widget visibility
		this->ItemPickupWidget->SetVisibility(false);

		//Set item trace enabling collision sphere to overlap a player
		this->ItemTraceEnableSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		this->ItemTraceEnableSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		this->ItemTraceEnableSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		

		//Set up item pick up box to block player traces in the defined collision channel
		this->ItemPickupInteractArea->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		this->ItemPickupInteractArea->SetCollisionResponseToChannel(COLLISION_ITEM, ECollisionResponse::ECR_Block);
		this->ItemTraceEnableSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		break;
	case EItemState::EIS_InWorld:

		this->ItemMeshComponent->SetVisibility(true);//hide mesh
	
		//Turn off widget visibility
		this->ItemPickupWidget->SetVisibility(false);

		//Set item trace enabling collision sphere to overlap a player
		//this->ItemTraceEnableSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		this->ItemTraceEnableSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		this->ItemTraceEnableSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		//Set up item pick up box to block player traces in the defined collision channel
		this->ItemPickupInteractArea->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		this->ItemPickupInteractArea->SetCollisionResponseToChannel(COLLISION_ITEM, ECollisionResponse::ECR_Block);
		this->ItemTraceEnableSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		break;
	case EItemState::EIS_InInventory:

		this->ItemMeshComponent->SetVisibility(false);//hide mesh
		//this->ItemMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		//Turn off widget visibility
		this->ItemPickupWidget->SetVisibility(false);

		//Set item trace enabling collision sphere to overlap a player
		this->ItemTraceEnableSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		this->ItemTraceEnableSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		//Set up item pick up box to block player traces in the defined collision channel
		this->ItemPickupInteractArea->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		this->ItemPickupInteractArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		break;
	case EItemState::EIS_Equipped: 

		//Turn off widget visibility
		this->ItemMeshComponent->SetVisibility(true);

		this->ItemPickupWidget->SetVisibility(false);

		//Set item trace enabling collision sphere to overlap a player
		this->ItemTraceEnableSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		this->ItemTraceEnableSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		//Set up item pick up box to block player traces in the defined collision channel
		this->ItemPickupInteractArea->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		this->ItemPickupInteractArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		break;
	case EItemState::EIS_Equipping:

		//Turn off widget visibility
		this->ItemPickupWidget->SetVisibility(false);

		//Set item trace enabling collision sphere to overlap a player
		this->ItemTraceEnableSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		this->ItemTraceEnableSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		//Set up item pick up box to block player traces in the defined collision channel
		this->ItemPickupInteractArea->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		this->ItemPickupInteractArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		break;
	case EItemState::EIS_Attacking:

		break;
	case EItemState::EIS_Using:

		break;
	case EItemState::EIS_Default:

		break;
	default:
		break;
	}
}

void AItem::CopyItemStats(AItem* item_to_copy)
{
	if (item_to_copy)
	{
		this->ItemMaxStack = item_to_copy->ItemMaxStack; //Max stack
		this->ItemCurrentStack = item_to_copy->ItemCurrentStack; //Current stack
		
		this->ItemClass = item_to_copy->ItemClass; //Classes
		this->ItemSubClass = item_to_copy->ItemSubClass;

		this->UseActionText = item_to_copy->UseActionText; //Text Descriptions
		this->ItemDisplayName = item_to_copy->ItemDisplayName;
		this->ItemDescription = item_to_copy->ItemDescription;

		this->ItemImage = item_to_copy->ItemImage; //Image
		this->ItemWeight = item_to_copy->ItemWeight; //Weight

		this->bCanBeStacked = item_to_copy->bCanBeStacked;//Stackable

		this->PlayerOwner = item_to_copy->PlayerOwner;//Owners
		this->OwningInventory = item_to_copy->OwningInventory;
	}

}





