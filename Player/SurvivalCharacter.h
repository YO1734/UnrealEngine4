// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SurvivalCharacter.generated.h"

USTRUCT()
struct FInteractionData
{

	GENERATED_BODY()

	FInteractionData()
	{
		ViewedInteractionComponent = nullptr;
		LastInteractionCheckTime = 0.f;
		bInteractHeld = false;
	}

	UPROPERTY()
	class UInteractionComponent* ViewedInteractionComponent;

	UPROPERTY()
	float LastInteractionCheckTime;

	UPROPERTY()
	bool bInteractHeld;
};


UCLASS()
class SURVIVALGAME_API ASurvivalCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASurvivalCharacter();

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Components")
	class UInventoryComponent* PlayerInventory;

	UPROPERTY(EditAnywhere, Category="Compoents")
	class UCameraComponent* CameraComponent;

	UPROPERTY(EditAnywhere, Category="Compoents")
	class USkeletalMeshComponent* HelmetMesh;

	UPROPERTY(EditAnywhere, Category="Compoents")
	class USkeletalMeshComponent* ChestMesh;

	UPROPERTY(EditAnywhere, Category="Compoents")
	class USkeletalMeshComponent* LegsMesh;

	UPROPERTY(EditAnywhere, Category="Compoents")
	class USkeletalMeshComponent* FeetMesh;

	UPROPERTY(EditAnywhere, Category="Compoents")
	class USkeletalMeshComponent* VestMesh;

	UPROPERTY(EditAnywhere, Category="Compoents")
	class USkeletalMeshComponent* HandsMesh;

	UPROPERTY(EditAnywhere, Category="Compoents")
	class USkeletalMeshComponent* BackpackMesh;



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//How often in seconds to check for an interactable object. Set this to zero if you want to check every tick.
	UPROPERTY(EditDefaultsOnly,Category="Interaction")
	float InteractionCheckFrequency;

	//How far we`ll trace when we check if the player is looking at an interactable object
	UPROPERTY(EditDefaultsOnly, Category="Interaction")
	float InteractionCheckDistance;

	void PerfomInteractionCheck();

	void CouldntFindInteractable();
	void FoundNewInteractable(UInteractionComponent* Interactable);
	    
	void StartCrouching();
	void StopCrouching();

	void BeginInteract();
	void EndInteract();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerBeginInteract();

	UFUNCTION(Server,Reliable,WithValidation)
	void ServerEndInteract();


	void Interact();

	//Information about the current state of the players interaction
	UPROPERTY()
	FInteractionData InteractionData;

	//Helper function to make grabbing interactable faster
	FORCEINLINE class UInteractionComponent* GetInteractable() const { return InteractionData.ViewedInteractionComponent; }

	FTimerHandle TimerHandle_Interact;

public:

	//True if we`re interacting with an item that has an interaction time (for example a lamp takes 2 seconds to turn on)
	bool IsInteracting() const;

	//Get the time till we interact with the current interactable
	float GetRemainingInteractTime() const;

protected:

	void MoveForward(float Val);
	void MoveRight(float Val);

	void LookUp(float Val);
	void Turn(float Val);

public:	


	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
