// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BCGameModeBase.generated.h"

class ABCPlayerController;

/**
 * 
 */
UCLASS()
class BASEBALLCHAT_API ABCGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void OnPostLogin(AController* NewPlayer) override;

	FString GenerateSecretNumber();

	bool IsGuessNumberString(const FString& InNumberString);

	FString JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString);

	virtual void BeginPlay() override;

	void PrintChatMessageString(ABCPlayerController* InChattingPlayerController, const FString& InChatMessageString);

	void IncreaseGuessCount(ABCPlayerController* InChattingPlayerController);

	void ResetGame();

	void JudgeGame(ABCPlayerController* InChattingPlayerController, int InStrikeCount);

protected:
	FString SecretNumberString;

	TArray<TObjectPtr<ABCPlayerController>> AllPlayerControllers;

	// 게임 종료 후 리셋까지 대기하는 상태
	bool bIsGameFinished = false;

	// 게임 리셋 타이머
	FTimerHandle ResetGameTimerHandle;
};
