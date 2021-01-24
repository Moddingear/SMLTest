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

private:
	UPROPERTY(EditAnywhere)
	ECraftScale MaxSpawnableScale;

	UPROPERTY(EditAnywhere, Replicated)
	ADamageableCharacter* SpawnedActor;
	UPROPERTY(EditAnywhere)
	float FreeRadius; //Distance the last spawned character has to go before freeing the spawn point

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
	int32 Team;

	ADamageableCharacter* GetSpawnedActor() const
	{
		return SpawnedActor;
	}
	
	FSpawnNotifyDelegate OnSpawn;

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
public:
	virtual void Tick(float DeltaSeconds) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	bool CanClassSpawnHere(TSubclassOf<ADamageableCharacter> Class) const;

	bool CanSpawn(TSubclassOf<ADamageableCharacter> Class) const;

	void NotifySpawn(ADamageableCharacter* InSpawnedActor);
};
