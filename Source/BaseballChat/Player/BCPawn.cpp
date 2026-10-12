#include "Player/BCPawn.h"
#include "BaseballChat.h"


void ABCPawn::BeginPlay()
{
	Super::BeginPlay();

	FString NetRoleString = BaseballChatFunctionLibrary::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("BCPawn::BeginPlay() %s [%s]"), *BaseballChatFunctionLibrary::GetNetModeString(this), *NetRoleString);
	BaseballChatFunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}

void ABCPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	FString NetRoleString = BaseballChatFunctionLibrary::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("BCPawn::PossessedBy() %s [%s]"), *BaseballChatFunctionLibrary::GetNetModeString(this), *NetRoleString);
	BaseballChatFunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}


