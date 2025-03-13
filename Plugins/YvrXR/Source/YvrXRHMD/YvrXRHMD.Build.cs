// Copyright 2020-2023 Yvr Technology Co., Ltd. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

namespace UnrealBuildTool.Rules
{
	public class YvrXRHMD : ModuleRules
	{
		public YvrXRHMD(ReadOnlyTargetRules Target) : base(Target)
        {
            var EngineDir = Path.GetFullPath(Target.RelativeEnginePath);
            PrivateIncludePaths.AddRange(
				new string[] {
					"YvrXRHMD/Private",
                    //EngineDir + "/Source/ThirdParty/OpenXR/include",
                    EngineDir + "/Source/Runtime/Renderer/Private",
                    EngineDir + "/Source/Runtime/OpenGLDrv/Private",
                    EngineDir + "/Source/Runtime/VulkanRHI/Private",
					// ... add other private include paths required here ...
					PluginDirectory + "/Include",
				}
			);


			if (Target.Platform == UnrealTargetPlatform.Win64)
            {
                PrivateIncludePaths.Add(EngineDir + "/Source/Runtime/VulkanRHI/Private/Windows");
            }
			else if (Target.Platform == UnrealTargetPlatform.Android  || Target.Platform == UnrealTargetPlatform.Linux)
            {
                PrivateIncludePaths.Add(EngineDir + "/Source/Runtime/VulkanRHI/Private/" + Target.Platform);
            }

			PublicDefinitions.Add("XR_NO_PROTOTYPES");
			//PublicDefinitions.Add("YVR_APPSW");
			PublicDependencyModuleNames.Add("HeadMountedDisplay");

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"CoreUObject",
					"Engine",
                    "BuildSettings",
                    "InputCore",
					"RHI",
                    "RHICore",
                    "RenderCore",
					"Renderer",
                    "Slate",
                    "SlateCore",
					"ProceduralMeshComponent",
					"Projects",
                    "XRBase",
                }
			);

			PublicIncludePathModuleNames.AddRange(
			   new string[]
			   {
					"Launch",
					"ProceduralMeshComponent",
                    "XRBase",
                    "RHICore",
               }
		    );

			if (Target.bBuildEditor == true)
            {
                PrivateDependencyModuleNames.Add("UnrealEd");
			}

			if (Target.Platform == UnrealTargetPlatform.Win64 || Target.Platform == UnrealTargetPlatform.Android)
            {
                PrivateDependencyModuleNames.AddRange(new string[] {
                    "VulkanRHI"
                });

                AddEngineThirdPartyPrivateStaticDependencies(Target, "Vulkan");

				PrivateDependencyModuleNames.AddRange(new string[] {
					"OpenGLDrv",
				});

				AddEngineThirdPartyPrivateStaticDependencies(Target, "OpenGL");


				string ModulePath = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);
				AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(ModulePath, "YvrXRHMD_APL.xml"));
			}
		}
	}
}
