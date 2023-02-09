// Fill out your copyright notice in the Description page of Project Settings.


#include "StaminaComponent.h"

// Sets default values for this component's properties
UStaminaComponent::UStaminaComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UStaminaComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UStaminaComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	stamina = 100;

	GetWorld()->GetTimerManager().SetTimer(StaminaRegeneration, this, &UStaminaComponent::RegenerateStamina, 1.0f, true);
	GetWorld()->GetTimerManager().PauseTimer(StaminaRegeneration);
}

void UStaminaComponent::ControlStamina(bool isMoving)
{
	if (isMoving)
	{
		UE_LOG(LogTemp, Warning, TEXT("pausing regen for some reason"));
		GetWorld()->GetTimerManager().PauseTimer(StaminaRegeneration);

	}
	else if (!isMoving)
	{
		UE_LOG(LogTemp, Warning, TEXT("unpausing regen for some reason"));
		GetWorld()->GetTimerManager().UnPauseTimer(StaminaRegeneration);
	}
}

void UStaminaComponent::RegenerateStamina()
{
	if (stamina >= 100.0f)
	{
		stamina = 100.0f;
	}
	else if (stamina <= 0.0f)
	{
		stamina = 0.0f;
		stamina += 5;
	}
	else
	{
		stamina += 5;
		UE_LOG(LogTemp, Warning, TEXT("regen stamina stamina :%s"), *FString::SanitizeFloat(stamina));
	}
}

void UStaminaComponent::LowerStamina()
{
	if (stamina <= 0.0f)
	{
		stamina = 0.0f;
		return;
	}
	else
	{
		stamina -= 10;
	}
	UE_LOG(LogTemp, Warning, TEXT("lowering stamina :%s"), *FString::SanitizeFloat(stamina));
}

float UStaminaComponent::GetStamina()
{
	return stamina;
}

