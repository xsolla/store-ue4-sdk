// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

#pragma once

#include "Components/Widget.h"

#include "XsollaWebBrowser.generated.h"

UCLASS()
class XSOLLAWEBBROWSER_API UXsollaWebBrowser : public UWidget
{
	GENERATED_UCLASS_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUrlChanged, const FText&, Text);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBeforePopup, FString, URL, FString, Frame);

	/**
	 * Loads the specified URL.
	 *
	 * @param NewURL New URL to load.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Web Browser")
	void LoadURL(FString NewURL);

	/**
	 * Loads the specified HTML page content.
	 *
	 * @param Contents HTMPL page content.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Web Browser")
	void LoadHtml(FString Contents, FString DummyURL);

	/**
	 * Executes a JavaScript string in the context of the web page.
	 *
	 * @param ScriptText JavaScript string to execute.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Web Browser")
	void ExecuteJavascript(FString ScriptText);

	/**
	 * Gets the currently loaded URL.
	 *
	 * @return The URL, or empty string if no document is loaded.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Web Browser")
	FString GetUrl() const;

	/**
	 * Clear browser cache (cookies etc.).
	 *
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Web Browser")
	void ClearCache() const;

	/** Called when the URL changes. */
	UPROPERTY(BlueprintAssignable, Category = "Xsolla|Web Browser")
	FOnUrlChanged OnUrlChanged;

	/** Called when a pop-up is about to spawn. */
	UPROPERTY(BlueprintAssignable, Category = "Xsolla|Web Browser")
	FOnBeforePopup OnBeforePopup;

public:
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

protected:
	/** URL that the browser will initially navigate to. The URL should include the protocol, eg http:// */
	UPROPERTY(EditAnywhere, Category = Appearance)
	FString InitialURL;

	/** Should the browser window support transparency. */
	UPROPERTY(EditAnywhere, Category = Appearance)
	bool bSupportsTransparency;

protected:
	TSharedPtr<class SWebBrowser> WebBrowserWidget;

protected:
	// UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	// End of UWidget interface

	void HandleOnUrlChanged(const FText& Text);
	bool HandleOnBeforePopup(FString URL, FString Frame);
};
