// Copyright 2020 Xsolla Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
    public class XsollaTests : ModuleRules
    {
        public XsollaTests(ReadOnlyTargetRules Target) : base(Target)
        {
            PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

            PublicDependencyModuleNames.AddRange(
                new[]
                {
                    "Core",
                    "CoreUObject",
                    "Slate",
                    "SlateCore",
                    "UMG",
                    "Engine",
                    "ImageWrapper",
                    "HTTP",
                    "XsollaUtils",
                    "FunctionalTesting",
                    "AutomationDriver",
                    "InputCore",
                    "XsollaStore",
                    "XsollaLogin"
                }
            );
        }
    }
}