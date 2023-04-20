// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
using UnrealBuildTool;
using System;
using System.Collections;
using System.Diagnostics;
using System.IO;
using System.Text.RegularExpressions;

public class Sensors : ModuleRules
{
public Sensors(ReadOnlyTargetRules Target) : base(Target)
	{

		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		PrivatePCHHeaderFile = "SensorsPCH.h";

		PublicDependencyModuleNames.AddRange(new string[] {
					"Core",
					"CoreUObject",
					"Engine",
					"InputCore",
					"RenderCore",
					"RHI"
				});

		PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"CoreUObject",
					"Engine",
					"Slate",
					"SlateCore",
				});

		PrivateIncludePaths.AddRange(new string[] {
					"Sensors"
				});
	}
}
