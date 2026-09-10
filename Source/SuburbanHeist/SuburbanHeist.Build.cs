using UnrealBuildTool;

public class SuburbanHeist : ModuleRules
{
	public SuburbanHeist(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"GameplayTasks",
			"UMG",
			"Slate",
			"SlateCore",
			"NavigationSystem",
			"OnlineSubsystem",
			"OnlineSubsystemUtils"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"AudioCaptureCore",
			"AudioCapture",
			"AudioMixer"
		});

		PublicIncludePaths.AddRange(new string[] { "SuburbanHeist/Public" });
		PrivateIncludePaths.AddRange(new string[] { "SuburbanHeist/Private" });
	}
}
