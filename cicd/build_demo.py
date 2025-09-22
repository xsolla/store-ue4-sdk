import os
import sys
import constants
import utils
import unreal

utils.validate_args_count(6)

working_dir = sys.argv[1]
plugin_branch = sys.argv[2]
engine_version = sys.argv[3]
build_platform = sys.argv[4]
is_platform_browser = sys.argv[5]

project_dir = os.path.join(working_dir, "_WORK_")
source_dir = os.path.join(project_dir, 'Source')
plugin_dir = os.path.join(project_dir, 'Plugins', 'XsollaSdk')
build_dir = os.path.join(working_dir, 'Builds')
user_engine_ini_path = os.path.join(project_dir, 'Config')

# Clone test project and SDK plugin 
utils.clone_repo(constants.demo_project_remote, project_dir, constants.demo_project_branch)
utils.clone_repo(constants.plugin_remote, plugin_dir, plugin_branch)

# Update engine version in test project and SDK plugin
unreal.change_engine_version_for_demo_project(project_dir, engine_version)
unreal.update_target_files_for_demo_project(source_dir, engine_version)
unreal.update_default_engine_ini_file(user_engine_ini_path, engine_version)
unreal.change_engine_version_for_sdk_plugin(plugin_dir, engine_version)

# Apply settings to the test project
unreal.change_browser_settings(project_dir, is_platform_browser)

# Build test project
unreal.build_demo(project_dir, build_dir, engine_version, build_platform)