#include "EOSGameInstance.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"

void UEOSGameInstance::Init()
{
    Super::Init();
    UE_LOG(LogTemp, Log, TEXT("EOSGameInstance initialized"));
}

void UEOSGameInstance::LoginWithEOS()
{
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (Subsystem)
    {
        IOnlineIdentityPtr IdentityInterface = Subsystem->GetIdentityInterface();
        if (IdentityInterface.IsValid())
        {
            FOnlineAccountCredentials Credentials;
            Credentials.Type = "accountportal"; // or "developer" for testing
            Credentials.Id = "";
            Credentials.Token = "";

            IdentityInterface->Login(0, Credentials);
            UE_LOG(LogTemp, Log, TEXT("LoginWithEOS() called."));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Identity interface invalid"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("OnlineSubsystem not found"));
    }
}