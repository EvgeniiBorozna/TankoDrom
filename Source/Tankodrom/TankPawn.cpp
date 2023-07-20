
//#include "IScorable.h"

#include "TankPawn.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/TargetPoint.h"
#include "TankPlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/ArrowComponent.h"

//DECLARE_LOG_CATEGORY_EXTERN(TankLog, All, All);
//DEFINE_LOG_CATEGORY(TankLog);

// Sets default values
ATankPawn::ATankPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tank body"));
	RootComponent = BodyMesh;

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tank turret"));
	TurretMesh->SetupAttachment(BodyMesh);

	TrackRMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tank trackR"));
	TrackRMesh->SetupAttachment(BodyMesh);

	TrackLMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tank trackL"));
	TrackLMesh->SetupAttachment(BodyMesh);

	//GunMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tank gun"));
	//GunMesh->SetupAttachment(TurretMesh);
	CannonSetupPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("Cannon setup point"));
	CannonSetupPoint->AttachToComponent(TurretMesh, FAttachmentTransformRules::KeepRelativeTransform);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring arm"));
	SpringArm->SetupAttachment(BodyMesh);
	SpringArm->bDoCollisionTest = false;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritYaw = false;
	SpringArm->bInheritRoll = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health component"));
	HealthComponent->OnDie.AddUObject(this, &ATankPawn::Die);
	HealthComponent->OnDamaged.AddUObject(this, &ATankPawn::DamageTaked);

	HitCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Hit collider"));
	HitCollider->SetupAttachment(BodyMesh);

	//TSharedRef ScoreSender = new FScoreDelegate());
	//ScoreDelegate.BindSP(ScoreSender, &ATankPawn::CollectScores, 1);
}

void ATankPawn::MoveForward(float AxisValue)
{
	if (this) {
		TargetForwardAxisValue = AxisValue;
	}
}

void ATankPawn::RotateRight(float AxisValue)
{
	TargetRightAxisValue = AxisValue;
}

void ATankPawn::Movement(float DeltaTime)
{
	FVector currentLocation = GetActorLocation();
	FVector forwardVector = GetActorForwardVector();
	FVector movePosition = currentLocation + forwardVector * MoveSpeed * TargetForwardAxisValue * DeltaTime;
	SetActorLocation(movePosition, true);

	CurrentRightAxisValue = FMath::Lerp(CurrentRightAxisValue, TargetRightAxisValue, InterpolationKey);
	float yawRotation = RotationSpeed * CurrentRightAxisValue * DeltaTime;
	FRotator currentRotation = GetActorRotation();
	yawRotation = currentRotation.Yaw + yawRotation;
	FRotator newRotation = FRotator(0, yawRotation, 0);
	SetActorRotation(newRotation);

	FRotator ArmRot = SpringArm->GetTargetRotation();
	if(humanPlayer) {
		TankController->Xdif = (SpringArm->TargetArmLength - 10) * cos(ArmRot.Pitch * 3.14159 / 180) * cos(ArmRot.Yaw * 3.14159 / 180);
		TankController->Ydif = (SpringArm->TargetArmLength - 10) * cos(ArmRot.Pitch * 3.14159 / 180) * sin(ArmRot.Yaw * 3.14159 / 180);
	}

	if (TankController)
	{
		FVector mousePos = TankController->GetMousePos();
		//UE_LOG(TankLog, Warning, TEXT("MousePos() = %f"), mousePos);
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, mousePos.ToString());
		if (currentLocation == GetActorLocation())
			RotateTurretTo(mousePos);

	}
}

void ATankPawn::RotateTurretTo(FVector TargetPosition)
{
	FRotator targetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetPosition);
	FRotator currRotation = TurretMesh->GetComponentRotation();
	targetRotation.Pitch = currRotation.Pitch;
	targetRotation.Roll = currRotation.Roll;
	//GEngine->AddOnScreenDebugMessage(5, 15.0f, FColor::Red, "c: " + FString::SanitizeFloat(currRotation.Yaw- targetRotation.Yaw));
	//GEngine->AddOnScreenDebugMessage(15, 15.0f, FColor::Red, "t: " + FString::SanitizeFloat(targetRotation.Yaw));
	//GEngine->AddOnScreenDebugMessage(15, 15.0f, FColor::Red, targetRotation.ToString());
	if (!Destroyed)
	TurretMesh->SetWorldRotation(FMath::Lerp(currRotation, targetRotation, TurretRotationInterpolationKey));
}

