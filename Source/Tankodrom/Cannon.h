#pragma once

#include "Projectile.h"
#include "CoreMinimal.h"
#include "GameStructs.h"
#include "GameFramework/Actor.h"
#include "Components/AudioComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Cannon.generated.h"

class UArrowComponent;

UCLASS()
class TANKODROM_API ACannon : public AActor
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* Mesh;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UArrowComponent* ProjectileSpawnPoint;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UParticleSystemComponent* ShootEffect;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UAudioComponent* AudioEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fire params")
		float FireRate = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fire params")
		float FireRange = 1000;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fire params")
		float FireDamage = 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fire params")
		int ShellsCount = 20;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fire params")
		TSubclassOf<AProjectile> ProjectileClass;

	FTimerHandle ReloadTimerHandle;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fire params")
		ECannonType Type = ECannonType::FireProjectile;

	bool ReadyToFire = false;
public:
	ACannon();

	void Fire();
	void AddShells(int);

	bool IsReadyToFire();

protected:
	virtual void BeginPlay() override;

	void Reload();

};