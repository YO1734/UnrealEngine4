// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivalCharacter.h"
#include "Camera/CameraComponent.h"
#include "Player/SurvivalPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Actor.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/InventoryComponent.h"
#include "Components/InteractionComponent.h"
#include "Blueprint/UserWidget.h"
#include "Items/EquippableItem.h"
#include "Items/GearItem.h"
#include "Materials/MaterialInstance.h"
#include "World/Pickup.h"

// Sets default values
ASurvivalCharacter::ASurvivalCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
CameraComponent->SetupAttachment(GetMesh(), FName("CameraSocket"));
CameraComponent->bUsePawnControlRotation = true;


HelmetMesh = PlayerMeshes.Add ( EEquippableSlot::EIS_Helmet, CreateDefaultSubobject<USkeletalMeshComponent> ( TEXT ( "Helmet" ) ) );
ChestMesh = PlayerMeshes.Add ( EEquippableSlot::EIS_Chest, CreateDefaultSubobject<USkeletalMeshComponent> ( TEXT ( "Chest" ) ) );
LegsMesh = PlayerMeshes.Add ( EEquippableSlot::EIS_Legs, CreateDefaultSubobject<USkeletalMeshComponent> ( TEXT ( "Legs" ) ) );
FeetMesh = PlayerMeshes.Add ( EEquippableSlot::EIS_Feet, CreateDefaultSubobject<USkeletalMeshComponent> ( TEXT ( "Feet" ) ) );
VestMesh = PlayerMeshes.Add ( EEquippableSlot::EIS_Vest, CreateDefaultSubobject<USkeletalMeshComponent> ( TEXT ( "Vesth" ) ) );
HandsMesh = PlayerMeshes.Add ( EEquippableSlot::EIS_Hands, CreateDefaultSubobject<USkeletalMeshComponent> ( TEXT ( "Hands" ) ) );
BackpackMesh = PlayerMeshes.Add ( EEquippableSlot::EIS_Backpack, CreateDefaultSubobject<USkeletalMeshComponent> ( TEXT ( "Backpack" ) ) );


//Tell all the body meshes to use the head mesh for animation
for (auto& PlayerMesh : PlayerMeshes)
{
	USkeletalMeshComponent* MeshComponent = PlayerMesh.Value;
	MeshComponent->SetupAttachment ( GetMesh () );
	MeshComponent->SetMasterPoseComponent ( GetMesh () );
}

PlayerMeshes.Add ( EEquippableSlot::EIS_Head, GetMesh () );

HelmetMesh = CreateDefaultSubobject<USkeletalMeshComponent>("HelmetMesh");
HelmetMesh->SetupAttachment(GetMesh());
HelmetMesh->SetMasterPoseComponent(GetMesh());

ChestMesh  = CreateDefaultSubobject<USkeletalMeshComponent>("ChestMesh");
ChestMesh->SetupAttachment(GetMesh());
ChestMesh->SetMasterPoseComponent(GetMesh());

LegsMesh = CreateDefaultSubobject<USkeletalMeshComponent>("LegsMesh");
LegsMesh->SetupAttachment(GetMesh());
LegsMesh->SetMasterPoseComponent(GetMesh());

FeetMesh = CreateDefaultSubobject<USkeletalMeshComponent>("FeetMesh");
FeetMesh->SetupAttachment(GetMesh());
FeetMesh->SetMasterPoseComponent(GetMesh());

VestMesh = CreateDefaultSubobject<USkeletalMeshComponent>("VestMesh");
VestMesh->SetupAttachment(GetMesh());
VestMesh->SetMasterPoseComponent(GetMesh());

HandsMesh = CreateDefaultSubobject<USkeletalMeshComponent>("HandsMesh");
HandsMesh->SetupAttachment(GetMesh());
HandsMesh->SetMasterPoseComponent(GetMesh());

BackpackMesh = CreateDefaultSubobject<USkeletalMeshComponent>("BackpackMesh");
BackpackMesh->SetupAttachment(GetMesh());
BackpackMesh->SetMasterPoseComponent(GetMesh());

//Give the player an inventory with 20 slots, 80kg capacity
PlayerInventory = CreateDefaultSubobject<UInventoryComponent>("Player Inventory");
PlayerInventory->SetCapacity(20);
PlayerInventory->SetWeightCapacity(60.f);



InteractionCheckFrequency = 0.f;
InteractionCheckDistance = 1000.f;


GetMesh()->SetOwnerNoSee(true);

GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
}
 
// Called when the game starts or when spawned
void ASurvivalCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	//When the player spawns in they have no items equipped, so cache these items
	for (auto& PlayerMesh : PlayerMeshes)
	{
		NakedMeshes.Add ( PlayerMesh.Key, PlayerMesh.Value->SkeletalMesh );
	}
}

