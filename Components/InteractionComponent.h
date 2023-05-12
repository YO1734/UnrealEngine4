// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "InteractionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBeginInteract, class ASurvivalCharacter*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEndInteract, class ASurvivalCharacter*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBeginFocus, class ASurvivalCharacter*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEndFocus, class ASurvivalCharacter*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteract, class ASurvivalCharacter*, Character);


/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SURVIVALGAME_API UInteractionComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:

	UInteractionComponent();
	
	//The time the player must hold the interact key to interact with this object
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="Interaction")
	float Interactiontime;

	//The max distance the player can be away from this actor before you can interact
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="Interaction")
	float InteractionDistance;

	//The name that will come up when the player looks at the intractable
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="Interaction")
	FText InteractionableNameText;

	//The verb that describes how the interaction works, ie "Sit", for a chair, "Eat" for food, "Light" for a fireplace
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="Interaction")
	FText InteractionableActionText;

	//Whether we allow multiple players to interact with the item, or just one at ant given time
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="Interaction")
	bool bAllowMulitpleInteractions;


	//Call this to change the name of the interactable. Will also refresh the interaction widget.
	UFUNCTION(BlueprintCallable, Category="Interaction")
	void SetInteractableNameText(const FText& NewNameText);

	UFUNCTION(BlueprintCallable, Category="Interaction")
	void SetInteractableActionText(const FText& NewActionText);



//Delegates

	//[local + server] Called when the player presses the interact key whilst focusing on this interactable actor
	UPROPERTY(BlueprintAssignable, BlueprintReadOnly, Category="Interaction")
	FOnBeginInteract OnBeginInteract;
	
	//[local + server] Called when the player releases the interact key, stops looking at the interactable actor, or gets too far away after starting an interact
	UPROPERTY(BlueprintAssignable, BlueprintReadOnly, Category="Interaction")
	FOnEndInteract OnEndInteract;

	//[local + server] Called when the player presses the interact key whilst focusing on this interactable actor
	UPROPERTY(BlueprintAssignable, BlueprintReadOnly, Category="Interaction")
	FOnBeginFocus OnBeginFocus;
	//[local + server] Called when the player releases the interact key , stops looking at the interactable actor, or gets too far away after starting an interact
	UPROPERTY(BlueprintAssignable, BlueprintReadOnly, Category="Interaction")
	FOnEndFocus OnEndFocus;

	//[local + server] Called when the player has interacted with the item for the required amount time
	UPROPERTY(BlueprintAssignable, BlueprintReadOnly, Category="Interaction")
	FOnBeginInteract OnInteract;
protected:
	
	//Called when the game	starts
	virtual void Deactivate() override;

	bool CanInteract(class ASurvivalCharacter* Character) const;

	//On the server, this will hold all interactors. On the local player, this will just hold local player (provided they are an interactor)
	UPROPERTY()
	TArray<class ASurvivalCharacter*> Interactors;

public:

	/*** Refresh the interaction widget and its custom widgets.
	An example of when we`d use this is when we take 3 items out of a stack of 10, and we neew to update the widget
	so it shows the stack as having 7 items left. */
	void RefreshWidget();

	//Called on the client whet the players interaction check trace begins/ends hitting this item
	void BeginFocus(class ASurvivalCharacter* Character);
	void EndFocus(class ASurvivalCharacter* Character);

	//Called on the client when the player begins/ends interaction with the item
	void BeginInteract(class ASurvivalCharacter* Character);
	void EndInteract(class ASurvivalCharacter* Character);

	void Interact(class ASurvivalCharacter* Character);


	//Return a value from 0-1 denoting how far through the interact we are.
	//On server this is the first interactors percentage, on client this is the local interactors percentage
	UFUNCTION(BlueprintPure, Category="Interaction")
	float GetInteractPercentage();



};
