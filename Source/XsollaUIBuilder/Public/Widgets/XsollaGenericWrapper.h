// Copyright 2021 Xsolla Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "XsollaUIBuilderTypes.h"
#include "Blueprint/UserWidget.h"
#include "XsollaGenericWrapper.generated.h"

class UXsollaUIBuilderTheme;
/**
 * 
 */
UCLASS()
class XSOLLAUIBUILDER_API UXsollaGenericWrapper : public UUserWidget
{
	GENERATED_BODY()
public:

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = Default, meta = (EditCondition = "OverrideWidget == nullptr"))
	TEnumAsByte<EWidgetType> WidgetLibraryType;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = Default, meta = (ExposeOnSpawn = "true"))
	TSubclassOf<UUserWidget> OverrideWidget;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Default|Theme", meta = (ExposeOnSpawn = "true"))
	bool OverrideDefaultTheme;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Default|Theme", meta = (EditCondition = "OverrideDefaultTheme", ExposeOnSpawn = "true"))
	TSubclassOf<UXsollaUIBuilderTheme> Theme;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Default|Theme", meta = (ExposeOnSpawn = "true"))
	FThemeParameters Parameters;

	UFUNCTION(BlueprintCallable, Category = "Theme")
	TSubclassOf<UUserWidget> GetWidgetClass() const;

	UFUNCTION(BlueprintCallable, Category = "Theme")
	void UpdateThemeParameters(const FThemeParameters& InParameters);

private:
	

};