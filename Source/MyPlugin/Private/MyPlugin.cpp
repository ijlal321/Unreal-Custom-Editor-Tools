// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyPlugin.h"
#include "ContentBrowserModule.h"
#include "DebugHeader.h"
#include "EditorAssetLibrary.h"
#include "ObjectTools.h"
#include "AssetRegistry/AssetRegistryModule.h" // for calling fiix up redirector function
#include "AssetToolsModule.h"
#include "SlateWidgets/AdvanceDeletionWidget.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"


#define LOCTEXT_NAMESPACE "FMyPluginModule"

void FMyPluginModule::StartupModule()
{
	InitCBMenuExtention();
	RegisterAdvanceDeletionTab();

}


// load content browser module via FModuleManager 
// Get all  path view context menu extenders using ContentBrowserModule.GetAllPathViewContextMenuExtenders
// add our custom delegate in menu extender array
// binding delegate (Step 1 -> giving position)
// create a menu extender -> TSharedRef<FExtender> MenuExtender
// give posiiton
// binding delegate (Step 2 -> define details for this menu entry)
// binding delegate (Step 3 -> The actual function to excute)
// ====
// Bind a delegate using create raw
// When binded, to callback function must match the expected input parameters type
// 

// load context browser module -> load context browser menu extries -> add our own menu entry
// in CB -> create a menuExtender (package used to create menu entries), set position, and deleate, musst return the newly created menu extender
// in CB (takes menu builder)-> set details of menu entry, and set function to call
//

// Glossary 
// Content Browser (left pannel) -> Sources pannel (more common) / Path View (none ?)
// Right click on on a folder in Path View	-> Path View Context Menu / Or just Context Menu (Better)
// Adding something to path view context menu -> Path view context menu extender
// 
// Menu Entry -> One clickable item inside a menu.
// Menu Extender -> tool that allow to add custom entries to existing editor menus
// Content Browser Module -> module that manages the Content Browser
// FExecuteAction::CreateRaw -> function that runs when the user clicks your menu item.
// 
// FExtender -> menu-building package
// 
// PathView = folder tree inside source panel ( folder hirerarchy )
// AssetView = the right - side asset list
// CollectionView = the collections panel
//


#pragma region ContentBrowserMenuExtention

void FMyPluginModule::InitCBMenuExtention()
{
	FContentBrowserModule& ContentBrowserModule =
		FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));

	//Get hold of all the menu extenders
	TArray<FContentBrowserMenuExtender_SelectedPaths>& ContentBrowserModuleMenuExtenders =
		ContentBrowserModule.GetAllPathViewContextMenuExtenders();

	/*FContentBrowserMenuExtender_SelectedPaths CustomCBMenuDelegate;
	CustomCBMenuDelegate.BindRaw(this,&FMyPluginModule::CustomCBMenuExtender);

	ContentBrowserModuleMenuExtenders.Add(CustomCBMenuDelegate);*/

	//We add custom delete to all the existing delegate
	ContentBrowserModuleMenuExtenders.Add(FContentBrowserMenuExtender_SelectedPaths::
		CreateRaw(this, &FMyPluginModule::CustomCBMenuExtender));
}

//To define the positio for inserting menu entry
TSharedRef<FExtender> FMyPluginModule::CustomCBMenuExtender(const TArray<FString>& SelectedPaths)
{
	TSharedRef<FExtender> MenuExtender(new FExtender());

	if (SelectedPaths.Num() > 0)
	{
		MenuExtender->AddMenuExtension(FName("Delete"), //Extend hook, position to insert
			EExtensionHook::After, //Insert before or after
			TSharedPtr<FUICommandList>(), //Custom hot keys 
			FMenuExtensionDelegate::CreateRaw(this, &FMyPluginModule::AddCBMenuEntry)); //Second binding, will define details for this menu entry

		FolderPathsSelected = SelectedPaths;
	}

	return MenuExtender;
}

