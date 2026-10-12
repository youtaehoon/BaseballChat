#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BCPawn.generated.h"

UCLASS()
class BASEBALLCHAT_API ABCPawn : public APawn
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

};
