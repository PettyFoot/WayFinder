// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemy.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "BaseMeleeWeapon.h"
#include "BaseEnemyControllerAI.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h" 
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "LootTable.h"
#include "LevelSystem.h"
#include "Item.h"
#include "Consumable.h"
#include "BaseMeleeWeapon.h"
#include "Weapon.h"

#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "WayFinderHealthComponent.h"
#include "WayFinderCharacter.h"
#include "WayFinderGameMode.h"

ABaseEnemy::ABaseEnemy():
	PatrolPointOne(FVector(0.f)),
	PatrolPointTwo(FVector(0.f)),
	bIsInvulvernable(false),
	bEventTriggered(false),
	bIsEnemyDead(false),
	DeathTimerTime(5.f),
	PatrolPointFive(FVector(0.f)),
	PatrolPointSix(FVector(0.f))
{
	EnemyHealthComponent = CreateDefaultSubobject<UWayFinderHealthComponent>(TEXT("HealthComponent"));

	this->EnemyLootTable = CreateDefaultSubobject<ULootTable>(TEXT("LootTable"));
	this->EnemyLootTable->LootTableOwner = this;


	this->EventEnableCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("EventCollisionBox"));
	this->EventEnableCollisionBox->SetupAttachment(GetRootComponent());
	this->EventEnableCollisionBox->SetGenerateOverlapEvents(true);
	this->EventEnableCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	this->EventEnableCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	this->bReplicates = true;

	PrimaryActorTick.bCanEverTick = true;
}

void ABaseEnemy::StartBehaviorTree()
{
	if (this->EnemyController)
	{
		//Convert local PatrolPoint to global vector location 
		//Crude Patrol path set up
		FVector world_patrol_point = UKismetMathLibrary::TransformLocation(GetActorTransform(), this->PatrolPointOne);
		FVector world_patrol_pointtwo = UKismetMathLibrary::TransformLocation(GetActorTransform(), this->PatrolPointTwo);
		FVector world_patrol_pointthree = UKismetMathLibrary::TransformLocation(GetActorTransform(), this->PatrolPointThree);
		FVector world_patrol_pointfour = UKismetMathLibrary::TransformLocation(GetActorTransform(), this->PatrolPointFour);
		FVector world_patrol_pointfive = world_patrol_pointfour;
		FVector world_patrol_pointsix = world_patrol_pointfour;
		//If 5 and 6 have been set change their value
		if (this->PatrolPointFive != FVector(0.f))
		{
			world_patrol_pointfive = UKismetMathLibrary::TransformLocation(GetActorTransform(), this->PatrolPointFive);
			world_patrol_pointsix = UKismetMathLibrary::TransformLocation(GetActorTransform(), this->PatrolPointSix);
		}

		//Assign targetlocation with global patrol point
		this->EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPointOne"), world_patrol_point);
		this->EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPointTwo"), world_patrol_pointtwo);
		this->EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPointThree"), world_patrol_pointthree);
		this->EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPointFour"), world_patrol_pointfour);
		this->EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPointFive"), world_patrol_pointfive);
		this->EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPointSix"), world_patrol_pointsix);
		this->EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), GetActorLocation());


		this->EnemyController->RunBehaviorTree(this->EnemyBehaviorTree);
	}
}

void ABaseEnemy::BeginPlay()
{ 
	Super::BeginPlay();

	this->EnemyHealthComponent->SetHealthComponentOwner(this);

	this->EnemyController = Cast<ABaseEnemyControllerAI>(GetController());

	//Subscribe OverlappedEventCollisionBox to eventcollisionbox's overlap events
	this->EventEnableCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ABaseEnemy::OverlappedEventCollisionBox);
	this->EventEnableCollisionBox->OnComponentEndOverlap.AddDynamic(this, &ABaseEnemy::EndOverlapEventCollisionsBox);

	this->StartBehaviorTree();

	this->EnemyGameMode = Cast<AWayFinderGameMode>(GetWorld()->GetAuthGameMode());
	if (EnemyGameMode)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found game mode on ememy"));
	}
	

	this->SpawnDefaultMeleeWeapon();

}

