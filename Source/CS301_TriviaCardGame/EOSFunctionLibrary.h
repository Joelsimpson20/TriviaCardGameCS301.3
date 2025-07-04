#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "EOSFunctionLibrary.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnEOSLoginComplete, bool, bWasSuccessful);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnEOSCreateSessionComplete, bool, bWasSuccessful);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnEOSJoinSessionComplete, bool, bWasSuccessful);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnEOSJoinFirstSessionComplete, bool, bWasSuccessful);

USTRUCT(BlueprintType)
struct FEOSBlueprintSessionResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString OwningUserName;

    UPROPERTY(BlueprintReadOnly)
    int32 Ping;

    int32 InternalIndex;
};

UCLASS()
class CS301_TRIVIACARDGAME_API UEOSFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    // Login 
    UFUNCTION(BlueprintCallable, Category = "EOS")
    static void LoginToEOS(const FString& LoginType, FOnEOSLoginComplete OnComplete);

    UFUNCTION(BlueprintPure, Category = "EOS")
    static FString GetEOSAccountName();

    // Session Creation
    UFUNCTION(BlueprintCallable, Category = "EOS")
    static void CreateSession(int32 MaxPlayers, FOnEOSCreateSessionComplete OnComplete);

	// Session Search
    UFUNCTION(BlueprintCallable, Category = "EOS")
    static void FindSessions(int32 MaxResults);

    UFUNCTION(BlueprintCallable, Category = "EOS")
    static TArray<FEOSBlueprintSessionResult> GetSessionResults();

	// JOoining Session
    UFUNCTION(BlueprintCallable, Category = "EOS")
    static void JoinSession(int32 SessionIndex, FOnEOSJoinSessionComplete OnComplete);

    UFUNCTION(BlueprintCallable, Category = "EOS")
    static void JoinFirstAvailableSession(FOnEOSJoinFirstSessionComplete OnComplete);

	// Destroy Session
    UFUNCTION(BlueprintCallable, Category = "EOS")
    static void DestroySession();

private:
    static TSharedPtr<FOnlineSessionSearch> CachedSessionSearch;
};
