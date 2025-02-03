// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Project_D : ModuleRules
{
	public Project_D(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "MotionWarping", "CableComponent", "UMG", "AIModule", "Niagara", "LevelSequence", "MovieScene" });
	}
}