//Define details for the custom menu entry
void FMyPluginModule::AddCBMenuEntry(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT("Delete Unused Assets")), //Title text for menu entry
		FText::FromString(TEXT("Safely delete all unused assets under folder")), //Tooltip text
		FSlateIcon(),	//Custom icon
		FExecuteAction::CreateRaw(this, &FMyPluginModule::OnDeleteUnsuedAssetButtonClicked) //The actual function to excute
	);

	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT("Delete Empty Folders")), //Title text for menu entry
		FText::FromString(TEXT("Safely delete all empty folders")), //Tooltip text
		FSlateIcon(),	//Custom icon
		FExecuteAction::CreateRaw(this, &FMyPluginModule::OnDeleteEmptyFoldersButtonClicked) //The actual function to excute
	);

	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT("Advance Deletion")), //Title text for menu entry
		FText::FromString(TEXT("List assets by specific condition in a tab for deleting")), //Tooltip text
		FSlateIcon(),	//Custom icon
		FExecuteAction::CreateRaw(this, &FMyPluginModule::OnAdvanceDeletionButtonClicked) //The actual function to excute
	);
}

void FMyPluginModule::OnDeleteUnsuedAssetButtonClicked()
{
	if (FolderPathsSelected.Num() > 1)
	{
		DebugHeader::showMsgDialog(EAppMsgType::Ok, TEXT("You can only do this to one folder"));
		return;
	}

	TArray<FString> AssetsPathNames = UEditorAssetLibrary::ListAssets(FolderPathsSelected[0]);

	//Whether there are assets under selected folder
	if (AssetsPathNames.Num() == 0)
	{
		DebugHeader::showMsgDialog(EAppMsgType::Ok, TEXT("No asset found under selected folder"), false);
		return;
	}

	EAppReturnType::Type ConfirmResult =
		DebugHeader::showMsgDialog(EAppMsgType::YesNo, TEXT("A total of ") + FString::FromInt(AssetsPathNames.Num())
			+ TEXT(" assets need to be checked.\nWould you like to procceed?"), false);

	if (ConfirmResult == EAppReturnType::No) return;

	FixUpRedirectors();

	TArray<FAssetData> UnusedAssetsDataArray;

	for (const FString& AssetPathName : AssetsPathNames)
	{
		//Don't touch root folder
		if (AssetPathName.Contains(TEXT("Developers")) ||
			AssetPathName.Contains(TEXT("Collections")) ||
			AssetPathName.Contains(TEXT("__ExternalActors__")) ||
			AssetPathName.Contains(TEXT("__ExternalObjects__")))
		{
			continue;
		}

		if (!UEditorAssetLibrary::DoesAssetExist(AssetPathName)) continue;

		TArray<FString> AssetReferencers =
			UEditorAssetLibrary::FindPackageReferencersForAsset(AssetPathName);

		if (AssetReferencers.Num() == 0)
		{
			const FAssetData UnusedAssetData = UEditorAssetLibrary::FindAssetData(AssetPathName);
			UnusedAssetsDataArray.Add(UnusedAssetData);
		}
	}

	if (UnusedAssetsDataArray.Num() > 0)
	{
		ObjectTools::DeleteAssets(UnusedAssetsDataArray);
	}
	else
	{
		DebugHeader::showMsgDialog(EAppMsgType::Ok, TEXT("No unused asset found under selected folder"), false);
	}
}

void FMyPluginModule::FixUpRedirectors()
{
	TArray<UObjectRedirector*> RedirectorsToFixArray;

	FAssetRegistryModule& AssetRegistryModule =
		FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

	FARFilter Filter;
	Filter.bRecursivePaths = true;
	Filter.PackagePaths.Emplace("/Game");
	Filter.ClassPaths.Add(UObjectRedirector::StaticClass()->GetClassPathName());

	TArray<FAssetData> OutRedirectors;
	AssetRegistryModule.Get().GetAssets(Filter, OutRedirectors);

	for (const FAssetData& RedirectorData : OutRedirectors)
	{
		if (UObjectRedirector* RedirectorToFix = Cast<UObjectRedirector>(RedirectorData.GetAsset()))
		{
			RedirectorsToFixArray.Add(RedirectorToFix);
		}
	}

	FAssetToolsModule& AssetToolsModule =
		FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));

	AssetToolsModule.Get().FixupReferencers(RedirectorsToFixArray);
}

