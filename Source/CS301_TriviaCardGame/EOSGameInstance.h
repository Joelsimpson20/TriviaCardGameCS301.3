#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
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

    UFUNCTION(BlueprintImplementableEvent, Category = "EOS")
    void OnSessionCreatedSuccess();
};


