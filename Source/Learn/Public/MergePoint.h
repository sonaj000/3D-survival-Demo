// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MergePoint.generated.h"

class ABird;
class UBoxComponent;
class AFlockManager;
class ABigBird;

UCLASS()
class LEARN_API AMergePoint : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMergePoint();

	UPROPERTY(EditAnywhere, Category = "Mesh")
		UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
		UBoxComponent* MergePerimeter;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	TQueue<AActor*>MergerQueue;

	UFUNCTION()
		virtual void BeginOverLap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void MergeBirds();

	UPROPERTY()
		int Counter;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Flocking")
		AFlockManager* Manager;

	UFUNCTION()
		void CreateBird(ABird* bird0, ABird* bird1, FVector spawnloc, int CountNum);

	UPROPERTY(EditDefaultsOnly, Category = "bigbird")
		TSubclassOf<ABigBird>BigB;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
