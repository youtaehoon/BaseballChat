// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/BCGameModeBase.h"
#include "BCGameStateBase.h"
#include "Player/BCPlayerController.h"
#include "EngineUtils.h"
#include "Player/BCPlayerState.h"

void ABCGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	/*ABCGameStateBase* BCGameStateBase = GetGameState<ABCGameStateBase>();
	if (IsValid(BCGameStateBase) == true)
	{
		BCGameStateBase->MulticastRPCBroadcastLoginMessage(TEXT("XXXXXXX"));
	}

	ABCPlayerController* BCPlayerController = Cast<ABCPlayerController>(NewPlayer);
	if (IsValid(BCPlayerController) == true)
	{
		AllPlayerControllers.Add(BCPlayerController);
	}
	*/

	ABCPlayerController* BCPlayerController = Cast<ABCPlayerController>(NewPlayer);
	if (IsValid(BCPlayerController) == true)
	{
		BCPlayerController->NotificationText = FText::FromString(TEXT("Connected to the game server."));

		AllPlayerControllers.Add(BCPlayerController);

		ABCPlayerState* BCPS = BCPlayerController->GetPlayerState<ABCPlayerState>();

		if (IsValid(BCPS)){

			BCPS->PlayerNameString = TEXT("Player") + FString::FromInt(AllPlayerControllers.Num());

			ABCGameStateBase* BCGameStateBase = GetGameState<ABCGameStateBase>();

			if (IsValid(BCGameStateBase)){

				BCGameStateBase->MulticastRPCBroadcastLoginMessage(BCPS->PlayerNameString);
			}
		}

	}
}

FString ABCGameModeBase::GenerateSecretNumber(){

	TArray<int32> Numbers;
	for (int32 i = 1; i <= 9; ++i){
		Numbers.Add(i);
	}

	FMath::RandInit(FDateTime::Now().GetTicks());
	Numbers = Numbers.FilterByPredicate([](int32 Num) { return Num > 0; });

	FString Result;
	for (int32 i = 0; i < 3; ++i){

		int32 Index = FMath::RandRange(0, Numbers.Num() - 1);
		Result.Append(FString::FromInt(Numbers[Index]));
		Numbers.RemoveAt(Index);
	}

	return Result;
}

bool ABCGameModeBase::IsGuessNumberString(const FString& InNumberString){

	// 1. 입력한 문자열이 3자리인지 검사
	if (InNumberString.Len() != 3){
		return false;
	}

	TSet<TCHAR> UniqueDigits;

	// 2. 입력한 숫자를 하나씩 검사
	for (TCHAR C : InNumberString){

		// 숫자가 아니거나 0이면 실패
		if (C < '1' || C > '9'){

			return false;
		}

		// 이미 입력된 숫자라면 중복이므로 실패
		if (UniqueDigits.Contains(C)){

			return false;
		}

		// 검사한 숫자를 저장
		UniqueDigits.Add(C);
	}

	// 모든 조건을 통과하면 성공
	return true;
}

FString ABCGameModeBase::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString){

	int32 StrikeCount = 0, BallCount = 0;

	for (int32 i = 0; i < 3; ++i){

		if (InSecretNumberString[i] == InGuessNumberString[i]){

			StrikeCount++;
		}
		else{

			FString PlayerGuessChar = FString::Printf(TEXT("%c"), InGuessNumberString[i]);

			if (InSecretNumberString.Contains(PlayerGuessChar)){

				BallCount++;
			}
		}
	}

	if (StrikeCount == 0 && BallCount == 0){

		return TEXT("OUT");
	}

	return FString::Printf(TEXT("%dS%dB"), StrikeCount, BallCount);
}

void ABCGameModeBase::BeginPlay(){

	Super::BeginPlay();

	SecretNumberString = GenerateSecretNumber();

}

