// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnPoint.h"

#include "CraftScale.h"
#include "DamageableCharacter.h"
#include "GeneratedCodeHelpers.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"


ASpawnPoint::ASpawnPoint(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	MaxSpawnableScale = ECraftScale::Max;
	FreeRadius = 1000.f;
	USphereComponent* Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetSphereRadius(FreeRadius);
	Sphere->SetCollisionProfileName("NoCollision");
	Sphere->SetupAttachment(GetCapsuleComponent());
	SetActorTickEnabled(false);
	SetActorTickInterval(0.1);
	SetReplicates(true);
}

void ASpawnPoint::BeginPlay()
{
	Super::BeginPlay();
}

void ASpawnPoint::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if(SpawnedActor)
	{
		if(GetDistanceTo(SpawnedActor) > FreeRadius)
        {
        	SpawnedActor = nullptr;
        	SetActorTickEnabled(false);
        }
	}
}

void ASpawnPoint::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ASpawnPoint, Team, COND_InitialOnly);
	DOREPLIFETIME(ASpawnPoint, SpawnedActor);
}

bool ASpawnPoint::CanClassSpawnHere(const TSubclassOf<ADamageableCharacter> Class) const
{
	ADamageableCharacter* DefaultObject = Class.GetDefaultObject();
	return DefaultObject->CraftScale <= MaxSpawnableScale;
}

bool ASpawnPoint::CanSpawn(const TSubclassOf<ADamageableCharacter> Class) const
{
	return CanClassSpawnHere(Class) && SpawnedActor == nullptr;
}

void ASpawnPoint::NotifySpawn(ADamageableCharacter* InSpawnedActor)
{
	SetActorTickEnabled(true);
	SpawnedActor = InSpawnedActor;
	OnSpawn.Broadcast();
}
