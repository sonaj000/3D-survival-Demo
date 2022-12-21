// Fill out your copyright notice in the Description page of Project Settings.


#include "BigBird.h"
#include "BirdController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HealthComponent.h"

void ABigBird::ScaleBird(int MNum)
{

	if (IsValid(this->GetController()))
	{
		AActor* H = Cast<AActor>(GetController()->GetPawn());
		ABirdController* mind = Cast<ABirdController>(GetController());
		UStaticMeshComponent* sm = Cast<UStaticMeshComponent>(H->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		float rescale = float(MNum);
		switch (MNum)
		{
		case 2:
			MergeNum = 2;
			BirdBar->Health = 15;
			DamageNum = 10;
			sm->SetWorldScale3D(FVector(rescale,rescale,rescale));
			sm->SetRelativeScale3D(FVector(rescale, rescale, rescale));
			GetCharacterMovement()->MaxWalkSpeed = 700.0f;
			mind->testarray.Empty();
			mind->testarray.Add(this);
			break;

		case 3:
			MergeNum = 3;
			BirdBar->Health = 20;
			DamageNum = 15;
			sm->SetWorldScale3D(FVector(rescale, rescale, rescale));
			sm->SetRelativeScale3D(FVector(rescale, rescale, rescale));
			GetCharacterMovement()->MaxWalkSpeed = 800.0f;
			mind->testarray.Empty();
			mind->testarray.Add(this);
			break;

		case 4:
			MergeNum = 4;
			BirdBar->Health = 25;
			DamageNum = 20;
			sm->SetWorldScale3D(FVector(rescale, rescale, rescale));
			sm->SetRelativeScale3D(FVector(rescale, rescale, rescale));
			GetCharacterMovement()->MaxWalkSpeed = 900.0f;
			mind->testarray.Empty();
			mind->testarray.Add(this);
			break;
		}
	}

}
