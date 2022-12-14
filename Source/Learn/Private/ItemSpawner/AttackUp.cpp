// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSpawner/AttackUp.h"
#include "MCharacter.h"



void AAttackUp::PowerUp(AActor* MCharacter)
{
	//increase char attack dmg by 5 to 10 for 10 seconds. 
	const int increase = 5;
	AMCharacter* MC = Cast<AMCharacter>(MCharacter);
	MC->AttackUpTimer();
	Destroy();
	//hide it from game? and then destroy afgter 10 seconds. 
}
