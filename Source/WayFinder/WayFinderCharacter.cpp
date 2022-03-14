// Copyright Epic Games, Inc. All Rights Reserved.

#include "WayFinderCharacter.h"
#include "Animation/AnimInstance.h"
#include "BaseEnemy.h"
#include "BaseMeleeWeapon.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "WayFinderHealthComponent.h"

//////////////////////////////////////////////////////////////////////////
// AWayFinderCharacter

AWayFinderCharacter::AWayFinderCharacter():
	bIsAbleToFly(false),
	LivesLeft(0),
	LivesMax(3),
	AbilityOneMontage(nullptr),
	bIsPlayerDead(false),
	DeathTimerTime(5.f),
	MaxZoomOut(135.f),
	MaxZoomIn(45.f)
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

	//Init lives left to max lives, livesmax set in initializer list
	this->LivesLeft = this->LivesMax;

	PlayerHealthComponent = CreateDefaultSubobject<UWayFinderHealthComponent>(TEXT("HealthComponent"));
}

//////////////////////////////////////////////////////////////////////////
// Input

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

bool AWayFinderCharacter::LooseLife(bool should_apply_multiplier)
{
	if ((this->LivesLeft - 1) <= 0)
	{
		this->LivesLeft = 0;
		UE_LOG(LogTemp, Warning, TEXT("AWAYFINDERCHARACTER::LooseLife__Live Left: %d"), this->LivesLeft);
		this->Die();
		
	}

	this->LivesLeft -= 1;
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

void AWayFinderCharacter::SpawnDefaultMeleeWeapon()
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
			this->PlayerEquippedMeleeWeapon = MeleeWeapon;
			MeleeWeapon->SetOwner(this);
		}
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

void AWayFinderCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		this->SpawnDefaultMeleeWeapon();
	}
	

}



void AWayFinderCharacter::EnableWeaponCollision()
{
	if (this->PlayerEquippedMeleeWeapon)
	{
		this->PlayerEquippedMeleeWeapon->ToggleWeaponCollision(true);
	}
	
}

void AWayFinderCharacter::DisableWeaponCollision()
{
	if (!this->PlayerEquippedMeleeWeapon) { return; }
	ABaseEnemy* enemy_hit = Cast<ABaseEnemy>(this->PlayerEquippedMeleeWeapon->GetActorOverlappedOnUse());
	if (enemy_hit)
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
	}

	this->PlayerEquippedMeleeWeapon->ToggleWeaponCollision(false);
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

void AWayFinderCharacter::PlayerTakeDamage(float dmg_amount)
{
	if (this->PlayerHealthComponent)
	{
		this->PlayerHealthComponent->HealthTakeDamage(dmg_amount);
	}
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

void AWayFinderCharacter::ServerDoDamage_Implementation(ABaseEnemy* enemy_target)
{
	this->DoDamage(enemy_target);
}




bool AWayFinderCharacter::ServerDoDamage_Validate(ABaseEnemy* enemy_target)
{
	return true;
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
	if ( (Controller != nullptr) && (Value != 0.0f) )
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


void AWayFinderCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWayFinderCharacter, PlayerEquippedMeleeWeapon);
}