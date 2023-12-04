#include "VRBoatEditor.h"
#include "PropertyEditorModule.h"
#include "Editor.h"
#include "UnrealEdGlobals.h"
#include "Editor/UnrealEdEngine.h"

#include "MeshPlacementSplineComponentVisualizer.h"
#include "VRBoat/MeshPlacementSplineComponent.h"

IMPLEMENT_MODULE(FVRBoatEditorModule, VRBoatEditor)

void FVRBoatEditorModule::StartupModule()
{
	if (GUnrealEd != NULL)
	{
		TSharedPtr<FMeshPlacementSplineComponentVisualizer> Visualizer = MakeShareable(new FMeshPlacementSplineComponentVisualizer);
		if (Visualizer.IsValid())
		{
			GUnrealEd->RegisterComponentVisualizer(UMeshPlacementSplineComponent::StaticClass()->GetFName(), Visualizer);
			Visualizer->OnRegister();
		}

		//FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		//PropertyModule.RegisterCustomPropertyTypeLayout("SlimeGridButton", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FSlimeGridButtonCustomization::MakeInstance));
	}
}

void FVRBoatEditorModule::ShutdownModule()
{
	if (GUnrealEd != NULL)
	{
		GUnrealEd->UnregisterComponentVisualizer(UMeshPlacementSplineComponent::StaticClass()->GetFName());
	}
}