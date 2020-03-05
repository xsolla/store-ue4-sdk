// Copyright 2019 Xsolla Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class UXsollaPayStationSettings;

/**
 * Xsolla PayStation SDK Module
 */
class FXsollaPayStationModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/**
	 * Singleton-like access to this module's interface. This is just for convenience!
	 * Beware of calling this during the shutdown phase, though. Your module might have beed unloaded already.
	 * 
	 * @return Returns singleton instance, loading the module on demand if needed
	 */
	static inline FXsollaPayStationModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FXsollaPayStationModule>("XsollaPayStation");
	}

	/**
	 * Checks to see if module is loaded and ready. It is only valid to call Get() if IsAvailable() returns true.
	 * 
	 * @return True if the module is loaded and ready to use
	 */
	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("XsollaPayStation");
	}

	/** Getter for internal settings object to support runtime configuration changes */
	UXsollaPayStationSettings* GetSettings() const;

private:
	/** Module settings */
	UXsollaPayStationSettings* XsollaPayStationSettings;
};