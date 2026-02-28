// Fill out your copyright notice in the Description page of Project Settings.

#include "AssetActions/QuickAssetActions.h"
#include "DebugHeader.h"
#include "EditorUtilityLibrary.h" // functions realted to selected assets
#include "EditorAssetLibrary.h" // functions related to all assets in content browser
#include "ObjectTools.h" // for removing unused objects (require unrealEd in modules)
#include "AssetRegistry/AssetRegistryModule.h" // for calling fiix up redirector function
#include "AssetToolsModule.h"

void UQuickAssetActions::DuplicateAssests(int32 NumOfDuplicates)
{
	if (NumOfDuplicates <= 0)
	{
		DebugHeader::showMsgDialog(EAppMsgType::Ok, FString("Please Enter a valid number"));
		return;
	}
	TArray<FAssetData> selectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
	uint32 counter = 0;

	for (const FAssetData& selectedAssetData : selectedAssetsData)
	{
		for (int i = 0; i < NumOfDuplicates; i++)	
		{
			FString sourcePath = selectedAssetData.ObjectPath.ToString();
			FString destinationPath = selectedAssetData.PackageName.ToString() + "_" + FString::FromInt(i + 1);

			if (UEditorAssetLibrary::DuplicateAsset(sourcePath, destinationPath))
			{
				UEditorAssetLibrary::SaveAsset(destinationPath, false);
				counter++;
			}
		}
	}
	if (counter > 0)
	{
		DebugHeader::showNotifyInfo("Successfully duplicated " + FString::FromInt(counter) + " Object");
	}
}

void UQuickAssetActions::AddPrefixes()
{
	TArray<UObject*> selectedObjects = UEditorUtilityLibrary::GetSelectedAssets();
	uint32 counter = 0;

	for (UObject* selectedObject : selectedObjects)
	{
		if (!selectedObject) continue;

		FString* prefixFound = PrefixMap.Find(selectedObject->GetClass());
		if (!prefixFound || prefixFound->IsEmpty())
		{
			DebugHeader::PrintLog("No suitable class found for class: " + selectedObject->GetClass()->GetName());
			continue;
		}

		FString oldName = selectedObject->GetName();
		if (oldName.StartsWith(*prefixFound))
		{
			DebugHeader::PrintLog("Prefix already added for " + selectedObject->GetName());
			continue;
		}

		FString newPrefixName = *prefixFound + oldName;
		UEditorUtilityLibrary::RenameAsset(selectedObject, newPrefixName);

		counter++;
	}
	if (counter > 0)
	{
		DebugHeader::showNotifyInfo(FString::FromInt(counter) + " assets successfully renamed with prefixes");
	}
}


void UQuickAssetActions::RemoveUnusedAssets()
{
	TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
	TArray<FAssetData> UnusedAssetsData;

	FixUpRedirectors();

	for (const FAssetData& SelectedAssetData : SelectedAssetsData)
	{
		TArray<FString> AssetRefrencers =
			UEditorAssetLibrary::FindPackageReferencersForAsset(SelectedAssetData.ObjectPath.ToString());

		if (AssetRefrencers.Num() == 0)
		{
			UnusedAssetsData.Add(SelectedAssetData);
		}
	}

	if (UnusedAssetsData.Num() == 0)
	{
		DebugHeader::showMsgDialog(EAppMsgType::Ok, TEXT("No unused asset found among selected assets"), false);
		return;
	}

	const int32 NumOfAssetsDeleted = ObjectTools::DeleteAssets(UnusedAssetsData);

	if (NumOfAssetsDeleted == 0) return;

	DebugHeader::showNotifyInfo(TEXT("Successfully deleted " + FString::FromInt(NumOfAssetsDeleted) + TEXT(" unused assets")));
}




void UQuickAssetActions::FixUpRedirectors()
{
	TArray<UObjectRedirector*> RedirectorsToFixArray;

	FAssetRegistryModule& AssetRegistryModule =
		FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")); // get unreal asset registry

	// filter all under Game folder and UObjectRedirector class
	FARFilter Filter;
	Filter.bRecursivePaths = true;
	Filter.PackagePaths.Emplace("/Game");
	//Filter.ClassNames.Emplace("ObjectRedirector");
	Filter.ClassPaths.Add(UObjectRedirector::StaticClass()->GetClassPathName());

	TArray<FAssetData> OutRedirectors;
	AssetRegistryModule.Get().GetAssets(Filter, OutRedirectors); // query registry for all redirectors

	for (const FAssetData& RedirectorData : OutRedirectors)
	{
		if (UObjectRedirector* RedirectorToFix = Cast<UObjectRedirector>(RedirectorData.GetAsset()))
		{
			RedirectorsToFixArray.Add(RedirectorToFix);
		}
	}

	FAssetToolsModule& AssetToolsModule =
		FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));  // load AssetTools responsible for calling redirectos in engine

	AssetToolsModule.Get().FixupReferencers(RedirectorsToFixArray); // fix up redirectors
}
// use FModuleManager to load different modules
// use AssetRegistryModule to filter all redirectors
// use assetToolsModule to fix up redirectors



