// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "Engine/DataTable.h"
#include "ItemSpawn.generated.h"

USTRUCT(BlueprintType)
struct FlootableRow : public FTableRowBase
{
	GENERATED_BODY ()

public:

	//The Item(s) to spawn

	UPROPERTY ( EditDefaultsOnly, Category = "Loot" )
		TArray<TSubclassOf<class UItem>> Items;

	//The percetage of spawning this item, if we hit it on the roll
	UPROPERTY ( EditDefaultsOnly, Category = "Loot", meta = (ClampMin = 0.001, ClampMax = 1) )
	float Probability = 1.f;

};







/**
 * 
 */
UCLASS()
class SURVIVALGAME_API AItemSpawn : public ATargetPoint
{
	GENERATED_BODY()
	
};
