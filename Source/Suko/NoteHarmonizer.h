// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NoteHarmonizer.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SUKO_API UNoteHarmonizer : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UNoteHarmonizer();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	


protected:

		
};
