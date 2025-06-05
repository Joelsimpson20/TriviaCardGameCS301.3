#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OnlineSubsystemTypes.h"
#include "EOSFunctionLibrary.generated.h"

// Delegate to notify blueprint of CreateSession result
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnCreateSessionResult, bool, bWasSuccessful);

UCLASS()
class CS301_TRIVIACARDGAME_API UEOSFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /** Logs in to EOS using AccountPortal login */
    UFUNCTION(BlueprintCallable, Category = "EOS")
    static void LoginToEOS();

    /** Creates an EOS session */
    UFUNCTION(BlueprintCallable, Category = "EOS|Sessions")
    static void CreateSession(int32 MaxPlayers, FOnCreateSessionResult OnComplete);

    /** Searches for available EOS sessions */
    UFUNCTION(BlueprintCallable, Category = "EOS|Sessions")
    static void FindSessions(APlayerController* PlayerController, int32 MaxResults, bool bIsLAN, bool bUsePresence);

    /** Attempts to join the first available session found by FindSessions */
    UFUNCTION(BlueprintCallable, Category = "EOS|Sessions")
    static void JoinFirstAvailableSession();
};
