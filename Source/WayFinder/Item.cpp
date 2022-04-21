// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Curves/CurveFloat.h"
#include "Kismet/KismetMathLibrary.h"
#include "WayFinder.h"
#include "InventorySystem.h"
#include "ItemInfo.h"
#include "WayFinderCharacter.h"



// Sets default values
AItem::AItem():
	ItemState(EItemState::EIS_Default),
	UseActionText(TEXT("Default Action Text")),
	ItemDisplayName(TEXT("Item Default Type")),
	ItemDescription(TEXT("Default Item Description")),
	ItemCurrentStack(1),
	ItemMaxStack(20),
	bCanBeStacked(true),
	SpawnDropTimerTime(0.9f),
	SpawnDropLocationTarget(FVector(0.f)),
	SpawnDropStartLocation(FVector(0.f)),
	bIsSpawningDrop(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;



	this->ITemStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemStaticMeshComponent"));
	if (this->ItemStaticMesh)
	{
		this->ITemStaticMeshComponent->SetStaticMesh(this->ItemStaticMesh);
	}
	SetRootComponent(this->ITemStaticMeshComponent);


	this->ItemSkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemSkeletalMeshComponent"));
	this->ItemSkeletalMeshComponent->SetupAttachment(GetRootComponent());
	if (this->ItemSkeletalMesh)
	{
		this->ItemSkeletalMeshComponent->SetSkeletalMesh(this->ItemSkeletalMesh);
	}

	//Item pick up widget
	this->ItemPickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	this->ItemPickupWidget->SetupAttachment(GetRootComponent());

	//Create item trace enabling collision sphere 
	this->ItemTraceEnableSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ItemTraceEnableSphere"));
	this->ItemTraceEnableSphere->SetupAttachment(GetRootComponent());
	this->ItemTraceEnableSphere->SetSphereRadius(320.f);
	//



	//Box collision for player pick up capabilities
	this->ItemPickupInteractArea = CreateDefaultSubobject<UBoxComponent>(TEXT("ItemPickupInteractArea"));
	this->ItemPickupInteractArea->SetupAttachment(GetRootComponent());


	//this->ItemPickupInteractArea->Bounds.BoxExtent = 


	//this->ItemTraceEnableSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	//this->SetItemState(EItemState::EIS_Constructed);
	
}

void AItem::SpawnFromRarityTable(int32 level_to_spawn)
{
	this->SetItemState(EItemState::EIS_InWorld);

}

void AItem::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}


void AItem::InitWithItemInfo(FItemInfoStruct iteminfo)
{

	this->ItemStaticMesh = iteminfo.ITemStaticMesh;
	this->ItemSkeletalMesh = iteminfo.ItemSkeletalMesh;
	this->ItemSkeletalMeshComponent->SetSkeletalMesh(ItemSkeletalMesh);
	this->ITemStaticMeshComponent->SetStaticMesh(ItemStaticMesh);

	this->UseActionText = iteminfo.UseActionText;
	this->ItemClass = iteminfo.ItemClass;
	this->ItemSubClass = iteminfo.ItemSubClass;
	this->ItemDisplayName = iteminfo.ItemDisplayName;
	this->ItemDescription = iteminfo.ItemDescription;

	this->ItemImage = iteminfo.ItemImage;

	this->ItemWeight = iteminfo.ItemWeight;
	this->ItemCurrentStack = iteminfo.ItemCurrentStack;
	this->ItemMaxStack = iteminfo.ItemMaxStack;
	this->InventorySlotIndex = iteminfo.InventorySlotIndex;
	this->bCanBeStacked = iteminfo.bCanBeStacked;
	//this->ItemLevel = iteminfo.ItemLevel;

	this->SpawnDropCurve = iteminfo.SpawnDropCurve;
	this->SpawnDropCurve_Y = iteminfo.SpawnDropCurve_Y;
	this->SpawnDropLocationTarget = iteminfo.SpawnDropLocationTarget;
	this->PickupCollisionBoxExtent = iteminfo.PickupCollisionBoxExtent;
	this->PickupCollisionBoxOffset = iteminfo.PickupCollisionBoxOffset;

	//Set pickup box collision area
	this->ItemPickupInteractArea->SetBoxExtent(this->PickupCollisionBoxExtent);
	this->ItemPickupInteractArea->SetRelativeLocation(this->PickupCollisionBoxOffset);

	//Weapon Stuff
	this->MeleeType = iteminfo.WeaponInfoStruct.WeaponType.MeleeType;

	//Consumeable Stuff

}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	this->ItemPickupInteractArea->SetGenerateOverlapEvents(true);
	this->ItemTraceEnableSphere->SetGenerateOverlapEvents(true);

	this->ItemTraceEnableSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	//this->ItemSkeletalMeshComponent->SetSimulatePhysics(true);
	//this->ItemSkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	this->ITemStaticMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	this->ITemStaticMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
	this->ITemStaticMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	//this->ITemStaticMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	this->ITemStaticMeshComponent->SetSimulatePhysics(true);
	this->ITemStaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	this->ITemStaticMeshComponent->SetLinearDamping(2.f);


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

	if (this->bIsSpawningDrop) { this->SpawnDropItem(DeltaTime); }

}

