#pragma once

#include "CommonPlayer.h"
#include "CoreMinimal.h"
//#include "IScorable.h"
#include "TankAIController.h"
#include "Components/BoxComponent.h"
#include "Engine/TargetPoint.h"
#include "GameFramework/Pawn.h"
#include "TankPawn.generated.h"

class UStaticMeshComponent;
class UCameraComponent;
class USpringArmComponent;
class ATankPlayerController;
class ATankAIController;
class ACannon;

UCLASS()
class TANKODROM_API ATankPawn : public APawn, public IDamageTaker//, public IIScorable, public CommonPlayer
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Pawn")
		FVector startPos;// = FVector(1000, 1000, 20);
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Pawn")
		bool humanPlayer = true;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* BodyMesh;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* TurretMesh;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* TrackRMesh;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* TrackLMesh;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* GunMesh;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		USpringArmComponent* SpringArm;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UCameraComponent* Camera;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UArrowComponent* CannonSetupPoint;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UHealthComponent* HealthComponent;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UBoxComponent* HitCollider;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Speed")
		float MoveSpeed = 500;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Speed")
		float RotationSpeed = 10;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Speed")
		float InterpolationKey = 0.1f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Turret|Speed")
		float TurretRotationInterpolationKey = 0.05f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Turret|Cannon")
		TSubclassOf<ACannon> CannonClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Turret|Cannon")
		TSubclassOf<ACannon> SecondCannonClass;
	UPROPERTY()
		ACannon* Cannon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Move params|Patrol points", Meta = (MakeEditWidget = true))
		TArray<ATargetPoint*> PatrollingPoints;
	//int32 _currentPatrolPointIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Move params|Accurency")
		float MovementAccurency = 50;

	float TargetForwardAxisValue;
	float TargetRightAxisValue;
	float CurrentRightAxisValue;
	bool Destroyed = false;

public:
	int CollectedScores = 0;

	//IIScorable* ScoreSender;

	UPROPERTY()
		ATankPlayerController* TankController;
	UPROPERTY()
		ATankAIController* TankAIController;

public:
	// Sets default values for this pawn's properties
	ATankPawn();

	UFUNCTION()
		void MoveForward(float AxisValue);
	UFUNCTION()
		void RotateRight(float AxisValue);
	UFUNCTION()
		void Movement(float DeltaTime);
	UFUNCTION()
		void RotateTurretTo(FVector TargetPosition);
	FVector GetEyesPosition();

	TArray<FVector> GetPatrollingPoints();
	void SetPatrollingPoints(TArray<ATargetPoint*> NewPatrollingPoints);
	UFUNCTION()
		void Fire();
	UFUNCTION()
		void FireAlt();
	UFUNCTION()
		void TakeDamage(FDamageData DamageData);

	//UFUNCTION()
		//TArray<ATargetPoint*> GetPatrollingPoints();// { return PatrollingPoints; };
	UFUNCTION()
		float GetMovementAccurency() { return MovementAccurency; };
	UFUNCTION()
		FVector GetTurretForwardVector();

	//UFUNCTION()
		//void SetScores(IIScorable* ScoreSender);
	UFUNCTION()
		void CollectScores(int Scores);

		void CollectShells(int);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	

public:	

	void SetupCannon(TSubclassOf<ACannon>);
	void ChangeCannon();
	UFUNCTION()
		void Die();

	UFUNCTION()
		void DamageTaked(float DamageValue);
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