void ABCGameModeBase::PrintChatMessageString(ABCPlayerController* InChattingPlayerController, const FString& InChatMessageString){

	if (!IsValid(InChattingPlayerController)){
		return;
	}

	ABCPlayerState* BCPS = InChattingPlayerController->GetPlayerState<ABCPlayerState>();

	if (!IsValid(BCPS)){

		return;
	}

	// Original message from the client
	FString MessageBody = InChatMessageString;

	// Add player information on the server
	FString DisplayMessage = BCPS->GetPlayerInfoString() + TEXT(": ") + MessageBody;

	// 숫자로 시작하지 않으면 일반 채팅 처리
	if (MessageBody.IsEmpty() || !FChar::IsDigit(MessageBody[0])){

		for (TActorIterator<ABCPlayerController> It(GetWorld()); It; ++It){

			if (IsValid(*It)){

				(*It)->ClientRPCPrintChatMessageString(DisplayMessage);
			}
		}

		return;
	}

	// 게임 종료 상태 검사
	if (bIsGameFinished){
		InChattingPlayerController->ClientRPCPrintChatMessageString(TEXT("Please wait for the next game.")
		);

		return;
	}

	FString GuessNumberString = MessageBody;

	// 숫자 유효성 검사
	if (!IsGuessNumberString(GuessNumberString)){

		InChattingPlayerController->ClientRPCPrintChatMessageString( TEXT("Invalid input. Please try again."));

		return;
	}

	// 시도 횟수 초과 검사
	if (BCPS->CurrentGuessCount >= BCPS->MaxGuessCount){

		InChattingPlayerController->ClientRPCPrintChatMessageString( TEXT("You have used all your attempts."));

		return;
	}

	// 정답 판정
	FString JudgeResultString = JudgeResult(SecretNumberString, GuessNumberString);

	// 시도 횟수 증가
	IncreaseGuessCount(InChattingPlayerController);

	// 현재 시도 횟수 표시
	FString CombinedMessageString = FString::Printf(TEXT("%s -> %s [%d / %d]"), *DisplayMessage, *JudgeResultString, BCPS->CurrentGuessCount, BCPS->MaxGuessCount);

	// 모든 플레이어에게 결과 전달
	for (TActorIterator<ABCPlayerController> It(GetWorld()); It; ++It){

		if (IsValid(*It)){

			(*It)->ClientRPCPrintChatMessageString(CombinedMessageString);
		}
	}

	// 승패 판정은 한 번만 실행
	int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));

	JudgeGame(InChattingPlayerController, StrikeCount);
}


void ABCGameModeBase::IncreaseGuessCount(ABCPlayerController* InChattingPlayerController){

	if (!IsValid(InChattingPlayerController)){

		return;
	}

	ABCPlayerState* BCPS = InChattingPlayerController->GetPlayerState<ABCPlayerState>();

	if (!IsValid(BCPS)){

		return;
	}

	if (BCPS->CurrentGuessCount < BCPS->MaxGuessCount){
		BCPS->CurrentGuessCount++;
	}
}

void ABCGameModeBase::ResetGame(){

	// 새로운 정답 생성
	SecretNumberString = GenerateSecretNumber();

	// 모든 플레이어 시도 횟수 초기화
	for (const auto& BCPlayerController : AllPlayerControllers){

		if (!IsValid(BCPlayerController)){

			continue;
		}

		ABCPlayerState* BCPS = BCPlayerController->GetPlayerState<ABCPlayerState>();

		if (IsValid(BCPS)){

			BCPS->CurrentGuessCount = 0;
		}

		BCPlayerController->NotificationText = FText::GetEmpty();

		BCPlayerController->ClientRPCPrintChatMessageString(TEXT("New game started!"));
	}

	bIsGameFinished = false;
}

void ABCGameModeBase::JudgeGame(ABCPlayerController* InChattingPlayerController,int InStrikeCount){

	if (bIsGameFinished){

		return;
	}

	if (!IsValid(InChattingPlayerController)){

		return;
	}

	FString ResultMessage;

	// 승리 판정
	if (InStrikeCount == 3){

		ABCPlayerState* BCPS = InChattingPlayerController->GetPlayerState<ABCPlayerState>();

		if (!IsValid(BCPS)){

			return;
		}

		ResultMessage = BCPS->PlayerNameString + TEXT(" has won the game!");
	}
	else{

		// 무승부 판정
		bool bIsDraw = true;
		int32 ValidPlayerCount = 0;

		for (const auto& BCPlayerController : AllPlayerControllers){

			if (!IsValid(BCPlayerController)){

				continue;
			}

			ABCPlayerState* BCPS = BCPlayerController->GetPlayerState<ABCPlayerState>();

			if (!IsValid(BCPS)){

				continue;
			}

			ValidPlayerCount++;

			if (BCPS->CurrentGuessCount < BCPS->MaxGuessCount){

				bIsDraw = false;
				break;
			}
		}

		if (!bIsDraw || ValidPlayerCount == 0){

			return;
		}

		ResultMessage = TEXT("The game is a draw!");
	}

	// 게임 종료 처리
	bIsGameFinished = true;

	// 모든 플레이어에게 결과 전달
	for (const auto& BCPlayerController : AllPlayerControllers){

		if (IsValid(BCPlayerController)){

			BCPlayerController->NotificationText = FText::FromString(ResultMessage);

			BCPlayerController->ClientRPCPrintChatMessageString(ResultMessage);
		}
	}

	// 5초 후 게임 리셋
	GetWorldTimerManager().SetTimer(
		ResetGameTimerHandle,
		this,
		&ABCGameModeBase::ResetGame,
		5.0f,
		false
	);
}