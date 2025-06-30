#include "EOSFunctionLibrary.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineIdentityInterface.h"          
#include "OnlineSubsystemUtils.h"                        
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

#define SEARCH_KEYWORDS 0x0002 

TSharedPtr<FOnlineSessionSearch> UEOSFunctionLibrary::CachedSessionSearch;

void UEOSFunctionLibrary::LoginToEOS(const FString& LoginType, FOnEOSLoginComplete OnComplete)
{
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (!Subsystem) { OnComplete.ExecuteIfBound(false); return; }

    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    if (!Identity.IsValid()) { OnComplete.ExecuteIfBound(false); return; }

    FOnlineAccountCredentials Credentials;
    Credentials.Type = LoginType;
    Credentials.Id = FString();
    Credentials.Token = FString();

    Identity->OnLoginCompleteDelegates->AddLambda([OnComplete](int32, bool bWasSuccessful, const FUniqueNetId& UserId, const FString&)
        {
            UE_LOG(LogTemp, Log, TEXT("EOS: Login %s"), bWasSuccessful ? TEXT("successful") : TEXT("failed"));
            OnComplete.ExecuteIfBound(bWasSuccessful);
        });

    Identity->Login(0, Credentials);
}

FString UEOSFunctionLibrary::GetEOSAccountName()
{
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (!Subsystem) return "No Subsystem";

    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    if (!Identity.IsValid()) return "No Identity";

    TSharedPtr<const FUniqueNetId> UserId = Identity->GetUniquePlayerId(0);
    if (!UserId.IsValid()) return "No User ID";

    return Identity->GetPlayerNickname(*UserId);
}

void UEOSFunctionLibrary::CreateSession(int32 MaxPlayers, FOnEOSCreateSessionComplete OnComplete)
{
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (!Subsystem) { OnComplete.ExecuteIfBound(false); return; }

    IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
    if (!Session.IsValid()) { OnComplete.ExecuteIfBound(false); return; }

    FOnlineSessionSettings Settings;
    Settings.NumPublicConnections = MaxPlayers;
    Settings.bIsLANMatch = false;
    Settings.bShouldAdvertise = true;
    Settings.bUsesPresence = true;
    Settings.bAllowJoinViaPresence = true;
    Settings.bAllowJoinInProgress = true;

    Session->OnCreateSessionCompleteDelegates.AddLambda([OnComplete](FName, bool bWasSuccessful)
        {
            if (bWasSuccessful)
            {
                UE_LOG(LogTemp, Log, TEXT("EOS: Session created successfully."));
                UGameplayStatics::OpenLevel(GWorld, "LobbyMap", true, "listen");
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("EOS: Failed to create session."));
            }
            OnComplete.ExecuteIfBound(bWasSuccessful);
        });

    TSharedPtr<const FUniqueNetId> UserId = Subsystem->GetIdentityInterface()->GetUniquePlayerId(0);
    Session->CreateSession(*UserId, NAME_GameSession, Settings);
}

void UEOSFunctionLibrary::FindSessions(int32 MaxResults)
{
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (!Subsystem) return;

    IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
    if (!Session.IsValid()) return;

    CachedSessionSearch = MakeShareable(new FOnlineSessionSearch());
    CachedSessionSearch->QuerySettings.Set(FName("SEARCH_KEYWORDS"), FString("TriviaSession"), EOnlineComparisonOp::Equals);

    Session->OnFindSessionsCompleteDelegates.AddLambda([](bool bWasSuccessful)
        {
            if (bWasSuccessful && CachedSessionSearch->SearchResults.Num() > 0)
            {
                UE_LOG(LogTemp, Log, TEXT("EOS: %d session(s) found."), CachedSessionSearch->SearchResults.Num());
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("EOS: No sessions found."));
            }
        });

    TSharedPtr<const FUniqueNetId> UserId = Subsystem->GetIdentityInterface()->GetUniquePlayerId(0);
    Session->FindSessions(*UserId, CachedSessionSearch.ToSharedRef());
}

