// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnPoint.h"

#include "CraftScale.h"
#include "DamageableCharacter.h"
#include "GeneratedCodeHelpers.h"
#include "SMLGameState.h"
#include "SMLTest.h"
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
	bAlwaysRelevant = true;
}

void ASpawnPoint::OnConstruction(const FTransform& Transform)
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

void ASpawnPoint::BeginPlay()
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
	//UE_LOG(LogSML, Log, TEXT("Class : %s, Index : %ld"), *OwnerClass->GetName(), OwnerIndex);
}

void ASpawnPoint::EndPlay(const EEndPlayReason::Type EndPlayReason)
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
	DOREPLIFETIME_CONDITION(ASpawnPoint, Team, COND_None);
	DOREPLIFETIME(ASpawnPoint, SpawnedActor);
	DOREPLIFETIME_CONDITION(ASpawnPoint, OwnerIndex, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(ASpawnPoint, OwnerClass, COND_InitialOnly);
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
