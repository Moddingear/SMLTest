// Fill out your copyright notice in the Description page of Project Settings.


#include "SMLGameState.h"

#include "GeneratedCodeHelpers.h"
#include "RespawnPoint.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"

void ASMLGameState::BeginPlay()
{
	Super::BeginPlay();
	SpawnedClasses.SetNum(SpawnableClasses.Num());
	for (int i = 0; i < SpawnableClasses.Num(); ++i)
	{
		SpawnedClasses[i].SpawnedAmount.SetNumZeroed(NumTeams);
	}
}

void ASMLGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ASMLGameState, SpawnableClasses, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(ASMLGameState, NumTeams, COND_InitialOnly);
	DOREPLIFETIME(ASMLGameState, SpawnedClasses);
	DOREPLIFETIME(ASMLGameState, SpawnPoints);
}

TArray<FSpawnableClass> ASMLGameState::GetSpawnableClasses(int32 TeamIndex)
{
	TArray<FSpawnableClass> Available(SpawnableClasses);
	for (int i = 0; i < SpawnableClasses.Num(); ++i)
	{
		if (SpawnedClasses.IsValidIndex(i))
		{
			if (SpawnedClasses[i].SpawnedAmount.IsValidIndex(TeamIndex))
			{
				Available[i].Count -= SpawnedClasses[i].SpawnedAmount[TeamIndex];
			}
		}
	}
	return Available;
}

TArray<FSpawnableTeam> ASMLGameState::GetSpawnableTeams()
{
	TArray<FSpawnableTeam> Teams; Teams.SetNum(NumTeams);
	for (int TeamIndex = 0; TeamIndex < NumTeams; ++TeamIndex)
	{
		Teams[TeamIndex].TeamIndex = TeamIndex;
		switch (TeamIndex)
		{
			case 0:
				Teams[TeamIndex].TeamName = NSLOCTEXT("Teams", "0", "Red");
				break;
			case 1:
				Teams[TeamIndex].TeamName = NSLOCTEXT("Teams", "1", "Grn");
				break;
			case 2:
				Teams[TeamIndex].TeamName = NSLOCTEXT("Teams", "2", "Blu");
				break;
		default:
			Teams[TeamIndex].TeamName = FText::FormatNamed(NSLOCTEXT("Teams", "Other", "Team {num}"), TEXT("num"), TeamIndex);
			break;
		}
	}
	for (FSpawnedClasses SpawnedClass : SpawnedClasses)
	{
		for (int i = 0; i < FMath::Min(SpawnedClass.SpawnedAmount.Num(), Teams.Num()); ++i)
		{
			Teams[i].NumPlayers += SpawnedClass.SpawnedAmount[i];
		}
	}
	return Teams;
}

TArray<ARespawnPoint*> ASMLGameState::GetSpawnPoints(int32 TeamIndex, TSubclassOf<ADamageableCharacter> Class)
{
	TArray<ARespawnPoint*> Spawnable;
	for (ARespawnPoint* SpawnPoint : SpawnPoints)
	{
		if(SpawnPoint->CanClassSpawnHere(Class) && SpawnPoint->Team == TeamIndex)
		{
			Spawnable.Add(SpawnPoint);
		}
	}
	return Spawnable;
}
