// Fill out your copyright notice in the Description page of Project Settings.


#include "SMLGameState.h"

#include "GeneratedCodeHelpers.h"

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