void FMyPluginModule::OnDeleteEmptyFoldersButtonClicked()
{
	FixUpRedirectors();

	TArray<FString> FolderPathsArray = UEditorAssetLibrary::ListAssets(FolderPathsSelected[0], true, true);
	uint32 Counter = 0;

	FString EmptyFolderPathsNames;
	TArray<FString> EmptyFoldersPathsArray;

	for (const FString& FolderPath : FolderPathsArray)
	{
		if (FolderPath.Contains(TEXT("Developers")) ||
			FolderPath.Contains(TEXT("Collections")) ||
			FolderPath.Contains(TEXT("__ExternalActors__")) ||
			FolderPath.Contains(TEXT("__ExternalObjects__")))
		{
			continue;
		}

		if (!UEditorAssetLibrary::DoesDirectoryExist(FolderPath)) continue;

		if (!UEditorAssetLibrary::DoesDirectoryHaveAssets(FolderPath))
		{
			EmptyFolderPathsNames.Append(FolderPath);
			EmptyFolderPathsNames.Append(TEXT("\n"));

			EmptyFoldersPathsArray.Add(FolderPath);
		}
	}

	if (EmptyFoldersPathsArray.Num() == 0)
	{
		DebugHeader::showMsgDialog(EAppMsgType::Ok, TEXT("No empty folder found under selected folder"), false);
		return;
	}

	EAppReturnType::Type ConfirmResult = DebugHeader::showMsgDialog(EAppMsgType::OkCancel,
		TEXT("Empty folders found in:\n") + EmptyFolderPathsNames + TEXT("\nWould you like to delete all?"), false);

	if (ConfirmResult == EAppReturnType::Cancel) return;

	for (const FString& EmptyFolderPath : EmptyFoldersPathsArray)
	{
		if (UEditorAssetLibrary::DeleteDirectory(EmptyFolderPath))
			++Counter;
		else DebugHeader::Print(TEXT("Failed to delete " + EmptyFolderPath), FColor::Red);
	}

	if (Counter > 0)
	{
		DebugHeader::showNotifyInfo(TEXT("Successfully deleted ") + FString::FromInt(Counter) + TEXT("folders"));
	}

}

void FMyPluginModule::OnAdvanceDeletionButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(FName("AdvanceDeletion"));
}

#pragma endregion

// 
// 1. Register tab
// use FGlobalTabmanager::Get()->RegisterNomadTabSpawner 
// on callback, specify which tab to open when invoked, using SNew, also pass any args
// 2. Register a menu item from context menu, which on click, invoke a tab using 
//	-> 	FGlobalTabmanager::Get()->TryInvokeTab(FName("AdvanceDeletion"));
// Make sure naming is same, when registering a tab, and when invoking/creating it in menu entender
//


#pragma region CustomEditorTab

void FMyPluginModule::RegisterAdvanceDeletionTab()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(FName("AdvanceDeletion"),
		FOnSpawnTab::CreateRaw(this, &FMyPluginModule::OnSpawnAdvanceDeltionTab))
		.SetDisplayName(FText::FromString(TEXT("Advance Deletion")));
}

TSharedRef<SDockTab> FMyPluginModule::OnSpawnAdvanceDeltionTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return
		SNew(SDockTab).TabRole(ETabRole::NomadTab)
		[
			SNew(SAdvanceDeletionTab)
				.AssetsDataToStore(GetAllAssetDataUnderSelectedFolder())
		];
}

