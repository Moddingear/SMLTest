// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageableActor.h"


#include "GeneratedCodeHelpers.h"
#include "SMLPlayerController.h"

// Sets default values
ADamageableActor::ADamageableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ADamageableActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	Health = MaxHealth;
}

// Called when the game starts or when spawned
void ADamageableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

bool ADamageableActor::CanTakeDamage()
{
	return MaxHealth > 0;
}

bool ADamageableActor::IsBroken()
{
	return CanTakeDamage() && Health <=0;
}

void ADamageableActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADamageableActor, Health);
}

// Called every frame
void ADamageableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float ADamageableActor::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
    AActor* DamageCauser)
{
	if (CanTakeDamage())
	{
		ASMLPlayerController* PCInstigator = Cast<ASMLPlayerController>(EventInstigator);
		ASMLPlayerController* PC = GetInstigatorController<ASMLPlayerController>();
		if(IsValid(PC) && IsValid(PCInstigator))
		{
			if (PC->Team == PCInstigator->Team)
			{
				return 0; //take no damage if same team
			}
		}
		const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
		Health -= ActualDamage;
		if (Health <= 0)
		{
			OnDeath();
		}
		return ActualDamage;
	}
	return 0;
}

void ADamageableActor::OnDeath_Implementation()
{
	
}