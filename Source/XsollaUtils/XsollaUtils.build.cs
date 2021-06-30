// Copyright 2021 Xsolla Inc. All Rights Reserved.

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
                    "HTTP",
                    "Json",
                    "JsonUtilities",
                    "Projects"
                }
            );

            PublicDefinitions.Add("WITH_XSOLLA_UTILS=1");
        }
    }
}
