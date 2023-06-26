#include "TankPawn.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
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

	////Tiger
	//TigerBodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tiger body"));
	////RootComponent = BodyMesh;

	//TigerTurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tiger turret"));
	//TigerTurretMesh->SetupAttachment(TigerBodyMesh);

}

void ATankPawn::MoveForward(float AxisValue)
{
	TargetForwardAxisValue = AxisValue;
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
	TankController->Xdif = (SpringArm->TargetArmLength - 10) * cos(ArmRot.Pitch * 3.14159 / 180) * cos(ArmRot.Yaw * 3.14159 / 180);
	TankController->Ydif = (SpringArm->TargetArmLength - 10) * cos(ArmRot.Pitch * 3.14159 / 180) * sin(ArmRot.Yaw * 3.14159 / 180);
	GEngine->AddOnScreenDebugMessage(35, 1, FColor::Blue, FString::SanitizeFloat(TankController->Xdif));
	GEngine->AddOnScreenDebugMessage(36, 1, FColor::Blue, FString::SanitizeFloat(TankController->Ydif));
	//GEngine->AddOnScreenDebugMessage(40, 1, FColor::Red, FString::SanitizeFloat(SpringArm->TargetArmLength));

	FVector ActLoc = GetActorLocation();
	GEngine->AddOnScreenDebugMessage(45, 1, FColor::Blue, ActLoc.ToString());

	// Turret rotation
	//UE_LOG(TankLog, Warning, TEXT("TankController = %f"), TankController);
	if (TankController)
	{
		FVector mousePos = TankController->GetMousePos();
		//UE_LOG(TankLog, Warning, TEXT("MousePos() = %f"), mousePos);
		
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, mousePos.ToString());
		RotateTurretTo(mousePos);
		// FRotator targetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), mousePos);
		// FRotator currRotation = TurretMesh->GetComponentRotation();
		// targetRotation.Pitch = currRotation.Pitch;
		// targetRotation.Roll = currRotation.Roll;
		// TurretMesh->SetWorldRotation(FMath::Lerp(currRotation, targetRotation, TurretRotationInterpolationKey));
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
	TurretMesh->SetWorldRotation(FMath::Lerp(currRotation, targetRotation, TurretRotationInterpolationKey));
	//TurretMesh->SetWorldRotation(FMath::Lerp(currRotation, FRotator(0.f, targetRotation.Yaw, 00.f), TurretRotationInterpolationKey));
	//TurretMesh->SetWorldRotation(targetRotation);
}

void ATankPawn::Fire()
{
	if (Cannon)
	{
		Cannon->Fire(ECannonType::FireProjectile);
	}
}

void ATankPawn::FireAlt()
{
	if (Cannon)
	{
		Cannon->Fire(ECannonType::FireTrace);
	}
}

// Called when the game starts or when spawned
void ATankPawn::BeginPlay()
{
	Super::BeginPlay();
	TankController = Cast<ATankPlayerController>(GetController());
	SetupCannon();
	FVector startPos = FVector(1000, 1000, 50);
	SetActorLocation(startPos);
}

void ATankPawn::SetupCannon()
{
	if (Cannon)
	{
		Cannon->Destroy();
	}
	FActorSpawnParameters params;
	params.Instigator = this;
	params.Owner = this;
	Cannon = GetWorld()->SpawnActor<ACannon>(CannonClass, params);
	Cannon->AttachToComponent(CannonSetupPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}

// Called every frame
void ATankPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ATankPawn::Movement(DeltaTime);

}

// Called to bind functionality to input
void ATankPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

