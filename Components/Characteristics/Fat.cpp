// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Characteristics/Fat.h"
#include "Player/SurvivalCharacter.h"

void UFat::SetParameters ( ASurvivalCharacter* Character )
{
	if (Character)
	{
		float ModifyWeight = FMath::RandRange ( 15, 45 );
		//Character->UpdateCharacterWeight ( ModifyWeight );
	}
}
