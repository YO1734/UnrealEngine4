// Fill out your copyright notice in the Description page of Project Settings.


#include "World/Pickup.h"
#include "Items/Item.h"
#include"Net/UnrealNetwork.h"
#include "Player/SurvivalCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InteractionComponent.h"
#include "Components/InventoryComponent.h"
#include "Engine/ActorChannel.h"
// Sets default values
APickup::APickup()
{

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>("PickupMesh");
	PickupMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	SetRootComponent(PickupMesh);

	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>("PickupInteractionComponent");
	InteractionComponent->Interactiontime = 0.5f;
	InteractionComponent->InteractionDistance = 200.f;
	InteractionComponent->InteractionableNameText = FText::FromString("Pickup");
	InteractionComponent->InteractionableActionText = FText::FromString("Take");
	InteractionComponent->OnInteract.AddDynamic(this, &APickup::OnTakePickup);
	InteractionComponent->SetupAttachment(PickupMesh);

	SetReplicates(true);


}

void APickup::InitializePickup(const TSubclassOf<class UItem> ItemClass, const int32 Quantity)
{
	if (HasAuthority() && ItemClass && Quantity > 0)
	{
		Item = NewObject<UItem>(this, ItemClass);
		Item->SetQuantity(Quantity);

		OnRep_Item();
		Item->MarkDirtyForReplication();
	}
}

void APickup::OnRep_Item()
{
	if (Item)
	{
		PickupMesh->SetStaticMesh(Item->PickupMesh);
		InteractionComponent->InteractionableNameText = Item->ItemDisplayName;

		//Clients bind to this delegate in order to refresh the interaction widget if item quantity changes
		Item->OnItemModified.AddDynamic(this, &APickup::OnItemModified);
	}

	//If any replicated properties on the item are changed, we refresh the widget
	InteractionComponent->RefreshWidget();
}

void APickup::OnItemModified()
{
	if (InteractionComponent)
	{
		InteractionComponent->RefreshWidget();
	}

}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority() && ItemTemplate && bNetStartup)
	{
		InitializePickup(ItemTemplate->GetClass(), ItemTemplate->GetQuantity());
	}

	/* If pickup was spawned in at runtime, ensure that it mathces the rotation of the ground that it was dropped on
	If we dropeed a pickup on a 20 degree slope, the pick up would also be spawned at a 20 degree angle*/
	if (!bNetStartup)
	{
		AlignWithGround();
	}

	if (Item)
	{
		Item->MarkDirtyForReplication();
	}


}

void APickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(APickup, Item);
}

bool APickup::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	if (Item && Channel->KeyNeedsToReplicate(Item->GetUniqueID(), Item->Repkey))
	{
		bWroteSomething |= Channel->ReplicateSubobject(Item, *Bunch, *RepFlags);
	}
	return bWroteSomething;
}

#if WITH_EDITOR
void APickup::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{

	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	//If a new pickup is selected in the property editor, change the mesh to reflect the new item being selected
	if (PropertyName == GET_MEMBER_NAME_CHECKED(APickup, ItemTemplate))
	{
		if (ItemTemplate)
		{
			PickupMesh->SetStaticMesh(ItemTemplate->PickupMesh);
		}

	}

}
#endif

void APickup::OnTakePickup(ASurvivalCharacter* Taker)
{
	if (!Taker)
	{
		UE_LOG(LogTemp, Warning, TEXT("Pick up was taken but player was not valid"));
		return;
	}

	//Not 100% sure Pending kill check is needed but should prevent player from taking a pickup another player has already tried taking
	if (HasAuthority() && !IsPendingKillPending() && Item)
	{
		if (UInventoryComponent* PlayerInventory = Taker->PlayerInventory)
		{
			const FItemAddResult AddResult = PlayerInventory->TryAddItem(Item);

			if (AddResult.ActualAmountGiven < Item->GetQuantity())
			{
				
				Item->SetQuantity(Item->GetQuantity() - AddResult.ActualAmountGiven);
			
			}
			else if (AddResult.ActualAmountGiven >= Item->GetQuantity())
			{
				
				Destroy();
			}

		}
	}

}

