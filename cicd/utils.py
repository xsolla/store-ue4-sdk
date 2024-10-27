import configparser
import json
import os
import shutil
import sys
import git
import stat

def finish_with_error(message):
    print(message)
    sys.exit(1)


def validate_args_count(expected_args_count):
    if len(sys.argv) < expected_args_count:
        finish_with_error("Arguments count error. Expected: " + str(expected_args_count) + ", actual: " + str(len(sys.argv)))


def is_macos():
    return sys.platform == 'darwin' 

def clone_repo(url, path, branch):
    try:
        if os.path.exists(path):
            shutil.rmtree(path, onerror=_remove_readonly)
    except Exception as e:
        finish_with_error(f'Failed to remove directory: {e}')

    try:
        git.Repo.clone_from(url, path, branch=branch, depth=1)
    except Exception as e:
        finish_with_error(f'Failed to clone repository: {e}')


def _remove_readonly(func, path, exc_info):
    os.chmod(path, stat.S_IWRITE)
    func(path)


def update_ini_file(file_path, section, option, new_value):
    config = configparser.RawConfigParser(allow_no_value=True, strict=False)
    with open(file_path, 'r') as file:
        config.read_file(file)
    
    if section not in config:
        finish_with_error(f"Section '{section}' not found in {file_path}")
        return
    
    if option not in config[section]:
        finish_with_error(f"Option '{option}' not found in section '{section}'")
        return
    
    config[section][option] = new_value
    
    try:
        with open(file_path, 'w') as configfile:
            config.write(configfile)
    except Exception as e:
        finish_with_error(f'Failed to write to file: {e}')


def update_json_file(file_path, key, value):
    with open(file_path, 'r') as file:
        data = json.load(file)

    data[key] = value

    with open(file_path, 'w') as file:
        json.dump(data, file, indent=2)