void ABaseEnemy::OverlappedEventCollisionBox(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Save on processing
	if (bEventTriggered) { return; }

	AWayFinderCharacter* player = Cast<AWayFinderCharacter>(OtherActor);
	if (player)
	{
		//Player has entered the eventcollisionbox, enable event begin
		this->bEventTriggered = true;
	}
}

void ABaseEnemy::EndOverlapEventCollisionsBox(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!bEventTriggered) { return; }

	AWayFinderCharacter* player = Cast<AWayFinderCharacter>(OtherActor);
	if (player)
	{
		//Player has exited collision box, pause the event. This would break if more than one play inside collision box and left
		this->bEventTriggered = false;
	}
}

void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (this->AttackingPlayer)
	{
		/*FRotator damage_causer_rot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), this->AttackingPlayer->GetActorLocation());
		FRotator player_rot = GetActorRotation();
		FRotator difference = damage_causer_rot - player_rot;
		FVector rot_dif = difference.Vector();
		FVector degrees = difference.Euler();

		this->AttackerPositionAround = degrees.Z;

		UE_LOG(LogTemp, Warning, TEXT("rot_dif X: %f"), rot_dif.X);
		UE_LOG(LogTemp, Warning, TEXT("rot_dif Y: %f"), rot_dif.Y);
		UE_LOG(LogTemp, Warning, TEXT("rot_dif Z: %f"), rot_dif.Z);
		UE_LOG(LogTemp, Warning, TEXT("degrees X: %f"), degrees.X);
		UE_LOG(LogTemp, Warning, TEXT("degrees Y: %f"), degrees.Y);
		UE_LOG(LogTemp, Warning, TEXT("degrees Z: %f"), degrees.Z);*/
	}
	
}

void ABaseEnemy::EnemyTakeDamage(float DamageAmount, AActor* damage_causer)
{
	//enemy is invulnerable return and don't apply damage
	if (this->bIsInvulvernable) { return; }

	this->EnemyHealthComponent->HealthTakeDamage(DamageAmount);

	if (damage_causer)
	{
		UE_LOG(LogTemp, Warning, TEXT("Damage Causer: %s"), *damage_causer->GetName());
		//Attempt to cast damage_causer to wayfinderchar to ensure it's a player damaging, could set this up to respond to other actors differently
		this->AttackingPlayer = Cast<AWayFinderCharacter>(damage_causer);
		if (this->AttackingPlayer)
		{
			this->ShowHealthBar();
			UE_LOG(LogTemp, Warning, TEXT("Attempting To play hit react"));
			this->PlayHitReact();
		}
	}
}

void ABaseEnemy::EnemyHasDied()
{
	if (this->EnemyHealthComponent->GetCurrentHealth() <= 0.f)
	{
		//
		if (this->EnemyController)
		{
			this->EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("IsDead"), true);
		}
		if (this->DeathMontage && !this->bIsEnemyDead)
		{
			UAnimInstance* anim_inst = GetMesh()->GetAnimInstance();
			if (anim_inst)
			{
				UE_LOG(LogTemp, Warning, TEXT("Calling montage of death "));
				anim_inst->Montage_Play(this->DeathMontage);
				//GetWorldTimerManager().SetTimer(this->DeathTimerHandle, this, &ABaseEnemy::Die, this->DeathTimerTime);
			}
		}
		this->bIsEnemyDead = true;
	}
	
}

void ABaseEnemy::ToggleEnemyInvulnverability(bool bShouldEnemyBeInvulnerable)
{
	this->bIsInvulvernable = bShouldEnemyBeInvulnerable;
}

float ABaseEnemy::GetEnemyWeaponDamageAdjustments()
{
	return 0.0f;
}

