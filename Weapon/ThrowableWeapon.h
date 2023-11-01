// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ThrowableWeapon.generated.h"

UCLASS()
class SURVIVALGAME_API AThrowableWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AThrowableWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY ( EditDefaultsOnly, Category = "Components" )
		class UStaticMeshComponent* ThrowableMesh;

	UPROPERTY ( EditDefaultsOnly, Category = "Components" )
		class UProjectileMovementComponent* ThrowableMovement;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
