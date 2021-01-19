// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/GameState.h"
#include "SMLGameState.generated.h"

class ADamageableCharacter;

USTRUCT(BlueprintType)
struct FSpawnableClass
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ADamageableCharacter> Class;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Count;
};

USTRUCT(BlueprintType)
struct FSpawnedClasses
{
	GENERATED_BODY()

	TArray<int32> SpawnedAmount; //SpawnedAmount[i] is the number spawned of for team i
};

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class SMLTEST_API ASMLGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite)
	TArray<FSpawnableClass> SpawnableClasses; //Only server side, used to set up the arrays

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite)
	int32 NumTeams;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite)
	TArray<FSpawnedClasses> SpawnedClasses;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	virtual TArray<FSpawnableClass> GetSpawnableClasses(int32 TeamIndex); //Get all the classes that can be spawned as
};
