#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "EOSGameInstance.generated.h"

UCLASS(Blueprintable)
class CS301_TRIVIACARDGAME_API UEOSGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    virtual void Init() override;

    UFUNCTION(BlueprintCallable, Category = "EOS")
    void LoginWithEOS();

    UFUNCTION(BlueprintImplementableEvent, Category = "EOS")
    void OnSessionCreatedSuccess();
};