void ASurvivalCharacter::CouldntFindInteractable()
{
	//We`ve lost focus on an interactable. Clear the timer
	if (GetWorldTimerManager().IsTimerActive(TimerHandle_Interact))
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_Interact);
	}

	//Tell the interactable we`ve stopped focusing on it, and clear the current interactable
	if (UInteractionComponent* Interactable = GetInteractable())
	{
		Interactable->EndFocus(this);
		if (InteractionData.bInteractHeld) {
			EndInteract();
		}
	}

	InteractionData.ViewedInteractionComponent = nullptr;
}

void ASurvivalCharacter::FoundNewInteractable(UInteractionComponent* Interactable)
{

	EndInteract();

	if (UInteractionComponent* OldInteractable = GetInteractable())
	{
		OldInteractable->EndFocus(this);
	}

	InteractionData.ViewedInteractionComponent = Interactable;
	Interactable->BeginFocus(this);

}

void ASurvivalCharacter::StartCrouching()
{
	Crouch();
}

void ASurvivalCharacter::StopCrouching()
{
	UnCrouch();
}

void ASurvivalCharacter::BeginInteract()
{

	if (!HasAuthority())
	{
		ServerBeginInteract();

	}

	/*As an optimization, the server only checks that we're looking at an item once we begin interacting with it.
	This saves the server doing a check every tick for an interactable Item. The exception is non-instant interact.
	In this case, the server will check every tick for the duration of the interact*/
	if (HasAuthority())
	{
		PerfomInteractionCheck();
	}

	InteractionData.bInteractHeld = true;


	if (UInteractionComponent* Interactable = GetInteractable())
	{
		Interactable->BeginInteract(this);

		if (FMath::IsNearlyZero(Interactable->Interactiontime))
		{
			Interact();
		}
		else
		{
			GetWorldTimerManager().SetTimer(TimerHandle_Interact, this, &ASurvivalCharacter::Interact, Interactable->Interactiontime, false);
		}

	}

}

void ASurvivalCharacter::EndInteract()
{
	if (!HasAuthority())
	{
		ServerEndInteract();

	}

	InteractionData.bInteractHeld = false;

	GetWorldTimerManager().ClearTimer(TimerHandle_Interact);

	if (UInteractionComponent* Interactable = GetInteractable())
	{
		Interactable->EndInteract(this);
	}

}

void ASurvivalCharacter::ServerBeginInteract_Implementation()
{
	BeginInteract();
}

bool ASurvivalCharacter::ServerBeginInteract_Validate()
{
	return true;
}

void ASurvivalCharacter::ServerEndInteract_Implementation()
{
	EndInteract();
}

bool ASurvivalCharacter::ServerEndInteract_Validate()
{
	return true;
}

void ASurvivalCharacter::Interact()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Interact);

	if (UInteractionComponent* Interactable = GetInteractable())
	{
		Interactable->Interact(this);
	}
}
bool ASurvivalCharacter::IsInteracting() const
{
	return GetWorldTimerManager().IsTimerActive(TimerHandle_Interact);
}

float ASurvivalCharacter::GetRemainingInteractTime() const
{
	return GetWorldTimerManager().GetTimerRemaining(TimerHandle_Interact);
}

void ASurvivalCharacter::UseItem(UItem* Item)
{
	
	if (ASurvivalCharacter::AActor::GetLocalRole() < ROLE_Authority && Item)
	{
		ServerUseItem(Item);
	}

	if (HasAuthority())
	{
		if (PlayerInventory && PlayerInventory->FindItem(Item))
		{
			return;
		}
	}

	if (Item)
	{
		Item->Use ( this );
	}
}

void ASurvivalCharacter::DropItem(UItem* Item, const int32 Quantity)
{

	if (PlayerInventory && Item && PlayerInventory->FindItem(Item))
	{
		
		if (ASurvivalCharacter::AActor::GetLocalRole()< ROLE_Authority)
		{
			ServerDropItem(Item,Quantity);
			return;
		}
		if (HasAuthority())
		{
			const int32 ItemQuantity = Item->GetQuantity();
			const int32 DroppedQuantity = PlayerInventory->ConsumeItem(Item, Quantity);

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.bNoFail = true;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			FVector SpawnLocation = GetActorLocation();
			SpawnLocation.Z -= GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

			FTransform SpawnTransform(GetActorRotation(), SpawnLocation);

			ensure(PickupClass);

			APickup* Pickup = GetWorld()->SpawnActor<APickup>(PickupClass, SpawnTransform, SpawnParams);
			Pickup->InitializePickup(Item->GetClass(), DroppedQuantity);


		}
	}

}

void ASurvivalCharacter::ServerDropItem_Implementation(UItem* Item, const int32 Quantity)
{
	DropItem(Item, Quantity);

}
bool ASurvivalCharacter::ServerDropItem_Validate(UItem* Item, const int32 Quantity)
{
	return true;
}

void ASurvivalCharacter::ServerUseItem_Implementation(UItem* Item)
{
	UseItem(Item);
}
bool ASurvivalCharacter::ServerUseItem_Validate(UItem* Item)
{
	return true;
}

