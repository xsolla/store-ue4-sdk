// Copyright 2020 Xsolla Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class XsollaUiBuilder : ModuleRules
	{
		public XsollaUiBuilder(ReadOnlyTargetRules Target) : base(Target)
		{
			PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

			PublicDependencyModuleNames.AddRange(
				new []
				{
					"Core",
					"UMG"
				}
			);

			PrivateDependencyModuleNames.AddRange(
				new []
				{
					"CoreUObject",
					"SlateCore",
					"Slate",
					"Engine"
				}
			);
		}
	}
}