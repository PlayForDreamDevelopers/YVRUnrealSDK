// Copyright 2020-2024 Yvr Technology Co., Ltd. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class YvrXRMR : ModuleRules
{
    public YvrXRMR(ReadOnlyTargetRules Target) : base(Target)
    {
        var EngineDir = Path.GetFullPath(Target.RelativeEnginePath);
        PrivateIncludePaths.AddRange(
            new string[] {
                    "YvrXRHMD/Private",
                    "YvrXRInput/Private",
                    PluginDirectory + "/Include"
            }
            );

        PrivateIncludePathModuleNames.AddRange(
            new string[]
            {
                "InputDevice",
                "HeadMountedDisplay",
                "ImageWrapper",
                "Engine"
            });

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "Slate",
                "SlateCore",
                "RHI",
                "VulkanRHI",
                "RenderCore",
                "MediaAssets",
                "HeadMountedDisplay",
                "YvrXRHMD",
                "YvrXRInput",
                "ProceduralMeshComponent",
            });
    }
}