// Copyright 2021 Xsolla Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class XsollaInventory : ModuleRules
{
    public XsollaInventory(ReadOnlyTargetRules Target) : base(Target)
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
                "XsollaSettings"
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
                "XsollaLogin"
            }
            );

		PublicDefinitions.Add("WITH_XSOLLA_INVENTORY=1");
    }
}
