// Copyright 2021 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class UXsollaUiBuilderSettings;

/**
 * Xsolla Login SDK Module
 * See: https://developers.xsolla.com/api/v2/login/
 */
class FXsollaUiBuilderModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/**
	 * Singleton-like access to this module's interface.  This is just for convenience!
	 * Beware of calling this during the shutdown phase.  Your module might be unloaded already.
	 *
	 * @return Returns a singleton instance, loading the module on demand if needed.
	 */
	static inline FXsollaUiBuilderModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FXsollaUiBuilderModule>("XsollaUiBuilder");
	}

	/**
	 * Checks to see if this module is loaded and ready.  It is only valid to call Get() if IsAvailable() returns true.
	 *
	 * @return True if the module is loaded and ready to use.
	 */
	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("XsollaUiBuilder");
	}

	/** Getter for internal settings object to support runtime configuration changes. */
	UXsollaUiBuilderSettings* GetSettings() const;

private:
	/** Module settings */
	UXsollaUiBuilderSettings* XsollaUiBuilderSettings;
};
