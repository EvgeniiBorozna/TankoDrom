#include "Cannon.h"

//#include "DrawDebugHelpers.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "Engine/Engine.h"

ACannon::ACannon()
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* sceeneCpm = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = sceeneCpm;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cannon mesh"));
	Mesh->SetupAttachment(RootComponent);

	ProjectileSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("Spawn point"));
	ProjectileSpawnPoint->SetupAttachment(Mesh);
	
	ShootEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Shoot effect"));
	ShootEffect->SetupAttachment(ProjectileSpawnPoint);

	AudioEffect = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio effect"));
	//AudioEffect->SetupAttachment(ProjectileSpawnPoint);
}
void ACannon::Fire()
{
	if (!ReadyToFire)
	{
		return;
	}
	ReadyToFire = false;
	ShootEffect->ActivateSystem();
	AudioEffect->Play();
		if (ShellsCount > 0) {
			AProjectile* projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass,
				ProjectileSpawnPoint->GetComponentLocation(),
				ProjectileSpawnPoint->GetComponentRotation());
			if (projectile)
			{
				projectile->Start();
			}
			GEngine->AddOnScreenDebugMessage(10, 1, FColor::Green, "Fire - projectile");
			ShellsCount--;
			GEngine->AddOnScreenDebugMessage(25, 1, FColor::Green, FString::SanitizeFloat(ShellsCount));
			GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &ACannon::Reload, 1 / FireRate, false);
		}
		else
			GEngine->AddOnScreenDebugMessage(25, 1, FColor::Red, "No shells");

	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &ACannon::Reload, 1 / FireRate, false);
}

void ACannon::AddShells(int count)
{
	ShellsCount += count;
	GEngine->AddOnScreenDebugMessage(15, 15.0f, FColor::Red, FString::Printf(TEXT("Shells count: %d"), ShellsCount));
}

bool ACannon::IsReadyToFire()
{
	return ReadyToFire;
}

void ACannon::Reload()
{
	ReadyToFire = true;
}

// Called when the game starts or when spawned
void ACannon::BeginPlay()
{
	Super::BeginPlay();
	Reload();
}