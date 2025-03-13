// Copyright 2020-2024 Yvr Technology Co., Ltd. All Rights Reserved.
using System;
using System.IO;
using UnrealBuildTool;

public class YvrLivePreviewService : ModuleRules
{


    public YvrLivePreviewService(ReadOnlyTargetRules Target) : base(Target)
		{
			PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		// must be true if we want to use Dynamic cast
		bUseRTTI = true;

        string EnginePath = Path.GetFullPath(Target.RelativeEnginePath);
        PublicIncludePaths.AddRange(
				new string[] {
                    PluginDirectory + "/Include",
                    EnginePath+"Source/Runtime/Renderer/Private",
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
				"Projects",
                "RHI",
                "RenderCore",
                "Renderer",
					// ... add private dependencies that you statically link with here ...
				}
				);

			DynamicallyLoadedModuleNames.AddRange(
				new string[]
				{
					// ... add any modules that your module loads dynamically here ...
				}
				);

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

            string ProxyDirectory = Path.Combine(ModuleDirectory, "../ThirdParty/Proxy");

                PublicIncludePaths.Add(Path.Combine(ProxyDirectory, "include"));

                PublicAdditionalLibraries.Add(Path.Combine(ProxyDirectory, "lib/libboost_atomic-vc143-mt-x64-1_81.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ProxyDirectory, "lib/libboost_chrono-vc143-mt-x64-1_81.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ProxyDirectory, "lib/libboost_filesystem-vc143-mt-x64-1_81.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ProxyDirectory, "lib/libboost_log_setup-vc143-mt-x64-1_81.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ProxyDirectory, "lib/libboost_log-vc143-mt-x64-1_81.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ProxyDirectory, "lib/libboost_regex-vc143-mt-x64-1_81.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ProxyDirectory, "lib/libboost_thread-vc143-mt-x64-1_81.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ProxyDirectory, "lib/net_asio.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ProxyDirectory, "lib/spdlog.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ProxyDirectory, "lib/spdlog_log.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ProxyDirectory, "lib/yvrnetwork.lib"));

                PublicAdditionalLibraries.Add(Path.Combine(ProxyDirectory, "lib/logger.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ProxyDirectory, "lib/proxy.lib"));
            }


		}
	}
