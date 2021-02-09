// Fill out your copyright notice in the Description page of Project Settings.


#include "RespawnPoint.h"

#include "CraftScale.h"
#include "DamageableCharacter.h"
#include "GeneratedCodeHelpers.h"
#include "SMLGameState.h"
#include "SMLTest.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"


ARespawnPoint::ARespawnPoint()
{
	MaxSpawnableScale = ECraftScale::Max;
	FreeRadius = 1000.f;
	USphereComponent* Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetSphereRadius(FreeRadius);
	Sphere->SetCollisionProfileName("NoCollision");
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = Sphere;
	SetActorTickInterval(0.1);
	SetReplicates(true);
	bAlwaysRelevant = true;
}

void ARespawnPoint::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if(GetWorld()->IsServer())
	{
		ADamageableCharacter* Parent = Cast<ADamageableCharacter>(GetOwner());
		if(Parent)
		{
			OwnerClass = Parent->GetClass();
			OwnerIndex = reinterpret_cast<int64>(Parent);
		}
	}
}

void ARespawnPoint::BeginPlay()
{
	Super::BeginPlay();
	if (GetWorld()->IsServer())
	{
		ASMLGameState* GS = GetWorld()->GetGameState<ASMLGameState>();
        if(GS)
        {
        	GS->SpawnPoints.AddUnique(this);
        }
	}
}

void ARespawnPoint::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (GetWorld()->IsServer())
	{
		ASMLGameState* GS = GetWorld()->GetGameState<ASMLGameState>();
		if(GS)
		{
			GS->SpawnPoints.Remove(this);
		}
	}
	Super::EndPlay(EndPlayReason);
}

void ARespawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    //UE_LOG(LogSML, Log, TEXT("[ASpawnPoint::Tick]"));
	if(SpawnedActor)
	{
		if(GetDistanceTo(SpawnedActor) > FreeRadius)
        {
			UE_LOG(LogSML, Log, TEXT("[ASpawnPoint::Tick] Freed spawn point %s from actor %s at distance %f"), *GetName(), *SpawnedActor->GetName(), GetDistanceTo(SpawnedActor));
        	SpawnedActor = nullptr;
        	//SetActorTickEnabled(false);
        }
		else
		{
			//UE_LOG(LogSML, Log, TEXT("[ASpawnPoint::Tick] Spawn point %s locked by actor %s at distance %f"), *GetName(), *SpawnedActor->GetName(), GetDistanceTo(SpawnedActor));
		}
	}
	else
	{
		//UE_LOG(LogSML, Log, TEXT("[ASpawnPoint::Tick] Spawn point %s has invalid spawned actor"), *GetName());
	}
}

void ARespawnPoint::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ARespawnPoint, Team, COND_None);
	DOREPLIFETIME(ARespawnPoint, SpawnedActor);
	DOREPLIFETIME_CONDITION(ARespawnPoint, OwnerIndex, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(ARespawnPoint, OwnerClass, COND_InitialOnly);
}

bool ARespawnPoint::CanClassSpawnHere(const TSubclassOf<ADamageableCharacter> Class) const
{
	ADamageableCharacter* DefaultObject = Class.GetDefaultObject();
	return DefaultObject->CraftScale <= MaxSpawnableScale;
}

bool ARespawnPoint::CanSpawn(const TSubclassOf<ADamageableCharacter> Class) const
{
	return CanClassSpawnHere(Class) && SpawnedActor == nullptr;
}

void ARespawnPoint::NotifySpawn(ADamageableCharacter* InSpawnedActor)
{
	SpawnedActor = InSpawnedActor;
	OnSpawn.Broadcast();
	UE_LOG(LogSML, Log, TEXT("[ASpawnPoint::Tick] Locked spawn point %s with actor %s at distance %f, CanEverTick = %s"), *GetName(), *SpawnedActor->GetName(), GetDistanceTo(SpawnedActor),
		CanEverTick() ? TEXT("true") : TEXT("false"));
}
