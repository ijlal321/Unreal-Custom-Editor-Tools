# Custom Asset Management Plugin!
This plugin is made to have some experience with the unreal engine tooling exo-system. It can be used for learning as well as including in your project for usage. Here are top features implemented.

- [Quick Asset Actions](#quick-asset-actions)
  - [Asset Batch Duplication](##asset-batch-duplication)
  - [Delete Unused Assets From Selection](##delete-unused-assets-from-selection)
    - [Fixed redirectors](#fixed-redirectors)
  - [Add prefix to assets](#add-prefix-to-assets)
  - [Usage and features](#usage-and-features)
    - [How to use](#how-to-use)
    - [Custom Dialogue Messages and Notifcations](#custom-dialogue-messages-and-notifcations)
- [Extend Content Browser Menu](#extend-content-browser-menu)
  - [Search And Delete Unused Assets](#search-and-delete-unused-assets)
    - [Auto Fixed Up Redirectors](#auto-fixed-up-redirectors)
  - [Search And Delete Empty Folders](#search-and-delete-empty-folders)
- [A Custom Editor Tab for Advanced Deletion](#a-custom-editor-tab-for-advanced-deletion)
  - [Features](#features)
  - [Demo](#demo)
  - [Usage of Slate Widgets](#usage-of-slate-widgets)
- [Auto add prefix names to Quixel Imports (Upcoming !)](#auto-add-prefix-names-to-quixel-imports-upcoming-)


# Quick Asset Actions
Quick asset actions pop up when you right click on a asset. Look at example below.
## Asset Batch Duplication
This functionality will duplicate assets. A input dialougue messgae would input the number of assets user wants to duplicate. The duplicated assets has postfix _n, where n is number of duplicated asset.
<img width="593" height="534" alt="1" src="https://github.com/user-attachments/assets/dba546d0-d322-48dc-b595-a24061484563" />
<img width="820" height="421" alt="www udemy com_course_unrealengine5-c-one-course-solution-for-extending-editor_learn_lecture_33815144" src="https://github.com/user-attachments/assets/492fe24c-b2ed-456e-b66e-d2f3b9151f3e" />
<img width="1234" height="341" alt="www udemy com_course_unrealengine5-c-one-course-solution-for-extending-editor_learn_lecture_33815144 (1)" src="https://github.com/user-attachments/assets/0295acf7-1275-44a4-a896-e35512a5bcdb" />

## Delete Unused Assets From Selection
This functionality will remove unused assets from selection. 

<img width="748" height="536" alt="www udemy com_course_unrealengine5-c-one-course-solution-for-extending-editor_learn_lecture_33815144" src="https://github.com/user-attachments/assets/ce411f24-235b-45d0-a21d-26cfd40f06e8" />

### Fixed redirectors
To avoid any errors, redirectors are automatically fixed before checking for unused assets.

## Add prefix to assets
This will automatically prefixes to assets based on official documentation from Unreal engine. In the example below, the prefix BP_ is added to bluprint.
The official unreal documentation for naming is referenced from here: https://dev.epicgames.com/documentation/en-us/unreal-engine/recommended-asset-naming-conventions-in-unreal-engine-projects?application_version=4.27

<img width="544" height="571" alt="www udemy com_course_unrealengine5-c-one-course-solution-for-extending-editor_learn_lecture_33815144 (1)" src="https://github.com/user-attachments/assets/f374d8ec-5049-46f3-961b-28ae07aed303" />
<img width="999" height="345" alt="www udemy com_course_unrealengine5-c-one-course-solution-for-extending-editor_learn_lecture_33815144 (2)" src="https://github.com/user-attachments/assets/c6ec98c3-4676-43b6-af34-57fe1c62a185" />


## Usage and features
### How to use
To use these features, just right click on any asset or a number of assets and select show scripted actions.
### Custom Dialogue Messages and Notifcations
Custom dialogue messages and notifications are added. Custom Messages can easily be edited via code.


# Extend Content Browser Menu
These funuctionalities will extend the custom browser, and more specfically the source panel of content browser. By clicking on any folder inside source panel (panel to left of content browser), you can see these functionality.

## Search And Delete Unused Assets
This will delete all unused assets from a folder as well as subfolders.

This is done in 3 steps. 
1. Finding all assets in folder.
2. fixing up redirectos.
3. Notify user about number of assets found (to safegrard from unintentional clicks)
4. Show unused assets and delete panel

<img width="546" height="501" alt="www udemy com_course_unrealengine5-c-one-course-solution-for-extending-editor_learn_lecture_33815144 (3)" src="https://github.com/user-attachments/assets/23d734f7-f43b-450b-ad67-6dd0684d0f26" />
<img width="873" height="373" alt="www udemy com_course_unrealengine5-c-one-course-solution-for-extending-editor_learn_lecture_33815144 (4)" src="https://github.com/user-attachments/assets/9cb88ce9-c812-4f97-a186-632f3d5f7206" />
<img width="510" height="573" alt="www udemy com_course_unrealengine5-c-one-course-solution-for-extending-editor_learn_lecture_33815144 (5)" src="https://github.com/user-attachments/assets/42ef2e8b-fe6a-47d2-85ca-212bec06f812" />

### Auto Fixed Up Redirectors
All redirectors are fixed before checking for unused assets.

## Search And Delete Empty Folders
This functionality will remove any empty folders from folder and subfolders.
<img width="261" height="161" alt="www udemy com_course_unrealengine5-c-one-course-solution-for-extending-editor_learn_lecture_33815144 (6)" src="https://github.com/user-attachments/assets/c7f3bd21-800e-4ec2-a492-8691dd76c2f2" />

# A Custom Editor Tab for Advanced Deletion
To use this feature, right click on a folder in context browser, and click advance deletion tab.

## Features
1. Multiple Listing options. User can list all assets, ununsed assets.
2. checbox for selecting assets to delete.
3. Listing each asset type, name.
4. Quick options for select/deselect all assets.
5. Double click on a asset to see it in content browser.


## Demo
<img width="909" height="735" alt="www udemy com_course_unrealengine5-c-one-course-solution-for-extending-editor_learn_lecture_33959408" src="https://github.com/user-attachments/assets/3263869e-4c93-4589-a169-027de6e16e75" />

## Usage of Slate Widgets
Slate Widgets are used to make and design the custom tab.


# Auto add prefix names to Quixel Imports (Upcoming !)
This will auto add prefix to models improted via Quixel. This module is incomplete and still a work in progress. Documentation will be updated when module is compelted. Feel free to make any contributions !!
