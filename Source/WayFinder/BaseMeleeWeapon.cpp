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
#include "WayFinderHealthComponent.h"



// Sets default values
ABaseMeleeWeapon::ABaseMeleeWeapon():
	WeaponLevel(EWeaponLevel::WL_DefaultWeapon),
	bIsWaitingToApplyDamage(false),
	bShouldCollide(false),
	UltimateChargeCurrent(0),
	BaseWeaponDamage(1.f),
	CriticalDamageAdjustment(1.5f),
	StartingWeaponDurability(100.f),
	DurabilityLossRate(0.01f),
	DurabilityLossPerUse(0.f),
	UltChargeRadius(100.f),
	UltChargeDamage(20.f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Set durability loss per use to this
	this->DurabilityLossPerUse = this->StartingWeaponDurability * this->DurabilityLossRate;

	//Create weapon collision box and attach to Skeletal weapon mesh component 
	this->WeaponCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollisionBox"));
	this->WeaponCollisionBox->SetupAttachment(GetRootComponent());
	//TODO may need to send in weapon collision box size with item spawn data to set collision box correctly
	

	this->UltAbilityAOE = CreateDefaultSubobject<USphereComponent>(TEXT("WeaponUltimateAbilityAOE"));
	this->UltAbilityAOE->AttachTo(GetRootComponent(), FName("FX_ult"));

	this->bReplicates = true;

}


void ABaseMeleeWeapon::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

}

// Called when the game starts or when spawned
void ABaseMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();
	//this->InitWaveDataTable();

	//Ult Ability sphere collision set up
	this->UltAbilityAOE->SetGenerateOverlapEvents(true);
	this->UltAbilityAOE->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	this->UltAbilityAOE->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	this->UltAbilityAOE->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	

	//Weapon collision collision set up
	this->WeaponCollisionBox->SetGenerateOverlapEvents(true);
	this->WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	this->WeaponCollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	this->WeaponCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);


	this->WeaponCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ABaseMeleeWeapon::OnWeaponOverlap);
	this->UltAbilityAOE->OnComponentBeginOverlap.AddDynamic(this, &ABaseMeleeWeapon::OnUltimateOverlap);
}

