// Copyright 2021 Xsolla Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class XsollaUIBuilder : ModuleRules
{
    public XsollaUIBuilder(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "UMG"
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
                "XsollaUtils"
            }
            );
    }
}
