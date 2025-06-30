using UnrealBuildTool;

public class CS301_TriviaCardGame : ModuleRules
{
    public CS301_TriviaCardGame(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
    "Core",
    "CoreUObject",
    "Engine",
    "InputCore",
    "OnlineSubsystem",
    "OnlineSubsystemUtils",
    "Networking",
    "Sockets"
});


        DynamicallyLoadedModuleNames.AddRange(new string[] {
            "OnlineSubsystemEOS"
        });

        bEnableExceptions = true;
    }
}
