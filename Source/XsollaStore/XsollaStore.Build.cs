// Copyright 2023 Xsolla Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class XsollaStore : ModuleRules
{
    public XsollaStore(ReadOnlyTargetRules Target) : base(Target)
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
                "XsollaWebBrowser",
                "XsollaUtils",
                "XsollaSettings",
                "OnlineSubsystem"
            }
            );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "UMG",
                "ImageWrapper",
                "WebSockets",
                "XsollaLogin"
            }
            );

		if (Target.Platform == UnrealTargetPlatform.Android)
		{
            PublicDependencyModuleNames.AddRange(new string[] { "Launch" });
			string PluginPath = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);
			AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(PluginPath, "XsollaStore_UPL.xml"));
		}

        if (Target.Platform == UnrealTargetPlatform.IOS)
        {
            PublicDependencyModuleNames.AddRange(new string[] { "Launch" });
            string PluginPath = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);

            AdditionalPropertiesForReceipt.Add("IOSPlugin", Path.Combine(PluginPath, "XsollaStore_IOS_UPL.xml"));

            PublicAdditionalFrameworks.Add(
            new Framework(
                "XsollaSDKPaymentsKit",
                "../IOS/XsollaSDKPaymentsKit.embeddedframework.zip"
            )
            );
            PublicAdditionalFrameworks.Add(
            new Framework(
                "XsollaSDKPaymentsKitObjectiveC",
                "../IOS/XsollaSDKPaymentsKitObjectiveC.embeddedframework.zip"
            )
            );
            PublicAdditionalFrameworks.Add(
            new Framework(
                "XsollaSDKUtilities",
                "../IOS/XsollaSDKUtilities.embeddedframework.zip"

            )
            );
        }

        PublicDefinitions.Add("WITH_XSOLLA_STORE=1");
    }
}
