// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/BCGameStateBase.h"

#include "Kismet/GameplayStatics.h"
#include "Player/BCPlayerController.h"

void ABCGameStateBase::MulticastRPCBroadcastLoginMessage_Implementation(const FString& InNameString)
{
	if (HasAuthority() == false)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (IsValid(PC) == true)
		{
			ABCPlayerController* BCPC = Cast<ABCPlayerController>(PC);
			if (IsValid(BCPC) == true)
			{
				FString NotificationString = InNameString + TEXT(" has joined the game.");
				BCPC->PrintChatMessageString(NotificationString);
			}
		}
	}
}
