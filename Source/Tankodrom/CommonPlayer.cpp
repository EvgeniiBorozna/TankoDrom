#include "CommonPlayer.h"

CommonPlayer::CommonPlayer()
{
}

CommonPlayer::~CommonPlayer()
{
}

void CommonPlayer::Fire()
{
	if (Cannon)
	{
		Cannon->Fire();
	}
}

void CommonPlayer::TakeDamage(FDamageData DamageData)
{
	HealthComponent->TakeDamage(DamageData);
	GEngine->AddOnScreenDebugMessage(25, 1, FColor::Red, FString::SanitizeFloat(HealthComponent->GetHealth()));
}

void CommonPlayer::DamageTaked(float DamageValue)
{
	GEngine->AddOnScreenDebugMessage(45, 1, FColor::Blue, TEXT("Turret taked damage"));
}
