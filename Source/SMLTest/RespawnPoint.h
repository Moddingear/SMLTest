// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "RespawnPoint.generated.h"

class USphereComponent;
enum class ECraftScale : uint8;
class ADamageableCharacter;
class ARespawnPoint;

DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE(FSpawnNotifyDelegate, ARespawnPoint, OnSpawn);
DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE(FSpawnEmptiedNotifyDelegate, ARespawnPoint, OnEmptied);
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class SMLTEST_API ARespawnPoint : public AActor
{
	GENERATED_BODY()
public:
	ARespawnPoint();

private:
	UPROPERTY(EditAnywhere, BlueprintGetter=GetMaxSpawnableScale)
	ECraftScale MaxSpawnableScale;
	
	UPROPERTY(EditAnywhere, Replicated, BlueprintGetter=GetSpawnedActor)
	ADamageableCharacter* SpawnedActor;
	UPROPERTY(EditAnywhere)
	float FreeRadius; //Distance the last spawned character has to go before freeing the spawn point

public:
	UPROPERTY(BlueprintReadOnly, Replicated)
	int64 OwnerIndex;
	UPROPERTY(BlueprintReadOnly, Replicated)
	TSubclassOf<ADamageableCharacter> OwnerClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
	int32 Team;

	UFUNCTION(BlueprintPure)
	ECraftScale GetMaxSpawnableScale() const
	{
		return MaxSpawnableScale;
	}
	
	UFUNCTION(BlueprintPure)
	ADamageableCharacter* GetSpawnedActor() const
	{
		return SpawnedActor;
	}
	
	FSpawnNotifyDelegate OnSpawn;
	FSpawnEmptiedNotifyDelegate OnEmptied;

	UPROPERTY()
	USphereComponent* Sphere;

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
public:
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure)
	bool CanClassSpawnHere(TSubclassOf<ADamageableCharacter> Class) const;

	UFUNCTION(BlueprintPure)
	bool CanSpawn(TSubclassOf<ADamageableCharacter> Class) const;

	void NotifySpawn(ADamageableCharacter* InSpawnedActor);
};
