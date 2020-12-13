// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class Avoid : ModuleRules
{
	public Avoid(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		bEnableExceptions = true;



		PublicIncludePaths.AddRange(
			new string[] {
				"Airsim",
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"Airsim",
				"CoreUObject", 
				"Engine", 
				"InputCore",
				"Foliage",
				"NavigationSystem",
				"Json"
				//"Foliage",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
			//	"CoreUObject",
			//	"Engine",
				"Slate",
				"SlateCore",
				
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

		string AvoidLibLocation = Path.GetFullPath(Path.Combine(ModuleDirectory, "../Dependencies"));

		PublicAdditionalLibraries.Add(Path.Combine(AvoidLibLocation, "libs/rpc.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(AvoidLibLocation, "libs/avoidlib_server.lib"));

		PublicIncludePaths.Add(Path.Combine(AvoidLibLocation, "include"));
		PrivateIncludePaths.Add(Path.Combine(AvoidLibLocation, "include"));

	}
}
