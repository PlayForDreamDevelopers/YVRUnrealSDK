using System.IO;
using UnrealBuildTool;

public class YvrBusiness : ModuleRules
{
	public YvrBusiness(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDefinitions.Add("YVRBUSINESS_PACKAGE=1");
		PublicDependencyModuleNames.AddRange
		(
			new string[]
			{
			}
		);
		PrivateDependencyModuleNames.AddRange
		(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
                "RHI",
                "RenderCore"
            }
		);

        PrivateIncludePaths.AddRange(
            new string[] {
				PluginDirectory + "/Include",
            }
        );

        PublicIncludePathModuleNames.Add("Launch");
		if (Target.Platform == UnrealTargetPlatform.Android)
		{
			PrivateDependencyModuleNames.AddRange(new string[] {"Launch"});
			string PluginPath = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);
			AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(PluginPath, "YvrBusiness_APL.xml"));
		}
	}
}
