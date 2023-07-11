#pragma once

#include "CoreMinimal.h"
#include "Cannon.h"
#include "DamageTaker.h"
#include "HealthComponent.h"
#include "Engine/TargetPoint.h"

//UCLASS()
class TANKODROM_API CommonPlayer : public IDamageTaker
{
public:

	//UFUNCTION()
	//	void Fire();
	//UFUNCTION()
	//	void TakeDamage(FDamageData DamageData);
	//UFUNCTION()
	//	void DamageTaked(float DamageValue);

//protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UHealthComponent* HealthComponent;
	UPROPERTY()
		ACannon* Cannon;
};
