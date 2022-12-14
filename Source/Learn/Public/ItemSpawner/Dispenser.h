// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Dispenser.generated.h"

class AAttackUp;
class AHealthUp;
class ABaseItem;
UCLASS()
class LEARN_API ADispenser : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADispenser();

	UPROPERTY(EditAnywhere, Category = "Mesh")
		UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere, Category = "Reference Point")
		USceneComponent* ReferencePoint;

	UPROPERTY(EditAnywhere, Category = "Container")
		ABaseItem* HolderItem;
	
	UFUNCTION()
		void ItemSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere, Category = "Spawn")
		TSubclassOf<AAttackUp>AttackItem;

	UPROPERTY(EditAnywhere, Category = "Spawn")
		TSubclassOf<AHealthUp>HealthItem;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
