// Fill out your copyright notice in the Description page of Project Settings.


#include "SMLTestGameMode.h"


#include "DamageableCharacter.h"
#include "SMLPlayerController.h"

ASMLTestGameMode::ASMLTestGameMode()
{
	PlayerControllerClass = ASMLPlayerController::StaticClass();
	DefaultPawnClass = ADamageableCharacter::StaticClass();
	MinRespawnDelay = 3;
}
