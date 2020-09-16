// Copyright 2020 Xsolla Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
    public class XsollaUtils : ModuleRules
    {
        public XsollaUtils(ReadOnlyTargetRules Target) : base(Target)
        {
            PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

            PublicDependencyModuleNames.AddRange(
                new string[]
                {
                    "Core",
                    "CoreUObject",
                    "Slate",
                    "SlateCore",
                    "UMG",
                    "Engine",
                    "ImageWrapper",
                    "HTTP"
                }
            );
        }
    }
}