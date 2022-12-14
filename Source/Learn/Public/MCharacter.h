// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UAudioComponent;

UCLASS()
class LEARN_API AMCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* SpringArm;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* CameraComp;


public:
	// Sets default values for this character's properties
	AMCharacter();

	UFUNCTION()
		void AttackStatUp();
	UFUNCTION()
		void AttackUpTimer();
	UPROPERTY(VisibleAnywhere, Category = "stats")
		int Attack;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Stats")
		int Health;
	UFUNCTION()
	void HealthRestore();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ho")
		int Score;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
