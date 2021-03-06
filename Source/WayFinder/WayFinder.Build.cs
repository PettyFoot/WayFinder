// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class WayFinder : ModuleRules
{
	public WayFinder(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", 
			"ProceduralMeshComponent", "HeadMountedDisplay", "UMG" });
	}
}
