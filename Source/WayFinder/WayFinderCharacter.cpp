// Copyright Epic Games, Inc. All Rights Reserved.

#include "WayFinderCharacter.h"
#include "Animation/AnimInstance.h"
#include "BaseEnemy.h"
#include "BaseMeleeWeapon.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/WidgetComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "InventorySystem.h"
#include "Item.h"
#include "Kismet/GameplayStatics.h"
#include "LevelSystem.h"
#include "Math/UnrealMathUtility.h"
#include "Net/UnrealNetwork.h"
#include "WayFinderHealthComponent.h"
#include "WayFinder.h"
#include "Weapon.h"
#include "Consumable.h"
#include "Food.h"



AWayFinderCharacter::AWayFinderCharacter():
	bIsAbleToFly(false),
	LivesMax(3),
	LivesLeft(0),
	AbilityOneMontage(nullptr),
	bIsPlayerDead(false),
	DeathTimerTime(5.f),
	MaxZoomOut(135.f),
	MaxZoomIn(45.f),
	WFPlayerState(EPlayerState::PS_Idle),
	PlayerCombatStateTimerTime(5.f),
	NumItemsOverlapping(0),
	InventoryCapacity(10),
	bShouldTraceForItems(false),
	bIsInvuln(false)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	//CharacterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh"));
	//CharacterMesh->SetupAttachment(GetRootComponent());

	//Init lives left to max lives, livesmax set in initializer list
	this->LivesLeft = this->LivesMax;

	//Health component creation
	this->PlayerHealthComponent = CreateDefaultSubobject<UWayFinderHealthComponent>(TEXT("HealthComponent"));

	//Player inventory component creation
	this->Inventory = CreateDefaultSubobject<UInventorySystem>(TEXT("Inventory"));

	//Player level system component creation 
	this->PlayerLevelSystem = CreateDefaultSubobject<ULevelSystem>(TEXT("PlayerLevel"));

	
}

  /******************************************************************************************************************************************/
 /******************************************************************************************************************************************
	MOVEMENT AND INPUT METHODS
 ******************************************************************************************************************************************/
/*******************************************************************************************************************************************/
void AWayFinderCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Fly", IE_Pressed, this, &AWayFinderCharacter::ToggleFly);

	//Set up ability bar actions
	PlayerInputComponent->BindAction("AbilityOne", IE_Pressed, this, &AWayFinderCharacter::PressedAbilityOne);
	PlayerInputComponent->BindAction("AbilityTwo", IE_Pressed, this, &AWayFinderCharacter::PressedAbilityTwo);
	PlayerInputComponent->BindAction("AbilityThree", IE_Pressed, this, &AWayFinderCharacter::PressedAbilityThree);
	PlayerInputComponent->BindAction("UltimateAbility", IE_Pressed, this, &AWayFinderCharacter::ActivateUltimateAbility);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AWayFinderCharacter::PressedInteract);
	
	//Move forward, sideways, & vertically axis binding
	PlayerInputComponent->BindAxis("MoveForward", this, &AWayFinderCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AWayFinderCharacter::MoveRight);
	PlayerInputComponent->BindAxis("MoveUp", this, &AWayFinderCharacter::MoveUp);

	//Turn and Look Axis binding
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AWayFinderCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AWayFinderCharacter::LookUpAtRate);

	PlayerInputComponent->BindAxis("Zoom", this, &AWayFinderCharacter::ZoomEnabled);

}

//Fun input action to allow z axis movement
void AWayFinderCharacter::ToggleFly()
{
	if (this->GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Flying)
	{
		this->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		this->bIsAbleToFly = false;
	}
	else
	{
		this->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
		this->bIsAbleToFly = true;
	}
	
}

//Input axis adjustment for z axis movement (animation still does walking, this is just for gags)
void AWayFinderCharacter::MoveUp(float Value)
{
	if (this->bIsAbleToFly)
	{
		if ((Controller != nullptr) && (Value != 0.0f))
		{
			// find out which way is right
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get right vector 
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Z);
			// add movement in that direction
			AddMovementInput(Direction, Value);
		}
	}
}

void AWayFinderCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AWayFinderCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AWayFinderCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AWayFinderCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

