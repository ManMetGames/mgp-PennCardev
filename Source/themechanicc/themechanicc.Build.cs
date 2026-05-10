// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class themechanicc : ModuleRules
{
	public themechanicc(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG" });
	}
}
