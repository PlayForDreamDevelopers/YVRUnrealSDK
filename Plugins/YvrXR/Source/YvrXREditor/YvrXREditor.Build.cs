// Copyright 2020-2023 Yvr Technology Co., Ltd. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class YvrXREditor : ModuleRules
{
	public YvrXREditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateIncludePaths.AddRange(
	        new string[] {
				"YvrXRHMD/Private",
			});

        PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "InputCore",
                "ApplicationCore",
                "InputDevice",
                "BlueprintGraph",
                "AnimGraph",
				"ToolMenus",
			}
			);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects",
				"InputCore",
				"UnrealEd",
				"LevelEditor",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"YvrXRHMD",
			}
			);
	}
}
