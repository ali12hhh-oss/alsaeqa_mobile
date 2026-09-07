using UnrealBuildTool;

public class ALSAEQA : ModuleRules
{
    public ALSAEQA(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "AIModule",
            "ProceduralMeshComponent",
            "Niagara",
            "UMG"
        });

        PrivateDependencyModuleNames.AddRange(new[]
        {
            "EnhancedInput"
        });
    }
}