void AItem::StartSpawnDropAnim(FVector start_location)
{
	this->bIsSpawningDrop = true;

	float range_Z = FMath::RandRange(120, 160);
	float range_X = FMath::RandRange(-250, 250);
	if (range_X < 100 && range_X > -100)
	{
		float range_xx = FMath::RandRange(100, 300);
		range_X = range_xx;
	}
	float range_Y = FMath::RandRange(-250, 250);
	if (range_Y < 100 && range_Y > -100)
	{
		float range_yy = FMath::RandRange(-300, -100);
		range_Y = range_yy;
	}

	float range_spin = FMath::RandRange(0, 360);
	FRotator spin = GetActorRotation() + FRotator(0.f, range_spin, 0.f);
	this->SpawnDropStartLocation = start_location;
	this->SpawnDropLocationTarget = GetActorLocation() + FVector(range_X, range_Y, range_Z);
	SetActorRotation(spin);
	GetWorldTimerManager().SetTimer(this->SpawnDropTimerHandle, this, &AItem::EndSpawnDrop, this->SpawnDropTimerTime);

}

void AItem::EndSpawnDrop()
{
	this->bIsSpawningDrop = false;
	this->SetItemState(EItemState::EIS_InWorld);
}

void AItem::SpawnDropItem(float DeltaTime)
{
	if (!this->bIsSpawningDrop) { return; }
	if (this->SpawnDropCurve && this->SpawnDropCurve_Y)
	{

		const float elapsed_time = GetWorldTimerManager().GetTimerElapsed(this->SpawnDropTimerHandle); //our timers current time
		const float curve_val = this->SpawnDropCurve->GetFloatValue(elapsed_time); //get curves return value for passed in time
		const float curve_val_y = this->SpawnDropCurve_Y->GetFloatValue(elapsed_time); //get curves return value for passed in time
		

		FVector item_loc = this->SpawnDropStartLocation;
		const FVector target_loc = this->SpawnDropLocationTarget;

		const FVector item_camera(FVector(0.f, 0.f, (target_loc - item_loc).Z));
		const FVector item_camera_y(FVector(0.f, (target_loc - item_loc).Y, 0.f));
		const FVector item_camera_x(FVector((target_loc - item_loc).X, 0.f, 0.f));
		
		
		float delta_z = item_camera.Size(); //Scale factor of curve to game world item
		float delta_y = item_camera_y.Size(); //Scale factor of curve to game world item
		float delta_x = item_camera_x.Size();
		if (item_camera.Z < 0)
		{
			delta_z = -delta_z;
		}
		if (item_camera_y.Y < 0)
		{
			delta_y = -delta_y;
		}
		if (item_camera_x.X < 0)
		{
			delta_x = -delta_x;
		}

		item_loc.Z += curve_val * delta_z;
		item_loc.Y += (curve_val_y) * delta_y;
		item_loc.X += (curve_val_y) * delta_x;
		SetActorLocation(item_loc, true, nullptr, ETeleportType::TeleportPhysics);
	}
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

		this->ItemSkeletalMeshComponent->SetVisibility(true);//show mesh
		this->ITemStaticMeshComponent->SetVisibility(true);//shows mesh
		this->ITemStaticMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
		this->ITemStaticMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
		this->ITemStaticMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	
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

		this->ItemSkeletalMeshComponent->SetVisibility(false);//hide mesh
		//this->ItemMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		this->ITemStaticMeshComponent->SetVisibility(false);//hide mesh
		this->ITemStaticMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

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
		this->ItemSkeletalMeshComponent->SetVisibility(true);
		this->ITemStaticMeshComponent->SetVisibility(true);//show mesh

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





