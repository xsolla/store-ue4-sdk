// Copyright 2019 Xsolla Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Http.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Subsystems/SubsystemCollection.h"

#include "XsollaPayStationSubsystem.generated.h"

class UXsollaPayStationSettings;

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnFetchPaymentTokenSuccess, const FString&, PaymentToken);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnPayStationError, int32, StatusCode, int32, ErrorCode, const FString&, ErrorMessage);

UCLASS()
class XSOLLAPAYSTATION_API UXsollaPayStationSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UXsollaPayStationSubsystem();

	// Begin USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End USubsystem

	/** Initiate purchase session and fetch token for payment console */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|PayStation", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void FetchPaymentToken(const FOnFetchPaymentTokenSuccess& SuccessCallback, const FOnPayStationError& ErrorCallback);

	/** Open payment console for provided token */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|PayStation", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void LaunchPaymentConsole(const FString& PaymentToken, UUserWidget*& BrowserWidget);

	/** Get pending PayStation URL to be opened in browser */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|PayStation")
	FString GetPendingPayStationUrl() const;

	/** Getter for internal settings object to support runtime configuration changes */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|PayStation")
	UXsollaPayStationSettings* GetSettings() const;

protected:
	void FetchPaymentToken_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnFetchPaymentTokenSuccess SuccessCallback, FOnPayStationError ErrorCallback);

protected:
	/** Check whether sandbox is enabled */
	bool IsSandboxEnabled() const;

private:
	/** Create HTTP request and add Xsolla API meta */
	TSharedRef<IHttpRequest> CreateHttpRequest(const FString& Url);

protected:
	/** Pending paystation URL to be opened in browser */
	FString PengindPayStationUrl;

	static const FString PaymentEndpoint;
	static const FString SandboxPaymentEndpoint;

private:
	UPROPERTY()
	TSubclassOf<UUserWidget> DefaultBrowserWidgetClass;

	/** Module settings */
	UPROPERTY()
	UXsollaPayStationSettings* Settings;
};