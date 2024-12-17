import os
import subprocess
import constants
import utils


def change_engine_version_for_demo_project(project_dir, engine_version):
    project_file_path = os.path.join(project_dir, f"{constants.demo_project_name}.uproject")
    utils.update_json_file(project_file_path, "EngineAssociation", engine_version)


def update_target_files_for_demo_project(source_dir, engine_version):
    target_file_path = os.path.join(source_dir, f"{constants.demo_project_name}.Target.cs")
    editor_target_file_path = os.path.join(source_dir, f"{constants.demo_project_name}Editor.Target.cs")
    utils.update_line_in_file(target_file_path, 11, constants.settings_by_engine_version[engine_version])
    utils.update_line_in_file(editor_target_file_path, 11, constants.settings_by_engine_version[engine_version])


def update_login_upl_for_sdk_plugin(plugin_dir, engine_version):
    target_file_path = os.path.join(plugin_dir, "Source", "XsollaLogin", f"XsollaLogin_UPL.xml")
    utils.update_line_in_file(target_file_path, 81, constants.google_play_services_dependency_by_engine_version[engine_version])


def update_default_engine_ini_file(config_dir, engine_version):
    target_file_path = os.path.join(config_dir, f"DefaultEngine.ini")
    utils.update_line_in_file(target_file_path, 157, constants.java_path_by_engine_version[engine_version])
    utils.update_line_in_file(target_file_path, 158, constants.sdk_api_level_by_engine_version[engine_version])
    utils.update_line_in_file(target_file_path, 159, constants.ndk_api_level_by_engine_version[engine_version])


def change_engine_version_for_sdk_plugin(plugin_dir, engine_version):
    plugin_file_path = os.path.join(plugin_dir, f"{constants.plugin_name}.uplugin")
    utils.update_json_file(plugin_file_path, "EngineVersion", engine_version)


def change_browser_settings(project_dir, is_platform_browser):
    config_path = os.path.join(project_dir, 'Config', 'DefaultEngine.ini')
    utils.update_ini_file(config_path, '/Script/XsollaSettings.XsollaProjectSettings', 'UsePlatformBrowser', is_platform_browser)
    

def build_demo(test_project_dir, archive_dir, engine_version, platform):
    automation_tool_path = _get_automation_tool_path(engine_version)
    uproject_file_path = os.path.join(test_project_dir, f"{constants.demo_project_name}.uproject")
    build_command = [
            automation_tool_path, 
            "BuildCookRun", 
            f"-platform={platform}",
            f"-project={uproject_file_path}",
            f"-archivedirectory={archive_dir}", 
            "-cookflavor=Multi",
            "-nop4", "-cook", "-build", "-stage", "-prereqs", "-package", "-archive"
        ]
    
    print(f"Build command: {' '.join(build_command)}")

    result = subprocess.run(build_command)
    if result.returncode != 0:
            utils.finish_with_error(f'Failed to build project. Error code: {result.returncode}')


def _get_automation_tool_path(engine_version):
    if utils.is_macos():
        return f"/Volumes/External_SSD_256/EpicGames/UE_{engine_version}/Engine/Build/BatchFiles/RunUAT.sh"
    else:
        return os.path.join(f"C:\\EpicGames\\UE_{engine_version}\\Engine\\Binaries\\DotNET\\AutomationTool\\AutomationTool.exe")