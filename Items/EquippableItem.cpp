// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/EquippableItem.h"
#include "Net/UnrealNetwork.h"
#include "Player/SurvivalCharacter.h"
#include "Components/InventoryComponent.h"

#define LOCTEXT_NAMESPACE "EquippableItem"


UEquippableItem::UEquippableItem ()
{
	bStackable = false;
	bEquipped = false;
	UseActionText = LOCTEXT ( "ItemUseActionText", "Equip");
}




void UEquippableItem::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps ( OutLifetimeProps );
	DOREPLIFETIME ( UEquippableItem, bEquipped );
}

void UEquippableItem::Use ( class ASurvivalCharacter* Character )
{

	if (Character)
	{
		if (Character->GetEquippedItems ().Contains ( Slot ) && !bEquipped)
		{
			UEquippableItem* AlreadyEquippedItem = *Character->GetEquippedItems ().Find ( Slot );

			AlreadyEquippedItem->SetEquipped ( false );
		}
		SetEquipped ( !IsEquipped () );
	}
}

bool UEquippableItem::Equip (class ASurvivalCharacter* Character )
{
	if (Character)
	{
		return Character->EquipItem ( this );
	}
	return false;
}

bool UEquippableItem::UnEquip (class ASurvivalCharacter* Character )
{
	UE_LOG ( LogTemp, Warning, TEXT ( "UNEQUIPPEDFunct" ) );
	if (Character)
	{
		return Character->UnEquipItem ( this );
		UE_LOG ( LogTemp, Warning, TEXT ( "UNEQUIPPED_TRUE" ) );
	}
	return false;
	UE_LOG ( LogTemp, Warning, TEXT ( "UNEQUIPPED_FALSE" ) );
}

bool UEquippableItem::ShouldShowInInventory () const
{
	return !bEquipped;
}

void UEquippableItem::SetEquipped ( bool bNewEquipped )
{
	bEquipped = bNewEquipped;
	EquipStatusChanged ();
	MarkDirtyForReplication ();
}

void UEquippableItem::EquipStatusChanged ()
{
	if (ASurvivalCharacter* Character = Cast<ASurvivalCharacter> ( GetOuter () ))
	{
		if (bEquipped)
		{
			Equip (Character);
		}
		else
		{
			UnEquip ( Character );
			UE_LOG ( LogTemp, Warning, TEXT ( "UNEQUIPPED") );

		}
	}

	//Tell ui to update
	OnItemModified.Broadcast ();
}

#undef LOCTEXT_NAMESPACE