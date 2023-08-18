// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "EquippableItem.generated.h"

// All the slots that gear can be uqipped to.
UENUM(BlueprintType)
enum class EEquippableSlot : uint8
{
	EIS_Head UMETA(Displayname = "Head"),
	EIS_Helmet UMETA(Displayname = "Helmet"),
	EIS_Chest UMETA(Displayname = "Chest"),
	EIS_Vest UMETA(Displayname = "Vest"),
	EIS_Legs UMETA(Displayname = "Legs"),
	EIS_Feet UMETA(Displayname = "Feet"),
	EIS_Hands UMETA(Displayname = "Hands"),
	EIS_Backpack UMETA(Displayname = "Backpack"),
	EIS_PrimaryWeapon UMETA(Displayname = "Primary Weapon"),
	EIS_Throwable UMETA(Displayname = "Throwable Item")
};

UCLASS(Abstract, NotBlueprintable)
class SURVIVALGAME_API UEquippableItem : public UItem
{
	GENERATED_BODY()

public:

	UEquippableItem ();

	UPROPERTY ( EditDefaultsOnly, BlueprintReadOnly, Category = "Equippables" )
	EEquippableSlot Slot;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Use ( class ASurvivalCharacter* Character ) override;

	UFUNCTION ( BlueprintCallable, Category = "Equippables" )
		virtual bool Equip ( class ASurvivalCharacter* Character );

	UFUNCTION ( BlueprintCallable, Category = "Equippables" )
		virtual bool UnEquip ( class ASurvivalCharacter* Character );

	virtual bool ShouldShowInInventory () const override;

	UFUNCTION ( BlueprintPure, Category = "Equippables" )
		bool IsEquipped () { return bEquipped; };

	// Call this on the server to equip the item
	void SetEquipped ( bool bNewEquipped );

protected:

	UPROPERTY ( ReplicatedUsing = EquipStatusChanged )
	bool bEquipped;
	
	UFUNCTION ()
		void EquipStatusChanged ();
};
