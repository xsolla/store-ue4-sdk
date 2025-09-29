demo_project_remote = 'git@gitlab.loc:sdk_group/store-ue-sdk-test.git'
demo_project_branch = 'master'
demo_project_name = 'StoreUeSdkTest'

plugin_remote = 'git@gitlab.loc:sdk_group/store-ue4-sdk.git'
plugin_name = 'Xsolla'
settings_by_engine_version = {'5.2': 'DefaultBuildSettings = BuildSettingsVersion.V2; IncludeOrderVersion = EngineIncludeOrderVersion.Latest;',
                              '5.3': 'DefaultBuildSettings = BuildSettingsVersion.V4; IncludeOrderVersion = EngineIncludeOrderVersion.Latest;',
                              '5.4': 'DefaultBuildSettings = BuildSettingsVersion.V5; IncludeOrderVersion = EngineIncludeOrderVersion.Latest;',
                              '5.5': 'DefaultBuildSettings = BuildSettingsVersion.V5; IncludeOrderVersion = EngineIncludeOrderVersion.Latest;'}
java_path_by_engine_version = {'5.2': 'JavaPath=(Path="C:/Program Files/Java/jdk1.8.0_181")',
                               '5.3': 'JavaPath=(Path="C:/Program Files/Java/jdk-17")',
                               '5.4': 'JavaPath=(Path="C:/Program Files/Java/jdk-17")',
                               '5.5': 'JavaPath=(Path="C:/Program Files/Java/jdk-17")'}
sdk_api_level_by_engine_version = {'5.2': 'SDKAPILevel=android-31',
                                   '5.3': 'SDKAPILevel=android-31',
                                   '5.4': 'SDKAPILevel=android-34',
                                   '5.5': 'SDKAPILevel=android-34'}
ndk_api_level_by_engine_version = {'5.2': 'NDKAPILevel=android-21',
                                   '5.3': 'NDKAPILevel=android-28',
                                   '5.4': 'NDKAPILevel=android-28',
                                   '5.5': 'NDKAPILevel=android-28'}