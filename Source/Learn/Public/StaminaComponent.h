// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StaminaComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LEARN_API UStaminaComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStaminaComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


	FTimerHandle StaminaRegeneration;

	FTimerHandle StaminaLower;
	FTimerDelegate BindStamina;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "stamina")
		float stamina;

	UPROPERTY()
		float StaminaAmount;

	UFUNCTION()
		void ControlStamina(bool isMoving);

	UFUNCTION()
		void RegenerateStamina();

	UFUNCTION()
		void LowerStamina(float amount);

	UFUNCTION()
		float GetStamina();
};
