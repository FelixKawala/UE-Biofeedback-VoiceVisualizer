// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "RESTfunctionLibraray.generated.h"

/**
 *
 */
UCLASS()
class ADUID_API URESTfunctionLibraray : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable)
	static void HttpRequest(const FString& url, const TArray<FString>& searchToken, AVoicePitchDisplay* display);
	UFUNCTION(BlueprintCallable)
	static TArray<FString> ProcessJSON(const FString& ResponseContent, AVoicePitchDisplay* display);
};