//Zoom function to allow user to zoom FOV of player camera
void AWayFinderCharacter::ZoomEnabled(float value)
{
	//FollowCamera
	if (!this->FollowCamera) { UE_LOG(LogTemp, Warning, TEXT("NoFollowCamera")); return; }

	float cur_fov(0.f);
	cur_fov = this->FollowCamera->FieldOfView;

	float interp_to_loc = cur_fov + ((this->ZoomSpeed * 2) * value);

	//UE_LOG(LogTemp, Warning, TEXT("cur_fov: %f"), cur_fov);
	cur_fov = FMath::FInterpTo(cur_fov, interp_to_loc, GetWorld()->GetDeltaSeconds(), this->ZoomSpeed);
	//UE_LOG(LogTemp, Warning, TEXT("cur_fov: %f"), cur_fov);

	if (cur_fov > this->MaxZoomOut) { cur_fov = this->MaxZoomOut; }
	if (cur_fov < this->MaxZoomIn) { cur_fov = this->MaxZoomIn; }
	this->FollowCamera->SetFieldOfView(cur_fov);
	//UE_LOG(LogTemp, Warning, TEXT("cur_fov_camera: %f"), this->FollowCamera->FieldOfView);
	/* Update camera's current FOV */
	//this->CameraCurrentFOV = FMath::FInterpTo(this->CameraCurrentFOV,
		//this->CameraZoomedFOV, delta_time, this->ZoomInterpSpeed);
}


//TICK && BEGINPLAYER

void AWayFinderCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	this->TraceForItems();

	//UE_LOG(LogTemp, Warning, TEXT("num overlapping: %d"), this->NumItemsOverlapping);
}

void AWayFinderCharacter::BeginPlay()
{
	Super::BeginPlay();

	//if (HasAuthority())
	//{
		this->SpawnDefaultMeleeWeapon();
		this->Inventory->SetPlayerOwner(this); //Set inventories owner
		this->Inventory->InventoryCapacity = 20; //Inventory capacity
		

		this->PlayerLevelSystem->SetLevelSystemOwner(this);
		this->PlayerHealthComponent->SetHealthComponentOwner(this);

		float something_set = 0.5f;

		this->WFPlayerState = EPlayerState::PS_Idle;
	//}


}



/******************************************************************************************************************************************/
/******************************************************************************************************************************************
   HANDLE PLAYER DEATH METHODS
******************************************************************************************************************************************/
/*******************************************************************************************************************************************/

bool AWayFinderCharacter::LooseLife(bool should_apply_multiplier)
{
	if ((this->LivesLeft - 1) <= 0)
	{
		this->LivesLeft = 0;
		UE_LOG(LogTemp, Warning, TEXT("AWAYFINDERCHARACTER::LooseLife__Live Left: %d"), this->LivesLeft);
		this->BeginDie();
	}

	this->LivesLeft -= 1;
	this->PlayerHealthComponent->SetCurrentHealth(this->PlayerHealthComponent->GetMaxHealth());
	return false;
}

void AWayFinderCharacter::BeginDie()
{
	// Get player controller, disable input
	APlayerController* controller = Cast<APlayerController>(this->GetController());
	if (controller)
	{
		DisableInput(controller);
	}

	this->bIsPlayerDead = true; //Set player is dead to true to disable any furthef damage taking
	//Set death timer

	GetWorldTimerManager().SetTimer(this->DeathTimerHandle, this, &AWayFinderCharacter::Die, this->DeathTimerTime);

	//Player death montage
	if (this->DeathMontage)
	{
		UAnimInstance* anim_inst = GetMesh()->GetAnimInstance();
		if (anim_inst)
		{
			anim_inst->Montage_Play(this->DeathMontage);
			//GetWorldTimerManager().SetTimer(this->DeathTimerHandle, this, &ABaseEnemy::Die, this->DeathTimerTime);
		}
	}
}

void AWayFinderCharacter::Die()
{
	UE_LOG(LogTemp, Warning, TEXT("Die Call from AWayfinderCharacter"));
	if (this->PlayerEquippedMeleeWeapon)
	{
		this->PlayerEquippedMeleeWeapon->Destroy();
	}
	GetMesh()->bPauseAnims = true;
	this->Destroy();
}

void AWayFinderCharacter::PlayerHasDied()
{
	if (this->bIsPlayerDead) { return; }

	if (this->PlayerHealthComponent->GetCurrentHealth() <= 0)
	{
		//If player has lives left, take life and return 
		if (this->LivesLeft > 0)
		{
			this->LooseLife(false);
			return;
		}

		this->BeginDie();
	}

}



/******************************************************************************************************************************************/
/******************************************************************************************************************************************
   iNVENTORY & ITEM PICKUP METHODS
******************************************************************************************************************************************/
/*******************************************************************************************************************************************/


