#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EOSFunctionLibrary.generated.h"

// Delegate to notify blueprint of CreateSession result
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnCreateSessionResult, bool, bWasSuccessful);

UCLASS()
class CS301_TRIVIACARDGAME_API UEOSFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // EOS Login
    UFUNCTION(BlueprintCallable, Category = "EOS")
    static void LoginToEOS(const FString& LoginType);

    // EOS Sessions
    UFUNCTION(BlueprintCallable, Category = "EOS|Sessions")
    static void CreateSession(int32 MaxPlayers, FOnCreateSessionResult OnComplete);

    UFUNCTION(BlueprintCallable, Category = "EOS|Sessions")
    static void FindSessions();

    UFUNCTION(BlueprintCallable, Category = "EOS|Sessions")
    static void JoinFirstAvailableSession();
};