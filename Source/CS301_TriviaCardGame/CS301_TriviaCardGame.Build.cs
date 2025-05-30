using UnrealBuildTool;

public class CS301_TriviaCardGame : ModuleRules
{
    public CS301_TriviaCardGame(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core", "CoreUObject", "Engine", "InputCore", "OnlineSubsystem", "OnlineSubsystemUtils"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "OnlineSubsystemEOS"
        });
    }
}