void AWayFinderCharacter::TraceForItems()
{
	if (!this->bShouldTraceForItems)
	{
		if (this->TraceHitItem)
		{
			this->TraceHitItem = nullptr;
		}
		if (this->TraceHitLastFrame)
		{
			this->TraceHitLastFrame = nullptr;
		}
		return;
		//UE_LOG(LogTemp, Warning, TEXT("Returned from TraceForItems inside tick inside WFcharacter"));
	}


	/* line trace for items */
	FHitResult item_trace_hit;
	FVector hit_location;
	if (this->TraceUnderCrosshairs(item_trace_hit, hit_location)) //Return true if 
	{
		//Cast hit actor to item
		this->TraceHitItem = Cast<AItem>(item_trace_hit.Actor);

		if (this->TraceHitItem && this->TraceHitItem->GetPickupWidget())
		{
			this->TraceHitItem->GetPickupWidget()->SetVisibility(true);
		}

		//If there was an item hit by last frame's trace
		if (this->TraceHitLastFrame)
		{
			//If that item is different from the last frame set old widget on old item trace to invisible
			if (this->TraceHitItem != this->TraceHitLastFrame)
			{
				this->TraceHitLastFrame->GetPickupWidget()->SetVisibility(false);
				
			}
		}

		//Set trace hit last frame to this item that is currently being hit by line trace
		this->TraceHitLastFrame = this->TraceHitItem;
		
	}
	else //There was no item hit, and check if the previously hit trace item is still registered, clear if so.
	{
		//clear trace hit item if no longer found
		if (this->TraceHitItem)
		{
			this->TraceHitItem = nullptr;
		}

		if (this->TraceHitLastFrame)
		{
			if (this->TraceHitItem != this->TraceHitLastFrame)
			{
				this->TraceHitLastFrame->GetPickupWidget()->SetVisibility(false);
				this->TraceHitLastFrame = nullptr; //Reset trace items
			}
		}
	}


}

bool AWayFinderCharacter::TraceUnderCrosshairs(FHitResult& out_hit_result, FVector& out_hit_location)
{
	FVector2D viewportsize;
	if (GEngine && GEngine->GameViewport)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Geninge"));
		GEngine->GameViewport->GetViewportSize(viewportsize);
	}

	/* Get screen space CH loc */
	FVector2D crosshair_loc(viewportsize.X / 2.f, viewportsize.Y / 2.f);
	crosshair_loc.Y -= 70;
	FVector crosshair_world_pos;
	FVector crosshair_world_dir;

	/* Get world pos and dir of cross hair */
	APlayerController* player_controller = Cast<APlayerController>(GetController());
	if (player_controller)
	{
		bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(player_controller,
			crosshair_loc, crosshair_world_pos, crosshair_world_dir);
		
		if (bScreenToWorld)
		{
			
			FVector start(crosshair_world_pos);
			
			FVector end(start + crosshair_world_dir * 50000);
			out_hit_location = end;
			GetWorld()->LineTraceSingleByChannel(out_hit_result, start, end, COLLISION_ITEM);

			if (out_hit_result.bBlockingHit)
			{
				UE_LOG(LogTemp, Warning, TEXT("Blocking hit: %s"), *out_hit_result.Actor->GetName());
				out_hit_location = out_hit_result.Location;
				return true;
			}
			//UE_LOG(LogTemp, Warning, TEXT("No blocking hit"));
			//DrawDebugLine(GetWorld(), start, out_hit_location, FColor::Red, false, 1.f, 0, 2.f);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Did not deproject"));
		}
		
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("No player controller"));
	}
	
	
	return false;
	
}

void AWayFinderCharacter::PickUpItem()
{
	//if no item to pick up is being looked at exit
	if (!this->TraceHitItem) { return; }
	if (this->Inventory)
	{
		this->Inventory->AddItem(this->TraceHitItem);
		UE_LOG(LogTemp, Warning, TEXT("Picked up item"));
	}

}

