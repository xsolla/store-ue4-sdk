// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncTestWriteText.h"

#include "AutomationDriverCommon.h"
#include "AutomationDriverTypeDefs.h"
#include "IAutomationDriver.h"
#include "IAutomationDriverModule.h"
#include "IDriverElement.h"
#include "LocateBy.h"

UAsyncTestWriteText::UAsyncTestWriteText(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), WorldContextObject(nullptr), Text("")
{
}

UAsyncTestWriteText* UAsyncTestWriteText::WriteText(const UObject* WorldContextObject, const FString& Text)
{
	UAsyncTestWriteText* BlueprintNode = NewObject<UAsyncTestWriteText>();
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->Text = Text;
	return BlueprintNode;
}

void UAsyncTestWriteText::Activate()
{
	if (IAutomationDriverModule::Get().IsEnabled())
	{
		IAutomationDriverModule::Get().Disable();
	}
	IAutomationDriverModule::Get().Enable();

	AsyncTask(ENamedThreads::AnyThread, [this]
	{
		FAutomationDriverPtr Driver = IAutomationDriverModule::Get().CreateDriver();
		FDriverElementPtr Element = Driver->FindElement(By::Cursor());
		Element->Type(Text);

		Driver.Reset();
		AsyncTask(ENamedThreads::GameThread, [this]
		{
			IAutomationDriverModule::Get().Disable();
			this->ExecuteAfterWriteText();
		});
	});
}

void UAsyncTestWriteText::ExecuteAfterWriteText()
{
	AfterWriteText.Broadcast();
}
