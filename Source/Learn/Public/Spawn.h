// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Spawn.generated.h"

class UBoxComponent;
class APoints;
class ABird;

UCLASS()
class LEARN_API ASpawn : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawn();

	UPROPERTY(EditAnywhere, Category = "Spawn")
		TSubclassOf<APoints>Point;

	UPROPERTY(EditAnywhere, Category = "Spawn")
		TSubclassOf<ABird>Birdie;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
		UBoxComponent* SpawnVolume;

	UFUNCTION()
		FVector GetPoint();

	UFUNCTION()
		void SpawnPoints();

	UFUNCTION()
		void SpawnBirds();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
