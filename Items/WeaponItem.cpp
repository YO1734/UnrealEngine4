// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/WeaponItem.h"
#include "Player/SurvivalPlayerController.h"
#include "Player/SurvivalCharacter.h"


UWeaponItem::UWeaponItem ()
{
}

bool UWeaponItem::Equip ( ASurvivalCharacter* Character )
{
 
    bool bEquipSuccesful = Super::Equip ( Character );

    if (bEquipSuccesful && Character)
    {
        Character->EquipWeapon ( this );
    }
    return bEquipSuccesful;
}

bool UWeaponItem::UnEquip ( ASurvivalCharacter* Character )
{
    bool bUnEquipSuccesful = Super::UnEquip ( Character );

    if (bUnEquipSuccesful && Character)
    {
        Character->UnEquipWeapon ();
    }
    return bUnEquipSuccesful;
}
