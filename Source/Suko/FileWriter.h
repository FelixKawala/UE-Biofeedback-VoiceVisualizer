// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "FileWriter.generated.h"

/**
 *
 */
UCLASS()
class SUKO_API UFileWriter : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:
	static const FString ParticipantNumberFilePath_;
	static FString participantString_;
	static FString conditionString_;
	static uint32 participantNumber_;

	static uint32 GetCurrentParticipantNumber();
	static bool AdvanceCurrentParticipantNumber();

	static FString BuildPath();

public:
	UFUNCTION(BlueprintCallable)
	static bool NewParticipant();

	UFUNCTION(BlueprintCallable)
	static void NewCondition(FString condition);

	UFUNCTION(BlueprintCallable)
	static bool WriteStrings(UPARAM(ref) TArray<FString>& strings, FString fileName);

	UFUNCTION(BlueprintCallable)
	static bool WriteStringsToCSV(TArray<FString> strings);
};