TArray<TSharedPtr<FAssetData>> FMyPluginModule::GetAllAssetDataUnderSelectedFolder()
{
	TArray< TSharedPtr <FAssetData> > AvaiableAssetsData;
	
	if (FolderPathsSelected.Num() == 0) return AvaiableAssetsData;

	TArray<FString> AssetsPathNames = UEditorAssetLibrary::ListAssets(FolderPathsSelected[0]);

	for (const FString& AssetPathName : AssetsPathNames)
	{
		//Don't touch root folder
		if (AssetPathName.Contains(TEXT("Developers")) ||
			AssetPathName.Contains(TEXT("Collections")) ||
			AssetPathName.Contains(TEXT("__ExternalActors__")) ||
			AssetPathName.Contains(TEXT("__ExternalObjects__")))
		{
			continue;
		}

		if (!UEditorAssetLibrary::DoesAssetExist(AssetPathName)) continue;

		const FAssetData Data = UEditorAssetLibrary::FindAssetData(AssetPathName);

		AvaiableAssetsData.Add(MakeShared<FAssetData>(Data));
	}

	return AvaiableAssetsData;
}

#pragma endregion


#pragma region ProccessDataForAdvanceDeletionTab

bool FMyPluginModule::DeleteSingleAssetForAssetList(const FAssetData& AssetDataToDelete)
{
	TArray<FAssetData> AssetDataForDeletion;
	AssetDataForDeletion.Add(AssetDataToDelete);

	if (ObjectTools::DeleteAssets(AssetDataForDeletion) > 0)
	{
		return true;
	}

	return false;
}

bool FMyPluginModule::DeleteMultipleAssetsForAssetList(const TArray<FAssetData>& AssetsToDelete)
{
	if (ObjectTools::DeleteAssets(AssetsToDelete) > 0)
	{
		return true;
	}

	return false;
}

void FMyPluginModule::ListUnusedAssetsForAssetList(const TArray<TSharedPtr<FAssetData>>& AssetsDataToFilter,
	TArray<TSharedPtr<FAssetData>>& OutUnusedAssetsData)
{
	OutUnusedAssetsData.Empty();

	for (const TSharedPtr<FAssetData>& DataSharedPtr : AssetsDataToFilter)
	{
		TArray<FString> AssetReferencers =
			UEditorAssetLibrary::FindPackageReferencersForAsset(DataSharedPtr->ObjectPath.ToString());

		if (AssetReferencers.Num() == 0)
		{
			OutUnusedAssetsData.Add(DataSharedPtr);
		}
	}
}

void FMyPluginModule::ListSameNameAssetsForAssetList(const TArray<TSharedPtr<FAssetData>>& AssetsDataToFilter,
	TArray<TSharedPtr<FAssetData>>& OutSameNameAssetsData)
{
	OutSameNameAssetsData.Empty();

	//Multimap for supporting finding assets with same name
	TMultiMap<FString, TSharedPtr<FAssetData> > AssetsInfoMultiMap;

	for (const TSharedPtr<FAssetData>& DataSharedPtr : AssetsDataToFilter)
	{
		AssetsInfoMultiMap.Emplace(DataSharedPtr->AssetName.ToString(), DataSharedPtr);
	}

	for (const TSharedPtr<FAssetData>& DataSharedPtr : AssetsDataToFilter)
	{
		TArray< TSharedPtr <FAssetData> > OutAssetsData;
		AssetsInfoMultiMap.MultiFind(DataSharedPtr->AssetName.ToString(), OutAssetsData);

		if (OutAssetsData.Num() <= 1) continue;

		for (const TSharedPtr<FAssetData>& SameNameData : OutAssetsData)
		{
			if (SameNameData.IsValid())
			{
				OutSameNameAssetsData.AddUnique(SameNameData);
			}
		}
	}
}

void FMyPluginModule::SyncCBToClickedAssetForAssetList(const FString& AssetPathToSync)
{
	TArray<FString> AssetsPathToSync;
	AssetsPathToSync.Add(AssetPathToSync);

	UEditorAssetLibrary::SyncBrowserToObjects(AssetsPathToSync);
}
#pragma endregion


void FMyPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMyPluginModule, MyPlugin)