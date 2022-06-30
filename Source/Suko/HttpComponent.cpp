// Fill out your copyright notice in the Description page of Project Settings.


#include "HttpComponent.h"

#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"

// Sets default values for this component's properties
UHttpComponent::UHttpComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;

    // ...
}


// Called when the game starts
void UHttpComponent::BeginPlay()
{
    Super::BeginPlay();
    const TArray<FString> searchToken = { TEXT("purity=100"), TEXT("sorting=favorites") };
    HttpRequest(TEXT("https://wallhaven.cc/api/v1/search"), searchToken);
    // ...

}


// Called every frame
void UHttpComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // ...
}

void UHttpComponent::HttpRequest(const FString& url, const TArray<FString>& searchToken)
{
    FHttpModule& httpModule = FHttpModule::Get();

    // Create an http request
    // The request will execute asynchronously, and call us back on the Lambda below
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> pRequest = httpModule.CreateRequest();

    // This is where we set the HTTP method (GET, POST, etc)
    // The Space-Track.org REST API exposes a "POST" method we can use to get
    // general perturbations data about objects orbiting Earth
    pRequest->SetVerb(TEXT("GET"));

    // We'll need to tell the server what type of content to expect in the POST data
    //pRequest->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));

    //FString RequestContent = TEXT("identity=") + NewUser + TEXT("&password=") + NewPassword + TEXT("&query=") + uriQuery;
    // Set the POST content, which contains:
    // * Identity/password credentials for authentication
    // * A REST API query
    //   This allows us to login and get the results is a single call
    //   Otherwise, we'd need to manage session cookies across multiple calls.
    //pRequest->SetContentAsString(RequestContent);

    // Set the http URL
    FString newURL = url;
    for (int i = 0; i < searchToken.Num(); ++i) {
        if (i <= 0)
            newURL += TEXT("?");
        else
            newURL += TEXT("&");
        newURL += searchToken[i];
    }
    pRequest->SetURL(newURL);

    // Set the callback, which will execute when the HTTP call is complete
    pRequest->OnProcessRequestComplete().BindLambda(
        // Here, we "capture" the 'this' pointer (the "&"), so our lambda can call this
        // class's methods in the callback.
        [&](
            FHttpRequestPtr pRequest,
            FHttpResponsePtr pResponse,
            bool connectedSuccessfully) mutable {

                if (connectedSuccessfully) {
                    // We should have a JSON response. Attempt to process it.
                    ProcessJSON(pResponse->GetContentAsString());
                }
                else {
                    switch (pRequest->GetStatus()) {
                    case EHttpRequestStatus::Failed_ConnectionError:
                        UE_LOG(LogTemp, Error, TEXT("Connection failed."));
                    default:
                        UE_LOG(LogTemp, Error, TEXT("Request failed."));
                    }
                }
        });

    // Finally, submit the request for processing
    pRequest->ProcessRequest();
}

void UHttpComponent::ProcessJSON(const FString& ResponseContent)
{
    // Validate http called us back on the Game Thread...
    check(IsInGameThread());

    TSharedRef<TJsonReader<TCHAR>> jsonReader = TJsonReaderFactory<TCHAR>::Create(ResponseContent);
    TSharedPtr<FJsonObject> outObject;

    if (FJsonSerializer::Deserialize(jsonReader, outObject))
    {
        FString id;
        int i = 0;
        for (auto& jsonObject : outObject->GetArrayField(TEXT("data"))) {
            if (jsonObject->AsObject()->TryGetStringField(TEXT("id"), id)){
                UE_LOG(LogTemp, Warning, TEXT("ID %d: %s"), i, *id);
            }
            i++;
        }
    }
}