FVector ATankPawn::GetEyesPosition()
{
	return CannonSetupPoint->GetComponentLocation();
}

void ATankPawn::SetPatrollingPoints(TArray<ATargetPoint*> NewPatrollingPoints)
{
	//FVector scale = GetActorScale3D();
	PatrollingPoints = NewPatrollingPoints;
}

void ATankPawn::Fire()
{
	if (!Destroyed)
	if (Cannon)
	{
		Cannon->Fire();
	}
}

void ATankPawn::FireAlt()
{
	if(!Destroyed)
	if (Cannon)
	{
		Cannon->Fire();
	}
}

void ATankPawn::CollectShells(int count)
{
	Cannon->AddShells(count);
}

// Called when the game starts or when spawned
void ATankPawn::BeginPlay()
{
	Super::BeginPlay();
	TankController = Cast<ATankPlayerController>(GetController());
	TankAIController = Cast<ATankAIController>(GetController());
	SetupCannon(CannonClass);

	if (!GetController())
	{
		SpawnDefaultController();
	}
	if (humanPlayer) {
		SetActorLocation(startPos);
	}
	//
	//PatrollingPoints.Add()
}

void ATankPawn::SetupCannon(TSubclassOf<ACannon> NewCannonClass)
{
	if (Cannon)
	{
		Cannon->Destroy();
	}
	FActorSpawnParameters params;
	params.Instigator = this;
	params.Owner = this;
	Cannon = GetWorld()->SpawnActor<ACannon>(NewCannonClass, params);
	Cannon->AttachToComponent(CannonSetupPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}

void ATankPawn::ChangeCannon()
{
	TSubclassOf<ACannon> temp = CannonClass;
	SetupCannon(SecondCannonClass);
	SecondCannonClass = temp;
}

// Called every frame
void ATankPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(!Destroyed)
		ATankPawn::Movement(DeltaTime);
}

// Called to bind functionality to input
void ATankPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ATankPawn::Die()
{
	//if (Cannon)
		//Cannon->AddActorLocalRotation(FRotator(-10, -60, 20));
	if (!Destroyed) {
		BodyMesh->AddLocalRotation(FRotator(-10, -60, 20));
		MoveSpeed = 0;
		RotationSpeed = 0;
		Destroyed = true;
		//Destroy();
	}
}

void ATankPawn::DamageTaked(float DamageValue)
{
	//UE_LOG(TankLog, Warning, TEXT("Tank %s taked damage:%f Health:%f"), *GetName(), DamageValue, HealthComponent->GetHealth());
	/*FDamageData DamageData;
	DamageData.DamageValue = DamageValue;
	HealthComponent->TakeDamage(DamageData);*/
	
}

void ATankPawn::TakeDamage(FDamageData DamageData)
{
	HealthComponent->TakeDamage(DamageData);
	GEngine->AddOnScreenDebugMessage(25, 1, FColor::Red, FString::SanitizeFloat(HealthComponent->GetHealth()));
}

TArray<FVector> ATankPawn::GetPatrollingPoints()
{
	TArray<FVector> points;
	for (ATargetPoint* point : PatrollingPoints)
	{
		points.Add(point->GetActorLocation());
	}

	return points;
}

FVector ATankPawn::GetTurretForwardVector()
{
	return TurretMesh->GetForwardVector();
}

//void ATankPawn::SetScores(IIScorable* ScoreSender)
//{
	//ScoreSender->ScoreDelegate.BindUObject(this, &ATankPawn::CollectScores);
//}

void ATankPawn::CollectScores(int Scores)
{
	
	GEngine->AddOnScreenDebugMessage(15, 15.0f, FColor::Red, "Scores collected");
}