bool ASurvivalCharacter::EquipItem ( UEquippableItem* Item )
{
	EquippedItems.Add ( Item->Slot, Item );
	OnEquippedItemsChanged.Broadcast ( Item->Slot, Item );
	return true;
}

bool ASurvivalCharacter::UnEquipItem ( UEquippableItem* Item )
{
	
	if (Item)
	{
		if (EquippedItems.Contains ( Item->Slot ))
		{
			if (Item == *EquippedItems.Find ( Item->Slot ))
			{
				EquippedItems.Remove ( Item->Slot );
				OnEquippedItemsChanged.Broadcast ( Item->Slot, nullptr );
				return true;
			}
		}
	}
	return false;
}

void ASurvivalCharacter::EquipGear (class UGearItem* Gear )
{

	if (USkeletalMeshComponent* GearMesh = *PlayerMeshes.Find ( Gear->Slot ))
	{
		GearMesh->SetSkeletalMesh ( Gear->Mesh );
		GearMesh->SetMaterial ( GearMesh->GetMaterials ().Num () - 1,Gear->MaterialInstacne );
	}
}

void ASurvivalCharacter::UnEquipGear ( const EEquippableSlot Slot )
{

	if (USkeletalMeshComponent* EquippableMesh = *PlayerMeshes.Find ( Slot ))
	{
		if (USkeletalMesh* BodyMesh = *NakedMeshes.Find ( Slot ))
		{
			EquippableMesh->SetSkeletalMesh ( BodyMesh );

			//Put the materials back on the body mesh (Since gear may have applied a different material
			for (int32 i = 0; i < BodyMesh->Materials.Num (); i++)
			{
				if (BodyMesh->Materials.IsValidIndex ( i ))
				{
					EquippableMesh->SetMaterial ( i, BodyMesh->Materials[i].MaterialInterface );
				}
			}

		}
		else
		{
			//For some gear like backpacks, there is no naked mesh
			EquippableMesh->SetSkeletalMesh ( nullptr );
		}
	}

}

USkeletalMeshComponent* ASurvivalCharacter::GetSlotSkeletalMeshComponent ( const EEquippableSlot Slot )
{
	if (PlayerMeshes.Contains ( Slot ))
	{
		return *PlayerMeshes.Find ( Slot );
	}
	return nullptr;
}

void ASurvivalCharacter::MoveForward(float Val)
{
	if (Val != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Val);
	}
}

void ASurvivalCharacter::MoveRight(float Val)
{
	if (Val != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), Val);
	}
}

void ASurvivalCharacter::LookUp(float Val)
{
	if (Val != 0.0f) {
		AddControllerPitchInput(Val);
	}
}

void ASurvivalCharacter::Turn(float Val)
{
	if (Val != 0.0f) {
		AddControllerYawInput(Val);
	}
}

// Called every frame
void ASurvivalCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const bool bIsInteractionOnServer = (HasAuthority() && IsInteracting());
	if (!bIsInteractionOnServer && GetWorld()->TimeSince(InteractionData.LastInteractionCheckTime) > InteractionCheckFrequency)
	{
		PerfomInteractionCheck();
	}
}

void ASurvivalCharacter::Restart ()
{
	Super::Restart ();

	if (ASurvivalPlayerController* PC = Cast<ASurvivalPlayerController> ( GetController () ))
	{
		PC->ShowIngameUI ();
	}

}

void ASurvivalCharacter::PerfomInteractionCheck()
{

	if (GetController() == nullptr)
	{
		return;
	}

	InteractionData.LastInteractionCheckTime = GetWorld()->GetTimeSeconds();

	FVector EyesLoc;
	FRotator EyesRot;

	GetController()->GetPlayerViewPoint(EyesLoc, EyesRot);

	FVector TraceStart = EyesLoc;
	FVector TraceEnd = (EyesRot.Vector() * InteractionCheckDistance) + TraceStart;
	FHitResult TraceHit;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(TraceHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
	{
		//Check if we hit an interactable object
		if (TraceHit.GetActor())
		{
			//Check if this an interaction component
			if (UInteractionComponent* InteractionComponent = Cast<UInteractionComponent>(TraceHit.GetActor()->GetComponentByClass(UInteractionComponent::StaticClass())))
			{
				float Distance = (TraceStart - TraceHit.ImpactPoint).Size();

				if (InteractionComponent != GetInteractable() && Distance<= InteractionComponent->InteractionDistance)
				{
					FoundNewInteractable(InteractionComponent);

				}
				else if (Distance > InteractionComponent->InteractionDistance && GetInteractable())
				{
					CouldntFindInteractable();
				}

				return;
			}

		}
	}

	CouldntFindInteractable();

}



// Called to bind functionality to input
void ASurvivalCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ASurvivalCharacter::BeginInteract);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &ASurvivalCharacter::EndInteract);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASurvivalCharacter::StartCrouching);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASurvivalCharacter::StopCrouching);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASurvivalCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASurvivalCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &ASurvivalCharacter::LookUp);
	PlayerInputComponent->BindAxis("Turn", this, &ASurvivalCharacter::Turn);

}

