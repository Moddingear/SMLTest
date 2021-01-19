// Fill out your copyright notice in the Description page of Project Settings.


#include "SMLPlayerController.h"

void ASMLPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (IsLocalController())
	{
		CloseRespawnMenu();
	}
}

void ASMLPlayerController::OnUnPossess()
{
	Super::OnUnPossess();
	if (IsLocalController())
	{
		OpenRespawnMenu();
	}
}

void ASMLPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction("Menu", IE_Pressed, this, &ASMLPlayerController::InputMenuPressed);
}

void ASMLPlayerController::InputMenuPressed_Implementation()
{
	OpenEscMenu();
}

void ASMLPlayerController::OpenEscMenu_Implementation()
{
}

void ASMLPlayerController::CloseEscMenu_Implementation()
{
}

void ASMLPlayerController::ForceRespawn_Implementation()
{
	GetPawn()->Destroy();
}

bool ASMLPlayerController::ForceRespawn_Validate()
{
	return true;
}

void ASMLPlayerController::OpenRespawnMenu_Implementation()
{
}

void ASMLPlayerController::CloseRespawnMenu_Implementation()
{
}

void ASMLPlayerController::AskRespawn_Implementation(TSubclassOf<ADamageableCharacter> Class)
{
	//todo 
}

bool ASMLPlayerController::AskRespawn_Validate(TSubclassOf<ADamageableCharacter> Class)
{
	return true;
}
