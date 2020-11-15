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
	IAutomationDriverModule::Get().Enable();

	FAutomationDriverPtr Driver = IAutomationDriverModule::Get().CreateDriver();
	FDriverSequenceRef Sequence = Driver->CreateSequence();
	Sequence->Actions()
		.MoveToElement(By::Cursor())
		.Press(TEXT("W"));

	Sequence->Perform();

	Driver.Reset();
	IAutomationDriverModule::Get().Disable();

	WorldContextObject->GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UAsyncTestWriteText::ExecuteAfterWriteText, 1.0f);
}

void UAsyncTestWriteText::ExecuteAfterWriteText()
{
	AfterWriteText.Broadcast();
}