void AWayFinderCharacter::AdjustOverlappedItems(int32 amount_to_adjust)
{

	this->NumItemsOverlapping += amount_to_adjust;
	UE_LOG(LogTemp, Warning, TEXT("Set to: %d"), this->NumItemsOverlapping);

	if (this->NumItemsOverlapping <= 0)
	{
		this->NumItemsOverlapping = 0;
		this->bShouldTraceForItems = false;
		UE_LOG(LogTemp, Warning, TEXT("Set to zero"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Adjusted overlapped actors"));
		if (!this->bShouldTraceForItems) { this->bShouldTraceForItems = true; } 	//Only set the once per reset

	}
}

void AWayFinderCharacter::PressedInteract()
{


	UE_LOG(LogTemp, Warning, TEXT("PResed interact"));
	this->PickUpItem();

}

void AWayFinderCharacter::SpawnDefaultMeleeWeapon()
{
	if (this->MeleeWeaponClass)
	{
		//Spawn a basemeleeweapon
		ABaseMeleeWeapon* MeleeWeapon = this->GetWorld()->SpawnActor<ABaseMeleeWeapon>(this->MeleeWeaponClass);

		//get weapon socket of mesh
	
		this->PlayerEquippedMeleeWeapon = MeleeWeapon;
		MeleeWeapon->SetOwner(this);
		this->AttachItemToHand(MeleeWeapon);
	}
}

void AWayFinderCharacter::EquipItem(AItem* item_to_equip)
{
	
	//Add a weapon type (axe, sword, katana, mace, etc.) to switch on
	//Weapon is melee type
	ABaseMeleeWeapon* melee_weapon = Cast<ABaseMeleeWeapon>(item_to_equip);
	if (melee_weapon)
	{
		if (this->PlayerEquippedMeleeWeapon)
		{
			if (this->Inventory->RemoveItem(item_to_equip->InventorySlotIndex))
			{
				this->Inventory->AddItem(this->PlayerEquippedMeleeWeapon, item_to_equip->InventorySlotIndex);
				this->PlayerEquippedMeleeWeapon = melee_weapon;
				this->AttachItemToHand(this->PlayerEquippedMeleeWeapon);
				UE_LOG(LogTemp, Warning, TEXT("Could not remove item__EquipItem::AWayfinderCharacter"));
			}
				
		}
		else
		{
			if (!this->Inventory->RemoveItem(item_to_equip->InventorySlotIndex))
			{
				UE_LOG(LogTemp, Warning, TEXT("Could not remove item__EquipItem::AWayfinderCharacter"));
			}
			this->PlayerEquippedMeleeWeapon = melee_weapon;
			this->AttachItemToHand(this->PlayerEquippedMeleeWeapon);
		}
	}
	
	//non melee type ADD these
}

void AWayFinderCharacter::AttachItemToHand(AItem* item_to_attach)
{
	//get weapon socket of mesh
	const USkeletalMeshSocket* weapon_socket = GetMesh()->GetSocketByName(FName(TEXT("weapon_socket_r")));
	if (weapon_socket)
	{
		//FName name_of_socket(weapon_socket->GetName());
		FAttachmentTransformRules rules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
		//const FAttachmentTransformRules  transformrules = FAttachmentTransformRules(FAttachmentTransformRules::SnapToTargetIncludingScale);
		item_to_attach->AttachToComponent(GetMesh(), rules, FName(TEXT("weapon_socket_r")));
		item_to_attach->SetItemState(EItemState::EIS_Equipped);
	}
}

void AWayFinderCharacter::UseItem(AItem* item_to_use)
{

	AWeapon* weapon_item = Cast<AWeapon>(item_to_use);
	if (weapon_item)
	{
		this->EquipItem(weapon_item);
	}
	

}



/******************************************************************************************************************************************/
/******************************************************************************************************************************************
   PLAYER ABILITY, ATTACK, DAMAGE, TAKE DAMAGE METHODS
******************************************************************************************************************************************/
/*******************************************************************************************************************************************/


void AWayFinderCharacter::PressedAbilityOne()
{
	UE_LOG(LogTemp, Warning, TEXT("PressedAbilityOne"));
	this->ActivateAbilityOne();
}

void AWayFinderCharacter::PressedAbilityTwo()
{
	UE_LOG(LogTemp, Warning, TEXT("PressedAbilityTwo"));
}

void AWayFinderCharacter::PressedAbilityThree()
{
	UE_LOG(LogTemp, Warning, TEXT("PressedAbilityThree"));
}

void AWayFinderCharacter::ActivateUltimateAbility()
{
	if (this->PlayerEquippedMeleeWeapon) { return; }

	if (this->PlayerEquippedMeleeWeapon->CanUseUlt())
	{
		this->PlayerEquippedMeleeWeapon->UseUlt();
	}
}

void AWayFinderCharacter::ActivateAbilityOne()
{
	//if (this->AbilityOneMontage)
	//{
	//	UAnimInstance* anim_inst = GetMesh()->GetAnimInstance();
	//	if (anim_inst)
	//	{
	//		anim_inst->Montage_Play(this->AbilityOneMontage);
	//		anim_inst->Montage_JumpToSection(FName("StartAbility"));
	//	}
	//	UE_LOG(LogTemp, Warning, TEXT("No Anim Instance"));
	//}
	UE_LOG(LogTemp, Warning, TEXT("No Ability One Montage"));
}

void AWayFinderCharacter::ActivateAbilityTwo()
{
}

void AWayFinderCharacter::ActivateAbilityThree()
{
}

void AWayFinderCharacter::EnableWeaponCollision()
{
	if (this->PlayerEquippedMeleeWeapon)
	{
		this->PlayerEquippedMeleeWeapon->ToggleWeaponCollision(true);
		this->WFPlayerState = EPlayerState::PS_Attacking;
	}
	
}

void AWayFinderCharacter::DisableWeaponCollision()
{
	if (!this->PlayerEquippedMeleeWeapon) { return; }
	//Need to implement server damaage stuff on the weapon bc that's where im handling damage now
	/*	if (enemy_hit)
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponReturned valid enemy to damage"));
		if (!enemy_hit->GetEnemyInvulnerability())
		{
			UE_LOG(LogTemp, Warning, TEXT("enemy is not dmaaged"));
			if (!HasAuthority())
			{
				UE_LOG(LogTemp, Warning, TEXT("Calling do server damage"));
				this->ServerDoDamage(enemy_hit);
			}

			UE_LOG(LogTemp, Warning, TEXT("ths is sever calling do damage"));
			this->DoDamage(enemy_hit);
			
		}
		//enemy_hit->TakeDamage(this->PlayerEquippedMeleeWeapon->GetBaseWeaponDamage(), FDamageEvent, GetController(), this);
		//reset overlap enemy actor trigger
		this->PlayerEquippedMeleeWeapon->ToggleWeaponWaitingToApplyDamage(false);
	}*/
	this->PlayerEquippedMeleeWeapon->ToggleWeaponCollision(false);
	this->WFPlayerState = EPlayerState::PS_Combat;
	//Set get out of combat state timer
	GetWorldTimerManager().SetTimer(this->PlayerCombatStateTimer, this, &AWayFinderCharacter::LeaveCombatState, this->PlayerCombatStateTimerTime);

}

void AWayFinderCharacter::DoDamage(ABaseEnemy* enemy_target)
{
	if (enemy_target && enemy_target->GetEnemyHealthComponent() > 0)
	{
		enemy_target->EnemyTakeDamage(this->PlayerEquippedMeleeWeapon->GetBaseWeaponDamage(), this);
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("Exited AWFH::DoDamage__no enemy_target(ABaseEnemy*) or enemy_target's health is <= 0"));
	
}

//Used to adjust the amount of damage applied when attacking
float AWayFinderCharacter::GetPlayerWeaponWeaponAdjustments()
{
	return 0.f;
}

void AWayFinderCharacter::PlayerTakeDamage(float dmg_amount)
{
	if (this->bIsInvuln) { UE_LOG(LogTemp, Error, TEXT("Took damageeeeeeeeeeeeeeeee")); return; }
	UE_LOG(LogTemp, Error, TEXT("Took damage"));
	//if (HasAuthority())
	//{
		//if (this->PlayerHealthComponent)
		//{
			//this->WFPlayerState = EPlayerState::PS_Combat;
			this->PlayerHealthComponent->HealthTakeDamage(dmg_amount);
		//}

	//}
	//else
	//{
	//	this->ServerPlayerTakeDamage(dmg_amount);
	//}
	
}

void AWayFinderCharacter::PlayerGainHealth(float health_amount)
{
	if (this->PlayerHealthComponent)
	{
		this->PlayerHealthComponent->SetCurrentHealth(health_amount);
	}
}

/*void AWayFinderCharacter::UseItem(AItem* item_to_use)
{
	item_to_use->UseItem(this);
}*/

void AWayFinderCharacter::LeaveCombatState()
{
	this->WFPlayerState = EPlayerState::PS_Idle;
}


/******************************************************************************************************************************************/
/******************************************************************************************************************************************
   SERVER METHODS
******************************************************************************************************************************************/
/*******************************************************************************************************************************************/


void AWayFinderCharacter::ServerPlayerTakeDamage_Implementation(float dmg_amount)
{
	if (this->PlayerHealthComponent)
	{
		this->WFPlayerState = EPlayerState::PS_Combat;
		this->PlayerHealthComponent->HealthTakeDamage(dmg_amount);
	}
}

bool AWayFinderCharacter::ServerPlayerTakeDamage_Validate(float dmg_amount)
{
	return true;
}

void AWayFinderCharacter::ServerDoDamage_Implementation(ABaseEnemy* enemy_target)
{
	this->DoDamage(enemy_target);
}

bool AWayFinderCharacter::ServerDoDamage_Validate(ABaseEnemy* enemy_target)
{
	return true;
}

void AWayFinderCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWayFinderCharacter, PlayerEquippedMeleeWeapon);
}