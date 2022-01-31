// Copyright 2022 Xsolla Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
    public class XsollaSettings : ModuleRules
    {
        public XsollaSettings(ReadOnlyTargetRules Target) : base(Target)
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
                    "Projects"
                }
            );

            PublicDefinitions.Add("WITH_XSOLLA_SETTINGS=1");
        }
    }
}
