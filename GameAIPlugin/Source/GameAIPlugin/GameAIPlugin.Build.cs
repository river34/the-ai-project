// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class GameAIPlugin : ModuleRules
{
    private string ModulePath
    {
        get { return ModuleDirectory; }
    }

    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../ThirdParties/")); }
    }

    public GameAIPlugin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "GameMathPlugin" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

        // Turn on RTTI
        bUseRTTI = true; 

        // LoadGameMathLib(Target);
        LoadLib(Target, "GameMathLib", "GAME_MATH_LIB");
        LoadLib(Target, "GameFrameworkLib", "GAME_FRAMEWORK_LIB");
        LoadLib(Target, "GameHFSMLib", "GAME_HFSM_LIB");
        LoadLib(Target, "GameBTLib", "GAME_BT_LIB");
        LoadLib(Target, "GameUtilityLib", "GAME_UTILITY_LIB");
        LoadLib(Target, "GameGOAPLib", "GAME_GOAP_LIB");
    }

    public bool LoadLib(ReadOnlyTargetRules Target, string LibName, string LibDef)
    {
        bool isLibrarySupported = false;

        if ((Target.Platform == UnrealTargetPlatform.Win64) || (Target.Platform == UnrealTargetPlatform.Win32))
        {
            isLibrarySupported = true;

            string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "x86";
            string LibrariesPath = Path.Combine(ThirdPartyPath, LibName, "Libraries");

            /*
            test your path with:
            using System; // Console.WriteLine("");
            Console.WriteLine("... LibrariesPath -> " + LibrariesPath);
            */

            PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, LibName + "." + PlatformString + ".lib"));
        }

        if (isLibrarySupported)
        {
            // Include path
            PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, LibName, "Includes"));
        }

        Definitions.Add(string.Format("WITH_" + LibDef + "_BINDING={0}", isLibrarySupported ? 1 : 0));

        return isLibrarySupported;
    }

    public bool LoadGameMathLib(ReadOnlyTargetRules Target)
    {
        bool isLibrarySupported = false;

        if ((Target.Platform == UnrealTargetPlatform.Win64) || (Target.Platform == UnrealTargetPlatform.Win32))
        {
            isLibrarySupported = true;

            string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "x86";
            string LibrariesPath = Path.Combine(ThirdPartyPath, "GameMathLib", "Libraries");

            /*
            test your path with:
            using System; // Console.WriteLine("");
            Console.WriteLine("... LibrariesPath -> " + LibrariesPath);
            */

            PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "GameMathLib." + PlatformString + ".lib"));
        }

        if (isLibrarySupported)
        {
            // Include path
            PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "GameMathLib", "Includes"));
        }

        Definitions.Add(string.Format("WITH_GAME_MATH_LIB_BINDING={0}", isLibrarySupported ? 1 : 0));

        return isLibrarySupported;
    }
}
