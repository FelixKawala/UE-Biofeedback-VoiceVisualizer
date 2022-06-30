// Fill out your copyright notice in the Description page of Project Settings.


#include "RESTfunctionLibraray.h"

#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"


void URESTfunctionLibraray::HttpRequest(const FString& url, const TArray<FString>& searchToken, FString& output)
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
    for (int i = 0; i < searchToken.Num(); ++i){
        if (i > 0) {
            newURL += TEXT("&") + searchToken[i];
        }
        else {
            newURL += TEXT("?") + searchToken[i];
        }
    }
    pRequest->SetURL(newURL);

    // Set the callback, which will execute when the HTTP call is complete
    pRequest->OnProcessRequestComplete().BindLambda(
        // Here, we "capture" the 'this' pointer (the "&"), so our lambda can call this
        // class's methods in the callback.
        [&output](
            FHttpRequestPtr pRequest,
            FHttpResponsePtr pResponse,
            bool connectedSuccessfully) mutable {

                if (connectedSuccessfully) {

                    // We should have a JSON response. Attempt to process it.
                    output = pResponse->GetContentAsString();
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

TArray<FString> URESTfunctionLibraray::ProcessJSON(const FString& ResponseContent)
{
    // Validate http called us back on the Game Thread...
    check(IsInGameThread());

    TSharedRef<TJsonReader<TCHAR>> jsonReader = TJsonReaderFactory<TCHAR>::Create(ResponseContent);
    TSharedPtr<FJsonObject> outObject;

    TArray<FString> returnArray;
    if (FJsonSerializer::Deserialize(jsonReader, outObject))
    {
        FString id;
        int i = 0;
        for (auto& jsonObject : outObject->GetArrayField(TEXT("data"))) {
            if (jsonObject->AsObject()->TryGetStringField(TEXT("id"), id)) {
                returnArray.Add(id);
                UE_LOG(LogTemp, Warning, TEXT("ID %d: %s"), i, *id);
            }
            i++;
        }
    }
    return returnArray;
}