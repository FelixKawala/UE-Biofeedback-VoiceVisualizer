// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/InstancedStaticMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "VoicePitchDisplay.generated.h"

UCLASS()
class SUKO_API AVoicePitchDisplay : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AVoicePitchDisplay();

	// Sets the display hand to point at appropriate position/value.
	UFUNCTION(BlueprintCallable)
	void SetDisplayHand(float value, float maxValue, float minValue = 0);

	UFUNCTION(BlueprintCallable)
	void ShowTarget(int targetIndex);
	UFUNCTION(BlueprintCallable)
	void HideTarget(int targetIndex);

	UFUNCTION(BlueprintCallable)
	void ShowNextTarget(int targetIndex);
	UFUNCTION(BlueprintCallable)
	void HideNextTarget(int targetIndex);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void RotateTargetPitch(UInstancedStaticMeshComponent* target, int targetIndex, double pitch);

	static void StaticMeshToInstance(UStaticMeshComponent* staticMesh, UInstancedStaticMeshComponent* instance);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* m_DisplayBase;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* m_DisplayHand;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* m_DisplayTarget;
	UPROPERTY(VisibleInstanceOnly)
	UInstancedStaticMeshComponent* m_DisplayTargetInstanced;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* m_DisplayNextTarget;
	UPROPERTY(VisibleInstanceOnly)
	UInstancedStaticMeshComponent* m_DisplayNextTargetInstanced;

	UPROPERTY(EditAnywhere)
	int m_TargetAmount = 12;
};
