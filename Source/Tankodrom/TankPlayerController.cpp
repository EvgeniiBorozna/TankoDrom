// Fill out your copyright notice in the Description page of Project Settings.

#include "TankPlayerController.h"
#include "DrawDebugHelpers.h"
#include "TankPawn.h"
#include "Engine/Engine.h"


ATankPlayerController::ATankPlayerController()
{
	bShowMouseCursor = true;
}


void ATankPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAxis("MoveForward", this, &ATankPlayerController::MoveForward);
	InputComponent->BindAxis("RotateRight", this, &ATankPlayerController::RotateRight);
	InputComponent->BindAxis("TurretRotationRight", this, &ATankPlayerController::RotateTurretRight);
	InputComponent->BindAction("Fire", IE_Pressed, this, &ATankPlayerController::Fire);
	InputComponent->BindAction("FireAlt", IE_Pressed, this, &ATankPlayerController::FireAlt);
}

void ATankPlayerController::Tick(float DeltaTime)
{
	FVector mouseDirection;
	DeprojectMousePositionToWorld(MousePos, mouseDirection);
	FVector pawnPos = TankPawn->GetActorLocation();
	MousePos.Z = pawnPos.Z;
	
	MousePos.Y += Ydif;
	MousePos.X += Xdif;
	FVector dir = MousePos - pawnPos;
	dir.Normalize();
	FVector targetLine = pawnPos + dir * 1500;
	GEngine->AddOnScreenDebugMessage(5, 1, FColor::Blue, MousePos.ToString());
	GEngine->AddOnScreenDebugMessage(15, 1, FColor::Blue, pawnPos.ToString());
	DrawDebugLine(GetWorld(), pawnPos, targetLine, FColor::Green, false, 0.1f, 0, 3);
}

void ATankPlayerController::BeginPlay()
{
	Super::BeginPlay();

	TankPawn = Cast<ATankPawn>(GetPawn());
}

void ATankPlayerController::MoveForward(float AxisValue)
{

	TankPawn->MoveForward(AxisValue);
}

void ATankPlayerController::RotateRight(float AxisValue)
{
	TankPawn->RotateRight(AxisValue);
}

void ATankPlayerController::RotateTurretRight(float AxisValue)
{
	//GEngine->AddOnScreenDebugMessage(5, 1, FColor::Blue, FString::SanitizeFloat(AxisValue));
}

void ATankPlayerController::Fire()
{
	TankPawn->Fire();
}

void ATankPlayerController::FireAlt()
{
	TankPawn->FireAlt();
}
