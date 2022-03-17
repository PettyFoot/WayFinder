// Fill out your copyright notice in the Description page of Project Settings.



#include "BaseMeleeWeapon.h"
#include "BaseEnemy.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
#include "WayFinderCharacter.h"


// Sets default values
ABaseMeleeWeapon::ABaseMeleeWeapon():
	WeaponLevel(EWeaponLevel::WL_DefaultWeapon),
	bIsWaitingToApplyDamage(false),
	bShouldCollide(false),
	UltimateChargeCurrent(0)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create skeletal weapon mesh component 
	this->SkeletalWeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalWeaponMeshComponent"));
	SetRootComponent(this->SkeletalWeaponMeshComponent);


	//Create weapon collision box and attach to Skeletal weapon mesh component 
	this->WeaponCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollisionBox"));
	this->WeaponCollisionBox->SetupAttachment(this->SkeletalWeaponMeshComponent, FName("Collision_Socket"));

	this->WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	this->WeaponCollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	this->WeaponCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	this->WeaponCollisionBox->SetGenerateOverlapEvents(true);
	this->WeaponCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ABaseMeleeWeapon::OnWeaponOverlap);

	this->UltAbilityAOE = CreateDefaultSubobject<USphereComponent>(TEXT("WeaponUltimateAbilityAOE"));
	this->UltAbilityAOE->AttachTo(this->SkeletalWeaponMeshComponent, FName("FX_ult"));
	this->UltAbilityAOE->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	this->UltAbilityAOE->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	this->UltAbilityAOE->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	this->UltAbilityAOE->SetGenerateOverlapEvents(true);
	this->UltAbilityAOE->OnComponentBeginOverlap.AddDynamic(this, &ABaseMeleeWeapon::OnUltimateOverlap);
	

	//this->UltimateChargeCurrent = this->UltimateChargeMax;

	//this->bIs
	this->bReplicates = true;

}


void ABaseMeleeWeapon::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	this->InitWaveDataTable();
}

// Called when the game starts or when spawned
void ABaseMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseMeleeWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseMeleeWeapon::SetUltimateCharge(float adj_amount)
{
	if (this->UltimateChargeCurrent + adj_amount >= this->UltimateChargeMax)
	{
		this->UltimateChargeCurrent = this->UltimateChargeMax;
		UE_LOG(LogTemp, Warning, TEXT("You have reached max ult charge"));
		//Play sound cue

	}

	else
	{
		//Add ultimate charge adjustment to current ult, works well for subtracting ult charge too
		this->UltimateChargeCurrent += adj_amount;
	}
}

bool ABaseMeleeWeapon::CanUseUlt()
{
	if (this->UltimateChargeCurrent == this->UltimateChargeMax)
	{
		return true;
	}
	return false;
}

void ABaseMeleeWeapon::UseUlt()
{
	UE_LOG(LogTemp, Warning, TEXT("Called UseUlt for c++ "));
	this->UltimateChargeCurrent = 0;
	this->UltAbilityAOE->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ABaseMeleeWeapon::EndUlt()
{
	UE_LOG(LogTemp, Warning, TEXT("Called EndUlt for c++ "));
	this->UltAbilityAOE->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseMeleeWeapon::ToggleWeaponCollision(bool bShouldWeaponCollide)
{
	//Check fi collision box exists 
	if (this->WeaponCollisionBox)
	{
		if (bShouldWeaponCollide)
		{
			this->WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			this->bShouldCollide = true;
		}
		else
		{
			this->WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			this->bShouldCollide = false;
		}

	}
}

void ABaseMeleeWeapon::OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!this->bShouldCollide) { return; }

	AActor* overlapped_actor = nullptr;
	overlapped_actor = Cast<ABaseEnemy>(OtherActor);
	if (!overlapped_actor) 
	{
		
		overlapped_actor = Cast<AWayFinderCharacter>(OtherActor);
		if (!overlapped_actor) { return; }
	}
	
	
	if(overlapped_actor && overlapped_actor != GetOwner())
	{
		
		this->OverlapHitReturn = SweepResult;
		
		//UE_LOG(LogTemp, Warning, TEXT("ths is sever calling do damage"));
		UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *SweepResult.Actor->GetName());
		//UE_LOG(LogTemp, Warning, TEXT("Hit actor: %s"), *this->OverlapHitReturn.Actor->GetName);
	//	UE_LOG(LogTemp, Warning, TEXT("Hit Location X: %f"), this->OverlapHitReturn.Location.X);
	//	UE_LOG(LogTemp, Warning, TEXT("Hit Location Y: %f"), this->OverlapHitReturn.Location.Y);
		//UE_LOG(LogTemp, Warning, TEXT("Hit Location Z: %f"), this->OverlapHitReturn.Location.Z);\
		//UE_LOG(LogTemp, Warning, TEXT)
		this->bIsWaitingToApplyDamage = true;
		this->ActorOverlappedOnUse = overlapped_actor;
		FVector impact_particle_spawn_loc = this->SkeletalWeaponMeshComponent->GetSocketLocation(FName("weapon_spawn_impact_r"));
		if (this->ImpactParticles)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), this->ImpactParticles, impact_particle_spawn_loc);
		}
		//this->OverlapHitReturn = SweepResult;
		//DrawDebugSphere(GetWorld(), SweepResult.Actor->GetActorLocation(), 20, 16, FColor::Red, true, 5.f);
	}
}

