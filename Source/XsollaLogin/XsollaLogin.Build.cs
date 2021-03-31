// Copyright 2021 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

using System.IO;
using UnrealBuildTool;

public class XsollaLogin : ModuleRules
{
    public XsollaLogin(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "HTTP",
                "Json",
                "JsonUtilities",
                "UMG",
                "OnlineSubsystem",
                "XsollaWebBrowser",
                "XsollaUtils"
            }
            );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
            }
            );

        if (Target.Platform == UnrealTargetPlatform.Android)
        {
            PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private/Android"));

            PrivateDependencyModuleNames.AddRange(new string[] { "Launch" });
            string PluginPath = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);
            AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(PluginPath, "XsollaLogin_UPL.xml"));
        }

        PublicDefinitions.Add("WITH_XSOLLA_LOGIN=1");
    }
}
