// Fill out your copyright notice in the Description page of Project Settings.


#include "ThrowableWeapon.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AThrowableWeapon::AThrowableWeapon()
{
	ThrowableMesh = CreateDefaultSubobject<UStaticMeshComponent> ( "ThrowableMesh" );
	SetRootComponent ( ThrowableMesh );

	ThrowableMovement = CreateDefaultSubobject<UProjectileMovementComponent> ( "ThrowableMovement" );
	ThrowableMovement->InitialSpeed = 1000.f;
	SetReplicates ( true );
	SetReplicateMovement ( true );
}

// Called when the game starts or when spawned
void AThrowableWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AThrowableWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