EItemClass ABaseEnemy::SpawnItem(FItemInfoStruct iteminfo)
{
	FActorSpawnParameters SpawnParams = FActorSpawnParameters();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	float RandomSpawnPlacement = FMath::RandRange(-100, 100);
	float RandomSpawnPlacementtwo = FMath::RandRange(-100, 100);

	FVector spawnadjust(RandomSpawnPlacement, RandomSpawnPlacementtwo, 0.f);

	int32 generated_rarity = FMath::RandRange(GetEnemyLevel() - 2, GetEnemyLevel() + 2);

	if (iteminfo.ItemClass == EItemClass::IC_Armor)
	{
		//TODO
	}
	else if (iteminfo.ItemClass == EItemClass::IC_Consumable)
	{
		//Spawn item
		
		AConsumable* consume = this->GetWorld()->SpawnActor<AConsumable>(AConsumable::StaticClass(), GetActorLocation() + spawnadjust,
			GetActorRotation(), SpawnParams);
		//spawned_weapon_weakptr_consumable = MakeShareable(consume);
		if (IsValid(consume))
		{
			//consume->AddToRoot();
			//Point member
			this->spawned_consumable = consume;
			//this->spawned_consumable->AddToRoot();
		}

		//this->spawned_consumable->SetOwner(this->LootTableOwner);
		if (spawned_consumable)
		{
			spawned_consumable->ItemLevel = generated_rarity;
			spawned_consumable->InitWithItemInfo(iteminfo);
			//spawned_consumable->SetLifeSpan(100000000.f);
			//spawned_consumable->SetActorLocation(this->LootTableOwner->GetActorLocation());
			//this->EnemyLootTable->ThrowItem(spawned_consumable);

			spawned_consumable->StartSpawnDropAnim(GetActorLocation() + spawnadjust); //plays "drop anim"
			//spawned_consumable->SetItemState(EItemState::EIS_InWorld);
			
			this->EnemyGameMode->SpawnedLoot.Add(consume);
			UE_LOG(LogTemp, Error, TEXT("Spawned Item__Consumable: %s"), *consume->ItemDisplayName);
		}
		return EItemClass::IC_Consumable;

	}
	else if (iteminfo.ItemClass == EItemClass::IC_QuestItem)
	{
		//TODO
	}
	else if (iteminfo.ItemClass == EItemClass::IC_Weapon)
	{
		
		ABaseMeleeWeapon* weapon = this->GetWorld()->SpawnActor<ABaseMeleeWeapon>(ABaseMeleeWeapon::StaticClass(), GetActorLocation() + spawnadjust,
			GetActorRotation(), SpawnParams);
		//this->spawned_weapon_weakptr_weapon = MakeShareable(weapon);
		
		if (IsValid(weapon))
		{
			//weapon->AddToRoot();
			//Point member
			weapon->SetOwner(this);
			this->spawned_weapon = weapon;
			//this->spawned_weapon->AddToRoot();
		}
	
		if (this->spawned_weapon)
		{
			weapon->ItemLevel = generated_rarity;
			weapon->InitWithItemInfo(iteminfo);
			//spawned_weapon->SetLifeSpan(100000000.f);
			spawned_weapon->StartSpawnDropAnim(GetActorLocation() + spawnadjust); //plays "drop anim"
			//this->EnemyLootTable->ThrowItem(weapon);
			//weapon->SetItemState(EItemState::EIS_InWorld);
			this->EnemyGameMode->SpawnedLoot.Add(weapon);
			UE_LOG(LogTemp, Error, TEXT("Spawned Item__Weapon: %s"), *weapon->ItemDisplayName);
		}
		return EItemClass::IC_Weapon;
	}
	else if (iteminfo.ItemClass == EItemClass::IC_Readable)
	{
		//TODO
	}
	else
	{
		//Spawn item
		
		AItem* item = this->GetWorld()->SpawnActor<AItem>(AItem::StaticClass(), GetActorLocation() + spawnadjust,
			GetActorRotation(), SpawnParams);
		//	this->spawned_weapon_weakptr_item = MakeShareable(item);
		if (IsValid(item))
		{
			//Point member
			this->spawned_item = item;
			//this->spawned_item->AddToRoot();
		}
		if (spawned_item)
		{
			spawned_item->InitWithItemInfo(iteminfo);
			//spawned_item->SetLifeSpan(100000000.f); //Attempt to sett lifetime
			//this->EnemyLootTable->ThrowItem(spawned_item);
			spawned_weapon->StartSpawnDropAnim(GetActorLocation() + spawnadjust); //plays "drop anim"
			//spawned_item->SetItemState(EItemState::EIS_InWorld); //Item state set to inworld, for pickups
			spawned_item->ItemLevel = generated_rarity; //Set item level pseudo randomly
			this->EnemyGameMode->SpawnedLoot.Add(spawned_item); //Add item to gamemode item array
			UE_LOG(LogTemp, Error, TEXT("Spawned Item__Base: %s"), *spawned_item->ItemDisplayName); //debug log
		}
		return EItemClass::IC_Default;
	}
	return EItemClass::IC_Default;
}