// Called every frame
void ABaseMeleeWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseMeleeWeapon::InitWithItemInfo(FItemInfoStruct iteminfo)
{
	Super::InitWithItemInfo(iteminfo);

	int32 weapon_level = FMath::RandRange(0, 6); //This needs to be updated to be more RNG compliable
	switch (weapon_level)
	{
	case 0:
		this->WeaponLevel = EWeaponLevel::WL_NoviceWeapon;
		break;
	case 1:
		this->WeaponLevel = EWeaponLevel::WL_ApprenticeWeapon;
		break;
	case 2:
		this->WeaponLevel = EWeaponLevel::WL_AdeptWeapon;
		break;
	case 3:
		this->WeaponLevel = EWeaponLevel::WL_MasterWeapon;
		break;
	case 4:
		this->WeaponLevel = EWeaponLevel::WL_ExhaltedWeapon;
		break;
	case 5:
		this->WeaponLevel = EWeaponLevel::WL_LegendaryWeapon;
		break;
	default:
		this->WeaponLevel = EWeaponLevel::WL_DefaultWeapon;
		break;
	}

	//Set initial stats
	this->BaseWeaponDamage = iteminfo.WeaponInfoStruct.BaseWeaponDamage * this->ItemLevel; //Base damage times player level (Will have to balance this with player health, should do player health based off level)
	this->CriticalDamageAdjustment = iteminfo.WeaponInfoStruct.CriticalDamageAdjustment;
	this->StartingWeaponDurability = iteminfo.WeaponInfoStruct.StartingWeaponDurability;
	this->DurabilityLossRate = iteminfo.WeaponInfoStruct.DurabilityLossRate;
	this->UltChargeRate = iteminfo.WeaponInfoStruct.UltChargeRate;
	this->DurabilityLossPerUse = this->StartingWeaponDurability * this->DurabilityLossRate;
	this->ImpactParticles = iteminfo.WeaponInfoStruct.DTImpactParticles;
	this->UltAbilityParticles = iteminfo.WeaponInfoStruct.DTUltAbilityParticles;
	this->UltChargeRadius = iteminfo.WeaponInfoStruct.UltChargeRadius;
	this->UltChargeDamage = iteminfo.WeaponInfoStruct.UltChargeDamage;

	

	//This will update current weapon data from weapon level data table based on now current weapon level
	//Will adjust various stats on the weapon
	this->UpdateWaveTableDate();

	//Call Data table which adjusts weapon stats based of set weapon level
	// Novice = Worst --- Legendary = Best (Default is issue)
	//TODO make default assert or make sure there is a weapon level set

	
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

	//See if weapon owner is a player character
	AWayFinderCharacter* player_weapon_owner = Cast<AWayFinderCharacter>(GetOwner());
	if (player_weapon_owner)
	{
		AWayFinderCharacter* player_hit = Cast<AWayFinderCharacter>(OtherActor);
		//this is an enemy attacking another enemy, don't allow return
		if (player_hit) { return; }

		ABaseEnemy* enemy_hit = Cast<ABaseEnemy>(OtherActor);
		if (enemy_hit)
		{
			//Gets this weapons base damage and adds to wielders weapon damage adjustments (buffs, consumable effect etc.)
			float crit_chance = FMath::RandRange(0, 100);
			float damage_amount;
			if (crit_chance < 50)
			{
				damage_amount = this->BaseWeaponDamage;
			}
			else
			{

				damage_amount = this->BaseWeaponDamage * this->CriticalDamageAdjustment;
			}
			//TODO Implement get surface type to spawn the corresponding fx (hit rock vs. hit flesh) ---- don't currently have many assets for this

			//Get player to damage current health
			float cur_health = enemy_hit->GetEnemyHealthComponent()->GetCurrentHealth();
			enemy_hit->EnemyTakeDamage(damage_amount, GetOwner()); //Damage enemy set damage causer to this weapon's wielder (player character) __ for AI BT
			//check if player has been damaged
			if (cur_health > enemy_hit->GetEnemyHealthComponent()->GetCurrentHealth())
			{
				//player damaged - play hit fx
				this->PlayWeaponHitFX();
			}
		}
	}

	//See if weapon owenr is an enemy and is not self
	ABaseEnemy* enemy_weapon_owner = Cast<ABaseEnemy>(GetOwner());
	if (enemy_weapon_owner)
	{
		ABaseEnemy* enemy_hit = Cast<ABaseEnemy>(OtherActor);
		//this is an enemy attacking another enemy, don't allow return
		if (enemy_hit) { return; }

		AWayFinderCharacter* player_hit = Cast<AWayFinderCharacter>(OtherActor);
		if (player_hit)
		{
			float crit_chance = FMath::RandRange(0, 100);
			float damage_amount;
			if (crit_chance < 50)
			{
				damage_amount = this->BaseWeaponDamage;
			}
			else
			{
				damage_amount = this->BaseWeaponDamage * this->CriticalDamageAdjustment;
			}
			
			//TODO Implement get surface type to spawn the corresponding fx (hit rock vs. hit flesh) ---- don't currently have many assets for this

			//Get player to damage current health
			float cur_health = player_hit->GetPlayerHealthComponent()->GetCurrentHealth();
			player_hit->PlayerTakeDamage(damage_amount);
			//check if player has been damaged
			if (cur_health > player_hit->GetPlayerHealthComponent()->GetCurrentHealth())
			{
				//player damaged - play hit fx
				this->PlayWeaponHitFX();
			}
		}
	}
	
		//UE_LOG(LogTemp, Warning, TEXT("ths is sever calling do damage"));
		//UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *SweepResult.Actor->GetName());
		//UE_LOG(LogTemp, Warning, TEXT("Hit actor: %s"), *this->OverlapHitReturn.Actor->GetName);
		//	UE_LOG(LogTemp, Warning, TEXT("Hit Location X: %f"), this->OverlapHitReturn.Location.X);
		//	UE_LOG(LogTemp, Warning, TEXT("Hit Location Y: %f"), this->OverlapHitReturn.Location.Y);
		//UE_LOG(LogTemp, Warning, TEXT("Hit Location Z: %f"), this->OverlapHitReturn.Location.Z);\
		//UE_LOG(LogTemp, Warning, TEXT)
		//this->bIsWaitingToApplyDamage = true;
		//this->ActorOverlappedOnUse = overlapped_actor;
		//this->OverlapHitReturn = SweepResult;
		//DrawDebugSphere(GetWorld(), SweepResult.Actor->GetActorLocation(), 20, 16, FColor::Red, true, 5.f);

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

void ABaseMeleeWeapon::UseItem(AWayFinderCharacter* player)
{
	if (player)
	{
		player->EquipItem(this);
	}
}

void ABaseMeleeWeapon::PlayWeaponHitFX()
{
	//Spawn weapon hit particles
	FVector impact_particle_spawn_loc = this->ItemSkeletalMeshComponent->GetSocketLocation(FName("weapon_spawn_impact_r"));
	if (this->ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), this->ImpactParticles, impact_particle_spawn_loc);
	}


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
			
		}

	}

}

void ABaseMeleeWeapon::UpdateWaveTableDate()
{//Path to WaveStats data table 
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

		/*UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float DTBaseWeaponDamageMultiplier;

	//Adjustment to damage if critical strike is landed 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1", ClampMax = "10"))
		float DTCriticalDamageMultiplier;

	//smaller value makes weapon deteriorate slower
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", ClampMax = "1"))
		float DTWeaponDurabilityLossRate;

	//smaller value is less ult charge gain
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.5", ClampMax = "2"))
	float DTUltimateChargeGainRate;

	//smaller value is less damage on ult (use 5 for mega impact? will have to play test this value)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.5", ClampMax = "5"))
	float DTUltChargeDamageMultiplier;*/

		if (rarity_row)
		{
			this->BaseWeaponDamage *= rarity_row->DTBaseWeaponDamageMultiplier;
			this->CriticalDamageAdjustment += rarity_row->DTCriticalDamageMultiplier;
			this->DurabilityLossRate -= rarity_row->DTWeaponDurabilityLossRate; 
			this->UltChargeRate += rarity_row->DTUltimateChargeGainRate;
			this->UltChargeDamage *= rarity_row->DTUltChargeDamageMultiplier;

			this->DurabilityLossPerUse = this->StartingWeaponDurability * this->DurabilityLossRate;
		}

	}
}

