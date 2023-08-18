// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SurvivalPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALGAME_API ASurvivalPlayerController : public APlayerController
{
	GENERATED_BODY ()

	ASurvivalPlayerController ();

public:
	//Call this instead of show notification if on the server
	UFUNCTION ( Client, Reliable, BlueprintCallable )
	void ClientShowNotification ( const FText& Message );

	UFUNCTION ( BlueprintImplementableEvent )
	void ShowNotification ( const FText& Message );

	UFUNCTION ( BlueprintImplementableEvent )
	void ShowDeathScreen ( class ASurvivalCharacter* Killer );

	UFUNCTION ( BlueprintImplementableEvent )
	void ShowLootMenu ( const class UInventoryComponent* LootSource );

	UFUNCTION ( BlueprintImplementableEvent )
	void ShowIngameUI ();

	UFUNCTION ( BlueprintImplementableEvent, BlueprintCallable )
	void HideLootMenu ();

	UFUNCTION ( BlueprintimplementableEvent )
	void OnHitPlayer ();

};
