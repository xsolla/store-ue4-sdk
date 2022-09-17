// Copyright 2021 Xsolla Inc. All Rights Reserved.

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
                "XsollaUtils",
                "XsollaSettings"
            }
            );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore"
            }
            );

        if (Target.Platform == UnrealTargetPlatform.Android)
        {
            PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private/Android"));

            PrivateDependencyModuleNames.AddRange(new string[] { "Launch" });
            string PluginPath = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);
            AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(PluginPath, "XsollaLogin_UPL.xml"));
        }
        if (Target.Platform == UnrealTargetPlatform.IOS)
        {
            PublicAdditionalFrameworks.Add(
            new Framework(
                "XsollaSDKLoginKit",
                "../ThirdParty/IOS/XsollaSDKLoginKit.embeddedframework.zip"
            )
            );
            PublicAdditionalFrameworks.Add(
            new Framework(
                "XsollaSDKLoginKitObjectiveC",
                "../ThirdParty/IOS/XsollaSDKLoginKitObjectiveC.embeddedframework.zip"
            )
            );
            PublicAdditionalFrameworks.Add(
            new Framework(
                "XsollaSDKUtilities",
                "../ThirdParty/IOS/XsollaSDKUtilities.embeddedframework.zip"
            )
            );
        }

        PublicDefinitions.Add("WITH_XSOLLA_LOGIN=1");
    }
}
