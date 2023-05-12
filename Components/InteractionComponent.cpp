// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/InteractionComponent.h"
#include "Player/SurvivalCharacter.h"
#include "Widgets/InteractionWidget.h"

UInteractionComponent::UInteractionComponent()
{

	SetComponentTickEnabled(false);

	Interactiontime = 0.0f;
	InteractionDistance = 200.0f;
	InteractionableNameText = FText::FromString("Interactable Object");
	InteractionableActionText = FText::FromString("Interact");
	bAllowMulitpleInteractions = true;

	Space = EWidgetSpace::Screen;
	DrawSize = FIntPoint(600, 100);
	bDrawAtDesiredSize = true;

	SetActive(true);
	SetHiddenInGame(true);
}

void UInteractionComponent::SetInteractableNameText(const FText& NewNameText)
{
	InteractionableNameText = NewNameText;
	RefreshWidget();
}

void UInteractionComponent::SetInteractableActionText(const FText& NewActionText)
{
	InteractionableActionText = NewActionText;
	RefreshWidget();

}

void UInteractionComponent::Deactivate()
{
	Super::Deactivate();

	for (int32 i = Interactors.Num() - 1; i >= 0; --i)
	{
		if (ASurvivalCharacter* Interactor = Interactors[i])
		{
			EndFocus(Interactor);
			EndInteract(Interactor);
		}
	}
	Interactors.Empty();
}

bool UInteractionComponent::CanInteract(class ASurvivalCharacter* Character) const
{
	const bool bPlayerAlreadyInteracting = !bAllowMulitpleInteractions && Interactors.Num() >= 1;
	return !bPlayerAlreadyInteracting && IsActive() && GetOwner() != nullptr && Character != nullptr;
}

void UInteractionComponent::RefreshWidget()
{
	//Make sure the widget is initialized, and that we are displaying the right values (these may have changed)
	if (!bHiddenInGame && GetOwner()->GetNetMode() != NM_DedicatedServer)
	{
		if (UInteractionWidget* InteractionWidget = Cast<UInteractionWidget>(GetUserWidgetObject()))
		{
			InteractionWidget->UpdateInteractionWidget(this);
		}
	}
}

void UInteractionComponent::BeginFocus(class ASurvivalCharacter* Character)
{

	
	if (!IsActive() || !GetOwner() || !Character)
	{
		return;
	}

	OnBeginFocus.Broadcast(Character);

	SetHiddenInGame(false);
	
	if (!GetOwner()->HasAuthority())
	{
		TArray<UPrimitiveComponent*> PrimitiveComponents;
		GetOwner()->GetComponents<UPrimitiveComponent>(PrimitiveComponents);
		for (int32 i = 0; i < PrimitiveComponents.Num(); i++)
		{
			UPrimitiveComponent* Prim = PrimitiveComponents[i];
			if (Prim != nullptr)
			{
				Prim->SetRenderCustomDepth(true);
			}
		}
	}

	RefreshWidget();

}

void UInteractionComponent::EndFocus(class ASurvivalCharacter* Character)
{
	
	OnEndFocus.Broadcast(Character);

	SetHiddenInGame(true);

	if (!GetOwner()->HasAuthority())
	{
		TArray<UPrimitiveComponent*> PrimitiveComponents;
		GetOwner()->GetComponents<UPrimitiveComponent>(PrimitiveComponents);
		for (int32 i = 0; i < PrimitiveComponents.Num(); i++)
		{
			UPrimitiveComponent* Prim = PrimitiveComponents[i];
			if (Prim)
			{
				Prim->SetRenderCustomDepth(false);
			}
		}
	}

}

void UInteractionComponent::BeginInteract(class ASurvivalCharacter* Character)
{
	if (CanInteract(Character))
	{
		Interactors.AddUnique(Character);
		OnBeginFocus.Broadcast(Character);
	}
}

void UInteractionComponent::EndInteract(class ASurvivalCharacter* Character)
{
	Interactors.RemoveSingle(Character);
	OnEndInteract.Broadcast(Character);
}

void UInteractionComponent::Interact(class ASurvivalCharacter* Character)
{
	if (CanInteract(Character))
	{
		OnInteract.Broadcast(Character);
	}

}

float UInteractionComponent::GetInteractPercentage()
{
	if (Interactors.IsValidIndex(0))
	{
		if (ASurvivalCharacter* Interactor = Interactors[0])
		{
			if (Interactor && Interactor->IsInteracting())
			{
				return 1.f - FMath::Abs(Interactor->GetRemainingInteractTime() / Interactiontime);
			}
		}
	}
	return 0.f;
}
