// Fill out your copyright notice in the Description page of Project Settings.


#include "SMLPlayerController.h"

#include "CraftScale.h"
#include "DamageableCharacter.h"
#include "SMLGameState.h"
#include "SMLTest.h"
#include "SpawnPoint.h"
#include "Kismet/KismetSystemLibrary.h"

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

void ASMLPlayerController::AskRespawn_Implementation(TSubclassOf<ADamageableCharacter> Class, int32 Team, ASpawnPoint* SpawnPoint)
{
	ASMLGameState* GS = GetWorld()->GetGameState<ASMLGameState>();
	if (GS && SpawnPoint->Team == Team && GetPawn() == nullptr)
	{
		
		TArray<FSpawnableClass> SpawnableClasses = GS->GetSpawnableClasses(Team);
		for (FSpawnableClass SpawnableClass : SpawnableClasses)
		{
			if (SpawnableClass.Class == Class)
			{
				//do respawn, remove one
				//return
				ADamageableCharacter* DefaultObject = Class->GetDefaultObject<ADamageableCharacter>();
				if(UKismetSystemLibrary::GetGameTimeInSeconds(this) + DefaultObject->RespawnDelay > DeathTime)
				{
					if(DefaultObject->CraftScale == ECraftScale::Max || DefaultObject->CraftScale == ECraftScale::None)
					{
						UE_LOG(LogSML, Error, TEXT("Class %s has an invalid craft scale : %d"), *Class->GetName(), DefaultObject->CraftScale);
					}
					if(SpawnPoint->CanSpawn(Class))
					{
						ADamageableCharacter* NewCharacter = GetWorld()->SpawnActorDeferred<ADamageableCharacter>(Class, SpawnPoint->GetTransform(), this, nullptr);
						NewCharacter->Team = Team;
						NewCharacter->FinishSpawning(SpawnPoint->GetTransform());
						Possess(NewCharacter);
					}
					return;
				}
				
			}
		}
	}
}

bool ASMLPlayerController::AskRespawn_Validate(TSubclassOf<ADamageableCharacter> Class, int32 Team, ASpawnPoint*)
{
    
	return true;
}
