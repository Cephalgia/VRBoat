
using UnrealBuildTool;
using System.Collections.Generic;

public class VRBoatEditorTarget : TargetRules
{
	public VRBoatEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

		ExtraModuleNames.AddRange( new string[] { "VRBoat", "VRBoatEditor" } );
	}
}
