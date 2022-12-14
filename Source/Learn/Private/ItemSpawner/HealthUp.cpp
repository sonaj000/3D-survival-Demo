// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSpawner/HealthUp.h"
#include "MCharacter.h"

void AHealthUp::PowerUp(AActor* MCharacter)
{
	/* get health component and add 50 health.basically. */
	AMCharacter* MC = Cast<AMCharacter>(MCharacter);
	MC->HealthRestore();
	Destroy();
}
