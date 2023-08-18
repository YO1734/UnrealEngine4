// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/GearItem.h"
#include "Player/SurvivalCharacter.h"


UGearItem::UGearItem ()
{
    DamageDefenseMultiplier = 0.1f;
}

bool UGearItem::Equip ( ASurvivalCharacter* Character )
{
    bool bEquipSuccsesful = Super::Equip ( Character );

    if (bEquipSuccsesful && Character)
    {
        Character->EquipGear ( this );
    }
    return bEquipSuccsesful;
}

bool UGearItem::UnEquip ( ASurvivalCharacter* Character )
{
    bool bEquipSuccsesful = Super::UnEquip ( Character );

    if (bEquipSuccsesful && Character)
    {
        Character->UnEquipGear ( Slot );
    }
    return bEquipSuccsesful;
}

