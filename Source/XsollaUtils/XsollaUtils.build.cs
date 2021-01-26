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
                    "UMG",
                    "ImageWrapper",
                    "HTTP",
                    "Json",
                    "JsonUtilities"
                }
            );

            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "CoreUObject",
                    "Engine",
                    "Slate",
                    "Blutility",
                    "UMGEditor",
                    "UnrealEd",
                    "Projects",
                    "SlateCore"
                }
                );

            PublicDefinitions.Add("WITH_XSOLLA_UTILS=1");
        }
    }
}