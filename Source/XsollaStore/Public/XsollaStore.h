// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class UXsollaStoreSettings;

/**
 * Xsolla Store SDK Module
 */
class FXsollaStoreModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/**
	 * Singleton-like access to this module's interface.  This is just for convenience!
	 * Beware of calling this during the shutdown phase, though.  Your module might be already uploaded.
	 *
	 * @return Returns singleton instance, loading the module on demand if needed
	 */
	static inline FXsollaStoreModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FXsollaStoreModule>("XsollaStore");
	}

	/**
	 * Checks to see if this module is loaded and ready.  It is only valid to call Get() if IsAvailable() returns true.
	 *
	 * @return True if the module is loaded and ready to use.
	 */
	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("XsollaStore");
	}

	/** Getter for internal settings object to support runtime configuration changes. */
	UXsollaStoreSettings* GetSettings() const;

private:
	/** Module settings. */
	UXsollaStoreSettings* XsollaStoreSettings;
};
