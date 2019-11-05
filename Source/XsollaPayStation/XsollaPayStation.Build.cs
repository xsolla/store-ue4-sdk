// Copyright 2019 Xsolla Inc. All Rights Reserved.

using UnrealBuildTool;

public class XsollaPayStation : ModuleRules
{
    public XsollaPayStation(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "HTTP",
                "Json",
                "UMG",
                "XsollaWebBrowser"
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

        PublicDefinitions.Add("WITH_XSOLLA_PAYSTATION=1");
    }
}