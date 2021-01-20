// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "GameFramework/PlayerStart.h"
#include "SpawnPoint.generated.h"

enum class ECraftScale : uint8;
class ADamageableCharacter;
class ASpawnPoint;

DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE(FSpawnNotifyDelegate, ASpawnPoint, OnSpawn);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class SMLTEST_API ASpawnPoint : public APlayerStart
{
	GENERATED_BODY()
public:
	ASpawnPoint(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
	int32 Team;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ECraftScale MaxSpawnableScale;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
	ADamageableCharacter* SpawnedActor;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float FreeRadius; //Distance the last spawned character has to go before freeing the spawn point
	
	FSpawnNotifyDelegate OnSpawn;

protected:
	virtual void BeginPlay() override;
	
public:
	virtual void Tick(float DeltaSeconds) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	bool CanClassSpawnHere(TSubclassOf<ADamageableCharacter> Class) const;

	bool CanSpawn(TSubclassOf<ADamageableCharacter> Class) const;

	void NotifySpawn(ADamageableCharacter* InSpawnedActor);
};