void ABaseEnemy::EnemyAttack()
{
	if (this->EnemyAttackMontage)
	{
		UAnimInstance* anim_inst = GetMesh()->GetAnimInstance();
		if (anim_inst)
		{
			anim_inst->Montage_Play(this->EnemyAttackMontage);
			//GetWorldTimerManager().SetTimer(this->DeathTimerHandle, this, &ABaseEnemy::Die, this->DeathTimerTime);
		}
	}
}

void ABaseEnemy::EnableWeaponCollision()
{
	if (this->EnemyEquippedMeleeWeapon)
	{
		UE_LOG(LogTemp, Error, TEXT("Weapon collision enabled"));

		this->EnemyEquippedMeleeWeapon->ToggleWeaponCollision(true);
	}
}

void ABaseEnemy::DisableWeaponCollision()
{
	if (!this->EnemyEquippedMeleeWeapon) { return; }
	AWayFinderCharacter* player_hit = Cast<AWayFinderCharacter>(this->EnemyEquippedMeleeWeapon->GetActorOverlappedOnUse());
	if (player_hit)
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponReturned valid enemy to damage"));
		if (player_hit->GetPlayerHealthComponent()->GetCurrentHealth() > 0)
		{
			player_hit->PlayerTakeDamage(this->EnemyEquippedMeleeWeapon->GetBaseWeaponDamage());
		}
		//enemy_hit->TakeDamage(this->PlayerEquippedMeleeWeapon->GetBaseWeaponDamage(), FDamageEvent, GetController(), this);
		//reset overlap enemy actor trigger
		this->EnemyEquippedMeleeWeapon->ToggleWeaponWaitingToApplyDamage(false);
	}
	
	this->EnemyEquippedMeleeWeapon->ToggleWeaponCollision(false);
}

void ABaseEnemy::EnableBossParticles()
{
	if (this->BossParticleOne)
	{
		
		//FVector socket_loc = GetMesh()->GetSocketLocation(FName("FX_Shoulder_l"));
		this->ParticlesOne = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), this->BossParticleOne, GetActorTransform());
		this->ParticlesOne->AttachTo(GetMesh(), FName("FX_Shoulder_l"), EAttachLocation::SnapToTarget);
		//GetMesh()->GetSocketByName(FName("FX_Shoulder_l"))
	}

	if (this->BossParticleTwo)
	{
		//this->ParticlesTwo->AttachTo(GetMesh(), FName("FX_Shoulder_r"), EAttachLocation::SnapToTarget);
		//this->ParticlesTwo = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), this->BossParticleTwo, GetActorTransform());
		UGameplayStatics::SpawnEmitterAttached(this->BossParticleTwo, GetMesh(), FName(TEXT("FX_Shoulder_r")));
		//this->
		
		//GetMesh()->GetSocketByName(FName("FX_Shoulder_l"))
	}
}

void ABaseEnemy::ShowHealthBar_Implementation()
{
	GetWorldTimerManager().ClearTimer(this->ShowHealthBarHandle);
	GetWorldTimerManager().SetTimer(this->ShowHealthBarHandle, this, &ABaseEnemy::HideHealthBar, this->ShowHealthBarTimerTime);
}