void ABaseMeleeWeapon::OnUltimateOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	

	//The owner is an enemy
	ABaseEnemy* enemy_owner = Cast<ABaseEnemy>(this->GetOwner());
	if (enemy_owner)
	{
		AWayFinderCharacter* overlapped_player = Cast<AWayFinderCharacter>(OtherActor);
		if (!overlapped_player) { return; }
		overlapped_player->PlayerTakeDamage(this->UltChargeDamage);
		UE_LOG(LogTemp, Warning, TEXT("player found: %s"), *overlapped_player->GetName());
	}

	//The owner is an player
	AWayFinderCharacter* player_owner = Cast<AWayFinderCharacter>(this->GetOwner());
	if (player_owner)
	{
		ABaseEnemy* overlapped_Enemy = Cast<ABaseEnemy>(OtherActor);
		if (!overlapped_Enemy) { return; }

		overlapped_Enemy->EnemyTakeDamage(this->UltChargeDamage, player_owner);
		UE_LOG(LogTemp, Warning, TEXT("enemy found: %s"), *overlapped_Enemy->GetName());
	}

}

void ABaseMeleeWeapon::ToggleWeaponWaitingToApplyDamage(bool bIsWeaponWaitingToApplyDamage)
{
	this->bIsWaitingToApplyDamage = bIsWeaponWaitingToApplyDamage;
	this->ActorOverlappedOnUse = nullptr;
}



void ABaseMeleeWeapon::InitWaveDataTable()
{
	//Path to WaveStats data table 
	FString wave_stats_dt_path(TEXT("DataTable'/Game/Game/DataTables/DT_WeaponStatsDataTable.DT_WeaponStatsDataTable'"));

	UDataTable* dt_obj = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *wave_stats_dt_path));
	if (dt_obj)
	{
		UE_LOG(LogTemp, Warning, TEXT("data table found"));
		FWeaponStats* rarity_row = nullptr;

		switch (this->WeaponLevel)
		{
		case EWeaponLevel::WL_NoviceWeapon:
			rarity_row = dt_obj->FindRow<FWeaponStats>(FName("Novice"), TEXT(""));
			break;
		case EWeaponLevel::WL_ApprenticeWeapon:
			rarity_row = dt_obj->FindRow<FWeaponStats>(FName("Apprentice"), TEXT(""));
			break;
		case EWeaponLevel::WL_AdeptWeapon:
			rarity_row = dt_obj->FindRow<FWeaponStats>(FName("Adept"), TEXT(""));
			break;
		case EWeaponLevel::WL_MasterWeapon:
			rarity_row = dt_obj->FindRow<FWeaponStats>(FName("Master"), TEXT(""));
			break;
		case EWeaponLevel::WL_ExhaltedWeapon:
			rarity_row = dt_obj->FindRow<FWeaponStats>(FName("Exhalted"), TEXT(""));
			break;
		case EWeaponLevel::WL_LegendaryWeapon:
			rarity_row = dt_obj->FindRow<FWeaponStats>(FName("Legendary"), TEXT(""));
			break;
		case EWeaponLevel::WL_DefaultWeapon:
			rarity_row = dt_obj->FindRow<FWeaponStats>(FName("Default"), TEXT(""));
			break;
		default:
			break;
		}

		if (rarity_row)
		{
			this->BaseWeaponDamage = rarity_row->DTBaseWeaponDamage;
			this->CriticalDamageAdjustment = rarity_row->DTCriticalDamageAdjustment;
			this->StartingWeaponDurability = rarity_row->DTStartingWeaponDurability;
			this->UltimateChargeMax = rarity_row->DTUltimateChargeMax;
			this->ImpactParticles = rarity_row->DTImpactParticles;
			this->UltAbilityParticles = rarity_row->DTUltAbilityParticles;
			this->UltChargeDamage = rarity_row->DTUltChargeDamage;
			//this->StaticWeaponMeshComponent = rarity_row->DTStaticWeaponMeshComponent;
			//this->SkeletalWeaponMeshComponent = rarity_row->DTSkeletalWeaponMeshComponent;
		}

	}

}

