// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"
#include "XsollaGenericPrimitive.h"
#include "XsollaUIBuilderTypes.h"

#include "XsollaGenericWrapper.generated.h"

class UXsollaUIBuilderTheme;

UCLASS()
class XSOLLAUIBUILDER_API UXsollaGenericWrapper : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = Default, meta = (EditCondition = "OverrideWidget == nullptr", ExposeOnSpawn = "true"))
	TEnumAsByte<EWidgetType> WidgetLibraryType;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = Default, meta = (ExposeOnSpawn = "true"))
	TSubclassOf<UXsollaGenericPrimitive> OverrideWidget;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Default|Theme", meta = (ExposeOnSpawn = "true"))
	bool OverrideDefaultTheme;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Default|Theme", meta = (EditCondition = "OverrideDefaultTheme", ExposeOnSpawn = "true"))
	TSubclassOf<UXsollaUIBuilderTheme> Theme;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Default|Theme", meta = (ExposeOnSpawn = "true"))
	FThemeParameters Parameters;

	UFUNCTION(BlueprintCallable, Category = "Theme")
	TSubclassOf<UXsollaGenericPrimitive> GetWidgetClass() const;

	UFUNCTION(BlueprintCallable, Category = "Theme")
	void UpdateThemeParameters(const FThemeParameters& InParameters);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Theme")
	void UpdateWrapperTheme();

protected:
	UFUNCTION()
	void ThemeUpdated(TSubclassOf<UXsollaUIBuilderTheme> NewTheme);

	virtual void NativeOnInitialized() override;
};