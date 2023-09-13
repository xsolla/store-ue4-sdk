#!/bin/bash

PROJECT_DIR=$1
PROJECT_BRANCH=$2
PLUGIN_BRANCH=$3
CI_WORK_DIR=$4
ENGINE_VERSION=$5
PROJECT_NAME=StoreUeSdkTest

echo ""
echo "============================"
echo "### PREPARE TEST PROJECT ###"

PROJECT_REMOTE=git@gitlab.loc:sdk_group/store-ue-sdk-test.git
PLUGIN_REMOTE=git@gitlab.loc:sdk_group/store-ue4-sdk.git
PLUGIN_DIR=$PROJECT_DIR/Plugins/XsollaSdk

echo "PROJECT_REMOTE: ${PROJECT_REMOTE}"
echo "PLUGIN_REMOTE: ${PLUGIN_REMOTE}"
echo "PLUGIN_DIR: ${PLUGIN_DIR}"

if [ -d "$PROJECT_DIR" ]; then rm -Rf $PROJECT_DIR; fi
git clone --depth 1 --branch $PROJECT_BRANCH $PROJECT_REMOTE $PROJECT_DIR
git clone --depth 1 --branch $PLUGIN_BRANCH $PLUGIN_REMOTE $PLUGIN_DIR

echo ""
echo "============================="
echo "### GENERATE TEST PROJECT ###"

UBT_PATH="/Users/Shared/EpicGames/UE_$ENGINE_VERSION/Engine/Build/BatchFiles/Mac/Build.sh"
UPROJECT_PATH=$PROJECT_DIR/$PROJECT_NAME.uproject

echo "UBT_PATH: ${UBT_PATH}"
echo "UPROJECT_PATH: ${UPROJECT_PATH}"

$UBT_PATH Development Mac -TargetType=Editor -Project=$UPROJECT_PATH -Progress -NoEngineChanges -NoHotReloadFromIDE

echo ""
echo "================================================="
echo "### DEFINE VARIABLES FOR PACKAGE TEST PROJECT ###"

AT_PATH="/Users/Shared/EpicGames/UE_$ENGINE_VERSION/Engine/Build/BatchFiles/RunUAT.sh"
PACKAGE_ROOT_DIR=$CI_WORK_DIR/Builds
SETTINGS_INI_PATH=$PROJECT_DIR/Config/DefaultEngine.ini

echo "AT_PATH: ${AT_PATH}"
echo "UPROJECT_PATH: ${UPROJECT_PATH}"
echo "PACKAGE_ROOT_DIR: ${PACKAGE_ROOT_DIR}"
echo "SETTINGS_INI_PATH: ${SETTINGS_INI_PATH}"

echo ""
echo "======================================="
echo "### PACKAGE IN GAME BROWSER VARIANT ###"

sh $CI_WORK_DIR/cicd/change-ini-settings.sh $SETTINGS_INI_PATH "/Script/XsollaSettings.XsollaProjectSettings" "UsePlatformBrowser" "False"
$AT_PATH BuildCookRun -platform=IOS -project=$UPROJECT_PATH -archivedirectory=$PACKAGE_ROOT_DIR/InGameBrowser -nocompileeditor -nop4 -cook -build -stage -prereqss -package -archive

echo ""
echo "========================================"
echo "### PACKAGE PLATFORM BROWSER VARIANT ###"

sh $CI_WORK_DIR/cicd/change-ini-settings.sh $SETTINGS_INI_PATH "/Script/XsollaSettings.XsollaProjectSettings" "UsePlatformBrowser" "True"
$AT_PATH BuildCookRun -platform=IOS -project=$UPROJECT_PATH -archivedirectory=$PACKAGE_ROOT_DIR/PlatfromBrowser -nocompileeditor -nop4 -cook -build -stage -prereqss -package -archive