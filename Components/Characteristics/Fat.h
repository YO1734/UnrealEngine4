// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Characteristic.h"
#include "Fat.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALGAME_API UFat : public UCharacteristic
{
	GENERATED_BODY ()
		UFUNCTION ( BlueprintCallable )
		virtual void SetParameters (class ASurvivalCharacter* Character) override;


};
