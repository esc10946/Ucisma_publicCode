// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UCISMA : ModuleRules
{
	public UCISMA(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "Niagara", "Slate", "SlateCore","UnrealEd","LevelEditor"});

        //PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
    }
}
