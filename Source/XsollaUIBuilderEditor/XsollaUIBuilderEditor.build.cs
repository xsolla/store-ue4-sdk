// Copyright 2021 Xsolla Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

namespace UnrealBuildTool.Rules
{
    public class XsollaUIBuilderEditor : ModuleRules
    {
        public XsollaUIBuilderEditor(ReadOnlyTargetRules Target) : base(Target)
        {
            PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

            PublicDependencyModuleNames.AddRange(
                new string[]
                {
                    "Core",
                    "CoreUObject",
                    "Engine",
                    "PropertyEditor",
                    "Slate",
                    "SlateCore",
                    "UMG",
                    "XsollaUIBuilder",
                    "EditorStyle",
                    "XsollaUtils"
                }
            );
        }
    }
}
