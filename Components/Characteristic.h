// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Characteristic.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALGAME_API UCharacteristic : public UObject
{
	GENERATED_BODY ()

protected:
	UPROPERTY ( VisibleAnywhere, BlueprintReadOnly, Category = CharInfo )
	FText Name;

	UPROPERTY ( VisibleAnywhere, BlueprintReadOnly, Category = CharInfo )
		bool bIsPositive;

	UPROPERTY ( VisibleAnywhere, BlueprintReadOnly, Category = CharInfo )
		UTexture2D* Icon;
public:
	virtual void SetParameters (class ASurvivalCharacter* Character);
	UCharacteristic ();
	UFUNCTION()
	FText GetName () { return Name; };

	UFUNCTION ()
		UTexture2D* GetIcon () { return Icon; };

	UFUNCTION ()
		bool IsPositive () { return bIsPositive; };


};
