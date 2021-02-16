// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageableCharacter.h"

#include "CraftScale.h"
#include "GeneratedCodeHelpers.h"
#include "SMLPlayerController.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ADamageableCharacter::ADamageableCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MaxHealth = 1.0f;
    Health = MaxHealth;
	RespawnDelay = 1.f;
	CraftScale = ECraftScale::None;
	SetReplicates(true);
	SetReplicateMovement(true);
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
}

// Called when the game starts or when spawned
void ADamageableCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADamageableCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ADamageableCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveX", this, &ADamageableCharacter::InputAxisX);
	PlayerInputComponent->BindAxis("MoveY", this, &ADamageableCharacter::InputAxisY);
	PlayerInputComponent->BindAxis("MoveZ", this, &ADamageableCharacter::InputAxisZ);
	PlayerInputComponent->BindAxis("MouseLeftRight", this, &ADamageableCharacter::InputMouseHorizontal);
	PlayerInputComponent->BindAxis("MouseUpDown", this, &ADamageableCharacter::InputMouseVertical);
	PlayerInputComponent->BindAxis("Rotation", this, &ADamageableCharacter::InputRotation);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ADamageableCharacter::InputFirePressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ADamageableCharacter::InputFireReleased);
}

void ADamageableCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADamageableCharacter, Health);
	DOREPLIFETIME_CONDITION(ADamageableCharacter, Team, COND_InitialOnly);
}

void ADamageableCharacter::InputAxisX_Implementation(float Value)
{
}

void ADamageableCharacter::InputAxisZ_Implementation(float Value)
{
}

void ADamageableCharacter::InputAxisY_Implementation(float Value)
{
}

void ADamageableCharacter::InputMouseHorizontal_Implementation(float Value)
{
}

void ADamageableCharacter::InputMouseVertical_Implementation(float Value)
{
}

void ADamageableCharacter::InputRotation_Implementation(float Value)
{
}

void ADamageableCharacter::InputFirePressed_Implementation()
{
}

void ADamageableCharacter::InputFireReleased_Implementation()
{
}

void ADamageableCharacter::OnDeath_Implementation()
{
}

float ADamageableCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
                                       AController* EventInstigator, AActor* DamageCauser)
{
	ASMLPlayerController* PCInstigator = Cast<ASMLPlayerController>(EventInstigator);
	ASMLPlayerController* PC = GetController<ASMLPlayerController>();
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

bool ADamageableCharacter::ShouldDestroyOnRespawn()
{
		return true;
}