void ABaseEnemy::Die()
{
	//GetController()->PawnPendingDestroy(this);
	//Play death anims here
	if (this->EnemyEquippedMeleeWeapon)
	{
		this->EnemyEquippedMeleeWeapon->Destroy();
	}
	this->Destroy();

}

void ABaseEnemy::PlayHitReact()
{
	if (this->bIsEnemyDead) { return; }
	FRotator damage_causer_rot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), this->AttackingPlayer->GetActorLocation());
	FRotator player_rot = GetActorRotation();
	FRotator difference = damage_causer_rot - player_rot;
	FVector rot_dif = difference.Vector();
	FVector degrees = difference.Euler();

	this->AttackerPositionAround = degrees.Z;

	UE_LOG(LogTemp, Warning, TEXT("rot_dif X: %f"), rot_dif.X);
	UE_LOG(LogTemp, Warning, TEXT("rot_dif Y: %f"), rot_dif.Y);
	UE_LOG(LogTemp, Warning, TEXT("rot_dif Z: %f"), rot_dif.Z);
	UE_LOG(LogTemp, Warning, TEXT("degrees X: %f"), degrees.X);
	UE_LOG(LogTemp, Warning, TEXT("degrees Y: %f"), degrees.Y);
	UE_LOG(LogTemp, Warning, TEXT("degrees Z: %f"), degrees.Z);
	
	FName hitreactsection(TEXT("HitReactFront"));

	//Front 
	if (this->AttackerPositionAround >= -50.f && this->AttackerPositionAround < 50.f)
	{
		hitreactsection = TEXT("HitReactFront");
		UE_LOG(LogTemp, Error, TEXT("HitReactFront"));
	}
	//Right
	else if (this->AttackerPositionAround >= 50.f && this->AttackerPositionAround < 120.f)
	{
		hitreactsection = TEXT("HitReactRight");
		UE_LOG(LogTemp, Error, TEXT("HitReactRight"));
	}
	//Back
	else if (this->AttackerPositionAround >= 120.f || this->AttackerPositionAround < -125.f)
	{
		hitreactsection = TEXT("HitReactBack");
		UE_LOG(LogTemp, Error, TEXT("HitReactBack"));
	}
	//Left
	else if (this->AttackerPositionAround >= -115.f && this->AttackerPositionAround < -50.f)
	{
		hitreactsection = TEXT("HitReactLeft");
		UE_LOG(LogTemp, Error, TEXT("HitReactLeft"));
	}

	//Error
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SOMETHING WENT TERRIBLY WRONG AND THERE WAS NO SUITABLE POSITON FOUND FOR A HIT REACT MONTAGE_______________ABASEENEMY::PLAYHITREACT()"));
	}

	if (this->HitReactMontage)
	{
		UAnimInstance* anim_inst = GetMesh()->GetAnimInstance();
		if (anim_inst)
		{
			anim_inst->Montage_Play(this->HitReactMontage);
			anim_inst->Montage_JumpToSection(hitreactsection, this->HitReactMontage);
			//Need to set section to jump to here  based on direction of hit 
		}
	}



}

void ABaseEnemy::SpawnDefaultMeleeWeapon()
{
	if (this->MeleeWeaponClass)
	{
		//Spawn a basemeleeweapon
		ABaseMeleeWeapon* MeleeWeapon = GetWorld()->SpawnActor<ABaseMeleeWeapon>(this->MeleeWeaponClass);

		//get weapon socket of mesh
		const USkeletalMeshSocket* weapon_socket = GetMesh()->GetSocketByName(FName(TEXT("weapon_socket_r")));
		if (weapon_socket)
		{
			//Attach weapon to weapon socket of main hand on player mesh
			weapon_socket->AttachActor(MeleeWeapon, GetMesh());
			this->EnemyEquippedMeleeWeapon = MeleeWeapon;
			MeleeWeapon->SetOwner(this);
		}
	}
}


