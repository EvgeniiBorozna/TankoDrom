
#include "Turret.h"
#include "TankPlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ATurret::ATurret()
{
	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret body"));
	RootComponent = BodyMesh;

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret turret"));
	TurretMesh->AttachToComponent(BodyMesh, FAttachmentTransformRules::KeepRelativeTransform);

	CannonSetupPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("Cannon setup point"));
	CannonSetupPoint->AttachToComponent(TurretMesh, FAttachmentTransformRules::KeepRelativeTransform);

	HitCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Hit collider"));
	HitCollider->SetupAttachment(TurretMesh);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health component"));
	HealthComponent->OnDie.AddUObject(this, &ATurret::Die);
	HealthComponent->OnDamaged.AddUObject(this, &ATurret::DamageTaked);

}

void ATurret::TakeDamage(FDamageData DamageData)
{
	UE_LOG(LogTemp, Warning, TEXT("Turret %s taked damage:%f "), *GetName(), DamageData.DamageValue);
	HealthComponent->TakeDamage(DamageData);
	GEngine->AddOnScreenDebugMessage(45, 1, FColor::Blue, FString::SanitizeFloat(HealthComponent->GetHealth()));
}

// Called when the game starts or when spawned
void ATurret::BeginPlay()
{
	Super::BeginPlay();

	FActorSpawnParameters params;
	params.Owner = this;
	Cannon = GetWorld()->SpawnActor<ACannon>(CannonClass, params);
	Cannon->AttachToComponent(CannonSetupPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();

	FTimerHandle _targetingTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(_targetingTimerHandle, this, &ATurret::Targeting, TargetingRate, 
		true, TargetingRate);

	//UStaticMesh* turretMeshTemp = LoadObject<UStaticMesh>(this, *TurretMeshPath);
	//if (turretMeshTemp)
	//	TurretMesh->SetStaticMesh(turretMeshTemp);

	//UStaticMesh* bodyMeshTemp = LoadObject<UStaticMesh>(this, *BodyMeshPath);
	//if (bodyMeshTemp)
	//	BodyMesh->SetStaticMesh(bodyMeshTemp);
	
}

void ATurret::Destroyed()
{
	if (Cannon)
		Cannon->Destroy();
}

void ATurret::Targeting()
{
	if (IsPlayerInRange())
	{
		RotateToPlayer();
	}

	if (CanFire() && Cannon && Cannon->IsReadyToFire() && IsPlayerInRange())
	{
		Fire();
	}
	FVector CurrentPosition;
	CurrentPosition = BodyMesh->GetComponentLocation();
	FVector forwardVector = GetActorForwardVector();
	FVector movePosition = CurrentPosition + forwardVector * DirIt * 0.4;
	SetActorLocation(movePosition, true);
	Counter += DirIt;
	if (Counter == GuardingRange || Counter == 0) {
		DirIt *= -1;
	}
}

void ATurret::RotateToPlayer()
{
	FRotator targetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PlayerPawn->GetActorLocation());
	FRotator currRotation = TurretMesh->GetComponentRotation();
	targetRotation.Pitch = currRotation.Pitch;
	targetRotation.Roll = currRotation.Roll;
	//GEngine->AddOnScreenDebugMessage(45, 1, FColor::Blue, FString::SanitizeFloat(currRotation.Yaw));
	//GEngine->AddOnScreenDebugMessage(55, 1, FColor::Blue, FString::SanitizeFloat(targetRotation.Yaw));
	//
	TurretMesh->SetWorldRotation(FMath::Lerp(currRotation, targetRotation, TargetingSpeed));
}

bool ATurret::IsPlayerInRange()
{
	if (!PlayerPawn)
		PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();

	if (!PlayerPawn)
		return false;

	return FVector::Distance(PlayerPawn->GetActorLocation(), GetActorLocation()) <= TargetingRange;
}

bool ATurret::CanFire()
{
	if (!PlayerPawn)
		PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
		

	if (!PlayerPawn)
		return false;

	FVector targetingDir = TurretMesh->GetForwardVector();
	FVector dirToPlayer = PlayerPawn->GetActorLocation() - GetActorLocation();
	dirToPlayer.Normalize();
	float aimAngle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(targetingDir, dirToPlayer)));
//GEngine->AddOnScreenDebugMessage(45, 1, FColor::Blue, FString::SanitizeFloat(aimAngle));
	return (aimAngle) <= Accurency;
}

void ATurret::Fire()
{
	if (Cannon)
		Cannon->Fire();
}

void ATurret::Die()
{
	DirIt = 0;
	TurretMesh->SetRelativeRotation(FRotator(-10, -60, 20));
	TurretMesh->SetRelativeLocation(FVector(0, 0, 150));
	//Destroy();
	////TakeScore(10);
}

void ATurret::DamageTaked(float DamageValue)
{
	UE_LOG(LogTemp, Warning, TEXT("Turret %s taked damage:%f Health:%f"), *GetName(), DamageValue, HealthComponent->GetHealth());
	
}

//void ATurret::TakeScore(int Scores)
//{
	
//}
