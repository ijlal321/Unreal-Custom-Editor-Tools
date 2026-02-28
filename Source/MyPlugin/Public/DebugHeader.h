#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Notifications/NotificationManager.h"

namespace DebugHeader {
	static void Print(const FString& message, const FColor& color)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 20.0f, color, message);
		}
	}

	static void PrintLog(const FString& message)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *message);
	}

	static EAppReturnType::Type showMsgDialog(EAppMsgType::Type msgType, const FString& message, bool showMsgAsWarning = true)
	{
		if (showMsgAsWarning)
		{
			FText msgTitle = FText::FromString("Warning");
			return FMessageDialog::Open(msgType, FText::FromString(message), msgTitle);
		}
		else
		{
			return FMessageDialog::Open(msgType, FText::FromString(message));
		}
	}

	static void showNotifyInfo(const FString& message)
	{
		FNotificationInfo notifyInfo(FText::FromString(message));
		notifyInfo.bUseLargeFont = true;
		notifyInfo.ExpireDuration = 7.0f;

		FSlateNotificationManager::Get().AddNotification(notifyInfo);
	}
}
