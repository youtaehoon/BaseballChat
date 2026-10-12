// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BCPlayerController.generated.h"

class UBCChatInput;
class UUserWidget;

/**
 * 
 */
UCLASS()
class BASEBALLCHAT_API ABCPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ABCPlayerController();

	virtual void BeginPlay() override;

	void SetChatMessageString(const FString& InChatMessageString);

	void PrintChatMessageString(const FString& InChatMessageString);

	UFUNCTION(Client, Reliable)
	void ClientRPCPrintChatMessageString(const FString& InChatMessageString);

	UFUNCTION(Server, Reliable)
	void ServerRPCPrintChatMessageString(const FString& InChatMessageString);

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBCChatInput> ChatInputWidgetClass;

	UPROPERTY()
	TObjectPtr<UBCChatInput> ChatInputWidgetInstance;
	
	FString ChatMessageString;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> NotificationTextWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> NotificationTextWidgetInstance;

public:
	UPROPERTY(ReplicatedUsing = OnRep_NotificationText)
	FText NotificationText;

	UFUNCTION()
	void OnRep_NotificationText();
};
