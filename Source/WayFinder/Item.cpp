// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "WayFinder.h"
#include "WayFinderCharacter.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	this->ItemMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(this->ItemMeshComponent);
	//this->ItemMeshComponent

	//Item pick up widget
	this->ItemPickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	this->ItemPickupWidget->SetupAttachment(GetRootComponent());
	this->ItemPickupWidget->SetVisibility(false);

	//Create item trace enabling collision sphere 
	this->ItemTraceEnableSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ItemTraceEnableSphere"));
	this->ItemTraceEnableSphere->SetupAttachment(GetRootComponent());

	this->ItemTraceEnableSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	//this->ItemTraceEnableSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	this->ItemTraceEnableSphere->SetGenerateOverlapEvents(true);
	this->ItemTraceEnableSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	this->ItemTraceEnableSphere->SetCollisionResponseToChannel(COLLISION_PLAYER, ECollisionResponse::ECR_Overlap);

	//Box collision for player pick up capabilities
	this->ItemPickupInteractArea = CreateDefaultSubobject<UBoxComponent>(TEXT("ItemPickupInteractArea"));
	this->ItemPickupInteractArea->SetupAttachment(GetRootComponent());
	
	//this->ItemPickupInteractArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	this->ItemPickupInteractArea->SetGenerateOverlapEvents(true);
	this->ItemTraceEnableSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	this->ItemPickupInteractArea->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	this->ItemPickupInteractArea->SetCollisionResponseToChannel(COLLISION_ITEM, ECollisionResponse::ECR_Block); //in the player's custom traceforitems collision channel


}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	this->ItemTraceEnableSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::ItemBeingOverlapped);
	this->ItemTraceEnableSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::ItemEndOverlap);
	
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
		UE_LOG(LogTemp, Warning, TEXT("Called item sphere overlap:::: %s"), *OtherActor->GetName());
	}

}

void AItem::ItemEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AWayFinderCharacter* player_overlapped = Cast<AWayFinderCharacter>(OtherActor);
		if (player_overlapped)
		{
			player_overlapped->AdjustOverlappedItems(-2);
			UE_LOG(LogTemp, Warning, TEXT("Called item sphere end overlap"));
		}
		UE_LOG(LogTemp, Warning, TEXT("Called item sphere end overlap"));
	}
}
	

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



