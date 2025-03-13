// Copyright 2020-2024 Yvr Technology Co., Ltd. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class YvrLivePreviewInput : ModuleRules
{
	public YvrLivePreviewInput(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);

		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "InputDevice",
                "HeadMountedDisplay",
                "YvrLivePreviewService",
				// ... add other public dependencies that you statically link with here ...
			}
			);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
                "InputCore",
                "HeadMountedDisplay",
                "YvrLivePreviewHMD",
				// ... add private dependencies that you statically link with here ...
			}
			);

		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

        //D3D11RHIPrivate.h
        string EnginePath = Path.GetFullPath(Target.RelativeEnginePath);
        if (Target.Platform == UnrealTargetPlatform.Win64 || Target.IsInPlatformGroup(UnrealPlatformGroup.Linux))
        {
            AddEngineThirdPartyPrivateStaticDependencies(Target, "NVAftermath");
        }

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                            "D3D11RHI",
                            "D3D12RHI",
                });

            PrivateIncludePaths.AddRange(
                new string[]
                {
                            EnginePath+"Source/Runtime/Windows/D3D11RHI/Private",
                            EnginePath+"Source/Runtime/Windows/D3D11RHI/Private/Windows",
                            EnginePath+"Source/Runtime/D3D12RHI/Private",
                            EnginePath+"Source/Runtime/D3D12RHI/Private/Windows",
                });

            AddEngineThirdPartyPrivateStaticDependencies(Target, "DX11");
            AddEngineThirdPartyPrivateStaticDependencies(Target, "DX12");
            AddEngineThirdPartyPrivateStaticDependencies(Target, "OpenVR");
            AddEngineThirdPartyPrivateStaticDependencies(Target, "NVAftermath");
            AddEngineThirdPartyPrivateStaticDependencies(Target, "IntelMetricsDiscovery");

            BuildVersion Version;
            if (!BuildVersion.TryRead(BuildVersion.GetDefaultFileName(), out Version))
            {
                Version = new BuildVersion();
            }
            if (Version.MinorVersion > 24)
            {
                AddEngineThirdPartyPrivateStaticDependencies(Target, "IntelExtensionsFramework");
            }
        }
    }
}
