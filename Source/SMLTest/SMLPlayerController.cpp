// Fill out your copyright notice in the Description page of Project Settings.


#include "SMLPlayerController.h"

void ASMLPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction("Menu", IE_Pressed, this, &ASMLPlayerController::InputMenuPressed);
}

void ASMLPlayerController::InputMenuPressed_Implementation()
{
}

void ASMLPlayerController::OpenRespawnMenu_Implementation()
{
}