TArray<FEOSBlueprintSessionResult> UEOSFunctionLibrary::GetSessionResults()
{
    TArray<FEOSBlueprintSessionResult> Out;

    if (!CachedSessionSearch.IsValid()) return Out;

    for (int32 i = 0; i < CachedSessionSearch->SearchResults.Num(); i++)
    {
        const auto& Result = CachedSessionSearch->SearchResults[i];
        FEOSBlueprintSessionResult Entry;
        Entry.OwningUserName = Result.Session.OwningUserName;
        Entry.Ping = Result.PingInMs;
        Entry.InternalIndex = i;
        Out.Add(Entry);
    }

    return Out;
}

void UEOSFunctionLibrary::JoinSession(int32 SessionIndex, FOnEOSJoinSessionComplete OnComplete)
{
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (!Subsystem || !CachedSessionSearch.IsValid()) { OnComplete.ExecuteIfBound(false); return; }

    IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
    if (!Session.IsValid()) { OnComplete.ExecuteIfBound(false); return; }

    if (!CachedSessionSearch->SearchResults.IsValidIndex(SessionIndex))
    {
        UE_LOG(LogTemp, Warning, TEXT("EOS: Invalid session index %d"), SessionIndex);
        OnComplete.ExecuteIfBound(false);
        return;
    }

    Session->OnJoinSessionCompleteDelegates.AddLambda([OnComplete](FName SessionName, EOnJoinSessionCompleteResult::Type Result)
        {
            bool bSuccess = Result == EOnJoinSessionCompleteResult::Success;
            if (bSuccess)
            {
                FString ConnectInfo;
                IOnlineSubsystem::Get()->GetSessionInterface()->GetResolvedConnectString(SessionName, ConnectInfo);
                APlayerController* PC = UGameplayStatics::GetPlayerController(GWorld, 0);
                if (PC)
                {
                    PC->ClientTravel(ConnectInfo, ETravelType::TRAVEL_Absolute);
                    UE_LOG(LogTemp, Log, TEXT("EOS: Joining session..."));
                }
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("EOS: Failed to join session."));
            }
            OnComplete.ExecuteIfBound(bSuccess);
        });

    TSharedPtr<const FUniqueNetId> UserId = Subsystem->GetIdentityInterface()->GetUniquePlayerId(0);
    Session->JoinSession(*UserId, NAME_GameSession, CachedSessionSearch->SearchResults[SessionIndex]);
}

void UEOSFunctionLibrary::JoinFirstAvailableSession(FOnEOSJoinFirstSessionComplete OnComplete)
{
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (!Subsystem) { OnComplete.ExecuteIfBound(false); return; }

    IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
    if (!Session.IsValid()) { OnComplete.ExecuteIfBound(false); return; }

    CachedSessionSearch = MakeShareable(new FOnlineSessionSearch());
    CachedSessionSearch->bIsLanQuery = false;
    CachedSessionSearch->MaxSearchResults = 10;
    CachedSessionSearch->QuerySettings.Set(FName("SEARCH_KEYWORDS"), FString("TriviaSession"), EOnlineComparisonOp::Equals);

    Session->OnFindSessionsCompleteDelegates.AddLambda(
        [Session, OnComplete](bool bWasSuccessful)
        {
            if (bWasSuccessful && CachedSessionSearch->SearchResults.Num() > 0)
            {
                Session->OnJoinSessionCompleteDelegates.AddLambda(
                    [OnComplete](FName, EOnJoinSessionCompleteResult::Type Result)
                    {
                        const bool bJoinSuccess = Result == EOnJoinSessionCompleteResult::Success;
                        UE_LOG(LogTemp, Log, TEXT("EOS: Join first session %s"), bJoinSuccess ? TEXT("Success") : TEXT("Fail"));
                        OnComplete.ExecuteIfBound(bJoinSuccess);
                    });

                Session->JoinSession(0, NAME_GameSession, CachedSessionSearch->SearchResults[0]);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("EOS: No available sessions to join."));
                OnComplete.ExecuteIfBound(false);
            }
        });

    TSharedPtr<const FUniqueNetId> UserId = Subsystem->GetIdentityInterface()->GetUniquePlayerId(0);
    Session->FindSessions(*UserId, CachedSessionSearch.ToSharedRef());
}

void UEOSFunctionLibrary::DestroySession()
{
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (!Subsystem) return;

    IOnlineSessionPtr Sessions = Subsystem->GetSessionInterface();
    if (!Sessions.IsValid()) return;

    Sessions->DestroySession(NAME_GameSession);
}


