#include "Player/BCPlayerController.h"
#include "UI/BCChatInput.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BaseballChat.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Game/BCGameModeBase.h"
#include "BCPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Components/TextBlock.h"

ABCPlayerController::ABCPlayerController(){

	bReplicates = true;
}

void ABCPlayerController::BeginPlay(){

	Super::BeginPlay();

	if (IsLocalController() == false){
		return;
	}

	FInputModeUIOnly InputModeUIOnly;
	SetInputMode(InputModeUIOnly);

	if (IsValid(ChatInputWidgetClass) == true){

		ChatInputWidgetInstance = CreateWidget<UBCChatInput>(this, ChatInputWidgetClass);
		
		if (IsValid(ChatInputWidgetInstance) == true){

			ChatInputWidgetInstance->AddToViewport();
		}
	}

	if (IsValid(NotificationTextWidgetClass) == true)
	{
		NotificationTextWidgetInstance = CreateWidget<UUserWidget>(this, NotificationTextWidgetClass);
		if (IsValid(NotificationTextWidgetInstance) == true){

			NotificationTextWidgetInstance->AddToViewport();

			OnRep_NotificationText();
		}
	}
}

void ABCPlayerController::SetChatMessageString(const FString& InChatMessageString){

    ChatMessageString = InChatMessageString;

    if (!IsLocalController()){
        return;
    }

    // Send the original chat message to the server
    ServerRPCPrintChatMessageString(InChatMessageString);
}

void ABCPlayerController::PrintChatMessageString(const FString& InChatMessageString){

	//UKismetSystemLibrary::PrintString(this, ChatMessageString, true, true, FLinearColor::Red, 5.0f);

	//FString NetModeString = ChatXFunctionLibrary::GetNetModeString(this);
	//FString CombinedMessageString = FString::Printf(TEXT("%s: %s"), *NetModeString, *InChatMessageString);
	//ChatXFunctionLibrary::MyPrintString(this, CombinedMessageString, 10.f);

	BaseballChatFunctionLibrary::MyPrintString(this, InChatMessageString, 10.f);
}

void ABCPlayerController::ClientRPCPrintChatMessageString_Implementation(const FString& InChatMessageString){

	PrintChatMessageString(InChatMessageString);
}

void ABCPlayerController::ServerRPCPrintChatMessageString_Implementation(const FString& InChatMessageString){

	//for (TActorIterator<ABCPlayerController> It(GetWorld()); It; ++It){

		//ABCPlayerController* BCPlayerController = *It;

		//if (IsValid(BCPlayerController) == true){

			//BCPlayerController->ClientRPCPrintChatMessageString(InChatMessageString);
		//}
	//}

	UE_LOG(LogTemp, Warning,
		TEXT("SERVER RPC CALLED: %s"),
		*InChatMessageString);

	AGameModeBase* GM = UGameplayStatics::GetGameMode(this);

	if (IsValid(GM) == true){

		ABCGameModeBase* BCGM = Cast<ABCGameModeBase>(GM);

		if (IsValid(BCGM) == true){

			UE_LOG(LogTemp, Warning,
				TEXT("GAMEMODE FOUND"));

			BCGM->PrintChatMessageString(this, InChatMessageString);
		}
		else
		{
			UE_LOG(LogTemp, Error,
				TEXT("GAMEMODE CAST FAILED"));
		}
	}
}

void ABCPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, NotificationText);
}

void ABCPlayerController::OnRep_NotificationText(){

	if (!IsValid(NotificationTextWidgetInstance)){

		return;
	}

	UTextBlock* NotificationTextBlock = Cast<UTextBlock>(NotificationTextWidgetInstance->GetWidgetFromName(TEXT("NotificationText")));

	if (IsValid(NotificationTextBlock)){

		NotificationTextBlock->SetText(NotificationText);
	}
}