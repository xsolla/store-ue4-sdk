// Copyright 2020 Xsolla Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class UXsollaInventorySettings;

/**
 * Xsolla Inventory SDK Module
 */
class FXsollaInventoryModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/**
	 * Singleton-like access to this module's interface.  This is just for convenience!
	 * Beware of calling this during the shutdown phase, though.  Your module might have been unloaded already.
	 *
	 * @return Returns singleton instance, loading the module on demand if needed
	 */
	static inline FXsollaInventoryModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FXsollaInventoryModule>("XsollaInventory");
	}

	/**
	 * Checks to see if this module is loaded and ready.  It is only valid to call Get() if IsAvailable() returns true.
	 *
	 * @return True if the module is loaded and ready to use
	 */
	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("XsollaInventory");
	}

	/** Getter for internal settings object to support runtime configuration changes */
	UXsollaInventorySettings* GetSettings() const;

private:
	/** Module settings */
	UXsollaInventorySettings* XsollaInventorySettings;
};
