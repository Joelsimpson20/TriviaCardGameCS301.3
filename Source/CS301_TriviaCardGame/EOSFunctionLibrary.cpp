#include "EOSFunctionLibrary.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Kismet/GameplayStatics.h"

void UEOSFunctionLibrary::LoginToEOS(const FString& LoginType)
{
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (!Subsystem)
    {
        UE_LOG(LogTemp, Error, TEXT("EOS: OnlineSubsystem not found"));
        return;
    }

    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    if (!Identity.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("EOS: Identity Interface not valid"));
        return;
    }

    FOnlineAccountCredentials Credentials;
    Credentials.Type = LoginType;
    Credentials.Id = TEXT("");
    Credentials.Token = TEXT("");

    Identity->OnLoginCompleteDelegates->AddLambda([](int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
        {
            if (bWasSuccessful)
            {
                UE_LOG(LogTemp, Log, TEXT("EOS Login Successful! UserId: %s"), *UserId.ToString());
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("EOS Login Failed: %s"), *Error);
            }
        });

    Identity->Login(0, Credentials);
}

void UEOSFunctionLibrary::CreateSession(int32 MaxPlayers, FOnCreateSessionResult OnComplete)
{
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (!Subsystem) return;

    IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
    if (!SessionInterface.IsValid()) return;

    FOnlineSessionSettings SessionSettings;
    SessionSettings.bIsLANMatch = false;
    SessionSettings.NumPublicConnections = MaxPlayers;
    SessionSettings.bShouldAdvertise = true;
    SessionSettings.bUsesPresence = true;
    SessionSettings.bAllowJoinInProgress = true;
    SessionSettings.bAllowJoinViaPresence = true;

    SessionInterface->OnCreateSessionCompleteDelegates.AddLambda([OnComplete](FName SessionName, bool bWasSuccessful)
        {
            if (bWasSuccessful)
            {
                UE_LOG(LogTemp, Log, TEXT("EOS: Session created successfully."));
                UGameplayStatics::OpenLevel(GWorld, "TriviaCardfGameBaseLevel", true, "listen");
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("EOS: Failed to create session."));
            }

            OnComplete.ExecuteIfBound(bWasSuccessful);
        });

    SessionInterface->CreateSession(0, NAME_GameSession, SessionSettings);
}

void UEOSFunctionLibrary::FindSessions()
{
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (!Subsystem) return;

    IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
    if (!SessionInterface.IsValid()) return;

    TSharedRef<FOnlineSessionSearch> SearchSettings = MakeShareable(new FOnlineSessionSearch());
    SearchSettings->MaxSearchResults = 10;
    SearchSettings->bIsLanQuery = false;
    SearchSettings->QuerySettings.Set(FName("presence"), true, EOnlineComparisonOp::Equals);

    SessionInterface->OnFindSessionsCompleteDelegates.AddLambda([SearchSettings](bool bWasSuccessful)
        {
            if (!bWasSuccessful || SearchSettings->SearchResults.Num() == 0)
            {
                UE_LOG(LogTemp, Warning, TEXT("EOS: No sessions found."));
                return;
            }

            UE_LOG(LogTemp, Log, TEXT("EOS: Found %d session(s)."), SearchSettings->SearchResults.Num());

            IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
            if (!Subsystem) return;

            IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
            if (!SessionInterface.IsValid()) return;

            SessionInterface->OnJoinSessionCompleteDelegates.AddLambda([](FName SessionName, EOnJoinSessionCompleteResult::Type Result)
                {
                    FString ConnectString;
                    if (IOnlineSubsystem::Get()->GetSessionInterface()->GetResolvedConnectString(SessionName, ConnectString))
                    {
                        APlayerController* PC = UGameplayStatics::GetPlayerController(GWorld, 0);
                        if (PC)
                        {
                            UE_LOG(LogTemp, Log, TEXT("EOS: Joining session at %s"), *ConnectString);
                            PC->ClientTravel(ConnectString, ETravelType::TRAVEL_Absolute);
                        }
                    }
                    else
                    {
                        UE_LOG(LogTemp, Error, TEXT("EOS: Failed to resolve connect string."));
                    }
                });

            SessionInterface->JoinSession(0, NAME_GameSession, SearchSettings->SearchResults[0]);
        });

    SessionInterface->FindSessions(0, SearchSettings);
}

void UEOSFunctionLibrary::JoinFirstAvailableSession()
{
    FindSessions(); // Simple wrapper for now
}
