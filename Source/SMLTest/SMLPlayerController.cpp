// Fill out your copyright notice in the Description page of Project Settings.


#include "SMLPlayerController.h"

#include "CraftScale.h"
#include "DamageableCharacter.h"
#include "GeneratedCodeHelpers.h"
#include "SMLGameState.h"
#include "SMLTest.h"
#include "RespawnPoint.h"
#include "Kismet/KismetSystemLibrary.h"

void ASMLPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	OnPossessOwner(InPawn);
}

void ASMLPlayerController::OnPossessOwner_Implementation(APawn* InPawn)
{
	if (IsLocalController())
	{
		CloseRespawnMenu();
	}
}

void ASMLPlayerController::OnUnPossess()
{
	if (IsLocalController())
 	{
		if(!GetWorld()->IsPendingKill())
		{
			OpenRespawnMenu();
		}
 	}
	if(GetWorld()->IsServer())
	{
		ASMLGameState* GS = GetWorld()->GetGameState<ASMLGameState>();
		if (GS)
		{
			GS->UnregisterSpawned(Team, LastClass);
		}
	}
	Super::OnUnPossess();
}

void ASMLPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction("Menu", IE_Pressed, this, &ASMLPlayerController::InputMenuPressed);
}

void ASMLPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASMLPlayerController, Team);
	DOREPLIFETIME(ASMLPlayerController, LastClass);
	DOREPLIFETIME(ASMLPlayerController, LastSpawnPoint);
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
	if (GetPawn())
	{
		ADamageableCharacter* PlayerPawn = GetPawn<ADamageableCharacter>();
		if (PlayerPawn)
		{
			if (!PlayerPawn->ShouldDestroyOnRespawn())
			{
				UnPossess();
				return;
			}
		}
		GetPawn()->Destroy();
		return;
	}
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

float ASMLPlayerController::GetTimeUntilRespawn(TSubclassOf<ADamageableCharacter> Class)
{
	if (IsValid(Class))
	{
		ADamageableCharacter* DefaultObject = Class->GetDefaultObject<ADamageableCharacter>();
        return DeathTime + DefaultObject->RespawnDelay - UKismetSystemLibrary::GetGameTimeInSeconds(this);
	}
	return 0;
}

void ASMLPlayerController::RespawnAndRemember(TSubclassOf<ADamageableCharacter> Class, int32 InTeam,
	ARespawnPoint* SpawnPoint)
{
	//LastClass = Class;
	//Team = InTeam;
	//LastSpawnPoint = SpawnPoint;
	AskRespawn(Class, InTeam, SpawnPoint);
}

void ASMLPlayerController::AskRespawn_Implementation(TSubclassOf<ADamageableCharacter> Class, int32 InTeam, ARespawnPoint* SpawnPoint)
{
	ASMLGameState* GS = GetWorld()->GetGameState<ASMLGameState>();
	if (IsValid(Class) && IsValid(SpawnPoint))
	{
		if (GS && SpawnPoint->Team == InTeam && GetPawn() == nullptr)
		{
		
			TArray<FSpawnableClass> SpawnableClasses = GS->GetSpawnableClasses(Team);
			if(GS->CanSpawn(InTeam, Class))
			{
				for (FSpawnableClass SpawnableClass : SpawnableClasses)
				{
					if (SpawnableClass.Class == Class)
					{
						//do respawn, remove one
						//return
						ADamageableCharacter* DefaultObject = Class->GetDefaultObject<ADamageableCharacter>();
						if(GetTimeUntilRespawn(Class) <= 0)
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
								SpawnPoint->NotifySpawn(NewCharacter);
							}
							LastClass = Class;
							Team = InTeam;
							LastSpawnPoint = SpawnPoint;
							GS->RegisterSpawned(InTeam, Class);
							return;
						}
				
					}
				}
			}
		}
		else
		{
			UE_LOG(LogSML, Warning, TEXT("[ASMLPlayerController::AskRespawn_Implementation] Failed check 2 : GS is %s, SpawnPoint->Team = %d, Team = %d, GetPawn is %s"),
                IsValid(GS) ? TEXT("VALID") : TEXT("INVALID"), SpawnPoint->Team, Team, IsValid(GetPawn()) ? TEXT("VALID") : TEXT("INVALID"));
		}
	}
	else
	{
		UE_LOG(LogSML, Warning, TEXT("[ASMLPlayerController::AskRespawn_Implementation] Failed check 1 : Class is %s, SpawnPoint is %s"),
            IsValid(Class) ? TEXT("VALID") : TEXT("INVALID"), IsValid(SpawnPoint) ? TEXT("VALID") : TEXT("INVALID"));
	}
}

bool ASMLPlayerController::AskRespawn_Validate(TSubclassOf<ADamageableCharacter> Class, int32 InTeam, ARespawnPoint*)
{
	return true;
}
