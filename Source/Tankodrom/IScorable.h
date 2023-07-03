#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IScorable.generated.h"

DECLARE_DELEGATE_OneParam(FScoreDelegate, int);
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UIScorable : public UInterface
{
	GENERATED_BODY()
};

class TANKODROM_API IIScorable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void TakeScore(int Scores) = 0;

	FScoreDelegate ScoreDelegate;
};
