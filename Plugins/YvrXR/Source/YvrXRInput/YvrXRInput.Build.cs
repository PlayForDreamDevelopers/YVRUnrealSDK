// Copyright 2020-2023 Yvr Technology Co., Ltd. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

namespace UnrealBuildTool.Rules
{
    public class YvrXRInput : ModuleRules
    {
        public YvrXRInput(ReadOnlyTargetRules Target) : base(Target)
        {
            var EngineDir = Path.GetFullPath(Target.RelativeEnginePath);
            PrivateIncludePaths.AddRange(
                new string[] {
                    "YvrXRHMD/Private",
                    EngineDir + "/Source/Runtime/Renderer/Private",
                    //EngineDir + "/Source/ThirdParty/OpenXR/include",
					// ... add other private include paths required here ...
                    PluginDirectory + "/Include",
                }
                );

            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "Core",
                    "CoreUObject",
					"ApplicationCore",
                    "Engine",
                    "InputDevice",
                    "InputCore",
                    "HeadMountedDisplay",
                    "YvrXRHMD",
                }
                );

            //AddEngineThirdPartyPrivateStaticDependencies(Target, "OpenXR");

            if (Target.bBuildEditor == true)
            {
                PrivateDependencyModuleNames.Add("UnrealEd");
            }
        }
    }
}
