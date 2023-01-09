// Fill out your copyright notice in the Description page of Project Settings.


#include "Points.h"
#include "MCharacter.h"
#include "Bird.h"

void APoints::PowerUp(AActor* MCharacter)
{
	AMCharacter* MC = Cast<AMCharacter>(MCharacter);
	MC->Score += 5;
	Destroy();
}

void APoints::BeginOverLap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(OtherActor) && OtherActor != this && OtherActor->IsA(AMCharacter::StaticClass())) //&& OtherActor->IsA(ACharacter::StaticClass())
	{
		UE_LOG(LogTemp, Warning, TEXT("destroy"));
		PowerUp(OtherActor);
	}

}
