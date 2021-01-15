// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootableGun.h"



#include "DrawDebugHelpers.h"
#include "GeneratedCodeHelpers.h"
#include "SMLTest.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Math/UnrealMathUtility.h"

// Sets default values
AShootableGun::AShootableGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Firerate = 1.f;
	Damage = 1.f;
	NextTimeFireable = 0;
	bIsProjectile = false;
	ProjectileSpeed = 10000;
	MaxDistanceAfterPenetration = 0;
	MaximumRange = 100 * 1000;
	MaximumAngleFromForward = 90;
	bShootUnaligned = false;
	
	SetReplicates(true);
	SetReplicatingMovement(true);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BaseMeshHelper(TEXT("StaticMesh'/Game/drone/DroneGunBase.DroneGunBase'"));
	if(BaseMeshHelper.Succeeded())
	{
		BaseMesh = BaseMeshHelper.Object;
	}
	
	Base = CreateDefaultSubobject<UStaticMeshComponent>("Base");
	Base->SetStaticMesh(BaseMesh);
	RootComponent = Base;
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> GunMeshHelper(TEXT("StaticMesh'/Game/drone/DroneGun.DroneGun'"));
	if(GunMeshHelper.Succeeded())
	{
		GunMesh = GunMeshHelper.Object;
	}
	
	Gun = CreateDefaultSubobject<UStaticMeshComponent>("Gun");
	Gun->SetStaticMesh(GunMesh);
	Gun->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AShootableGun::BeginPlay()
{
	Super::BeginPlay();
	
}

void AShootableGun::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AShootableGun, IgnoredActors, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AShootableGun, Target, COND_SkipOwner);
}

// Called every frame
void AShootableGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetInstigatorController()) //if it's not controlled locally, then the controller will not exist
	{
		if(GetInstigatorController()->IsLocalController())
		{
			return;
		}
	}
	AimAt(Target);
}

void AShootableGun::SendTarget_Implementation(FVector NewTarget)
{
	Target = NewTarget;
}

bool AShootableGun::SendTarget_Validate(FVector NewTarget)
{
	return true;
}

bool AShootableGun::IsInFiringCone(FVector WorldLocation)
{
	FVector vector = WorldLocation - GetActorLocation();
	FVector axis = vector.GetSafeNormal();
	float scalar = axis | GetActorForwardVector();
	return scalar > cosf(FMath::DegreesToRadians<float>(MaximumAngleFromForward));
}

bool AShootableGun::IsRecharged()
{
	return UKismetSystemLibrary::GetGameTimeInSeconds(this) > NextTimeFireable;
}

bool AShootableGun::CanFireAt(FVector WorldLocation)
{
	return IsRecharged() && (IsInFiringCone(WorldLocation) || bShootUnaligned);
}

FQuat AShootableGun::GetRotationInCone(FVector WorldLocation)
{
	const FVector vector = WorldLocation - GetActorLocation();
	const FVector axis = vector.GetSafeNormal();
	const FQuat IdealRotation = UKismetMathLibrary::MakeRotFromXZ(axis, GetActorUpVector()).Quaternion();
	if (IsInFiringCone(WorldLocation))
	{
		//is in allowed cone
		return IdealRotation;
	}
	else
	{
		const FQuat ForwardRotator = GetActorRotation().Quaternion();
		const float angle = IdealRotation.AngularDistance(ForwardRotator);
		const FQuat TargetRotator = FQuat::Slerp(ForwardRotator, IdealRotation, FMath::DegreesToRadians<float>(MaximumAngleFromForward) / angle /2.0);
		return TargetRotator;
	}
}

void AShootableGun::RemoveIgnoredFromLineTrace(TArray<FHitResult>& HitResults)
{
	for (int i = HitResults.Num() - 1; i >= 0; --i)
	{
		FHitResult Hit = HitResults[i];
		if (IgnoredActors.Contains(Hit.GetActor()))
		{
			HitResults.RemoveAt(i);
		}
	}
}

bool AShootableGun::AimAt(FVector WorldLocation)
{
	Gun->SetWorldRotation(GetRotationInCone(WorldLocation));
	if(GetOwner())
	{
		SendTarget(WorldLocation);
	}
	return IsInFiringCone(WorldLocation);
}

void AShootableGun::Fire(FVector WorldLocation)
{
	//UE_LOG(LogSML, Log, TEXT("Fire command received"))
	if(bShootUnaligned || IsInFiringCone(WorldLocation))
	{
		const float TimeOfFiring = UKismetSystemLibrary::GetGameTimeInSeconds(this);
		//UE_LOG(LogSML, Log, TEXT("Is is firing cone; Time = %f, NextTimeFireable = %f"), TimeOfFiring, NextTimeFireable);
		if (TimeOfFiring > NextTimeFireable)
		{
			NextTimeFireable = TimeOfFiring + 1/Firerate;
			FQuat FiringRotation = GetRotationInCone(WorldLocation);
			FVector FiringStart = GetActorLocation() + FiringRotation.GetForwardVector() * DistanceFromCannon;
			if (bIsProjectile)
            {
            	//GetWorld()->SpawnActorDeferred<AActor>(ProjectileClass, FTransform(FiringStart), this, GetInstigator(), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
            }
            else
            {
            	FVector FiringEnd = FiringStart + FiringRotation.GetForwardVector() * MaximumRange;
            	TArray<FHitResult> Hits;
            	FCollisionQueryParams Params;
            	Params.bTraceComplex = false;
            	TArray<AActor*> ActorsHit;
            	GetWorld()->LineTraceMultiByProfile(Hits, FiringStart, FiringEnd, "Projectile");
                RemoveIgnoredFromLineTrace(Hits);
            	if(Hits.Num() > 0)
            	{
            		float StartDistance = Hits[0].Distance;
            		for (FHitResult Hit : Hits)
            		{
            			//UE_LOG(LogTemp, Log, TEXT("Hit %s, distance %f < MaxDistance %f"), *Hit.GetActor()->GetName(), Hit.Distance, StartDistance + MaxDistanceAfterPenetration)
            			if (Hit.Distance <= StartDistance + MaxDistanceAfterPenetration)
            			{
            				ActorsHit.AddUnique(Hit.GetActor());
            				FiringEnd = Hit.Location;
            				//DrawDebugPoint(GetWorld(), Hit.Location, 1, FColor::Green, false, 5);
            			}
            		}
            	}
            	//DrawDebugLine(GetWorld(), FiringStart, FiringEnd, FColor::Red, false, 5);
            	RegisterRaycastHit(ActorsHit, TimeOfFiring, FiringStart, FiringEnd);
            	PlayFiringAnimation(FiringStart, FiringEnd);
            }
		}
	}
}

void AShootableGun::RegisterRaycastHit_Implementation(const TArray<AActor*>& Hit, const float FiringTime, FVector StartLocation, FVector EndLocation)
{
	NextTimeFireable = FiringTime + 1/Firerate;
	AController* DamageInstigator = nullptr;
    if (GetInstigator())
    {
    	//UE_LOG(LogTemp, Log, TEXT("Instigator is valid"));
        DamageInstigator = GetInstigator()->GetController();
    }
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Instigator is invalid"));
	}
	for (int i = 0; i < Hit.Num(); ++i)
	{
		//UE_LOG(LogTemp, Log, TEXT("%s was hit"), *Hit[i]->GetName());
		Hit[i]->TakeDamage(Damage, FDamageEvent(), DamageInstigator, this);
	}
	PlayFiringAnimation(StartLocation, EndLocation);
}

bool AShootableGun::RegisterRaycastHit_Validate(const TArray<AActor*>& Hit, const float FiringTime, FVector StartLocation, FVector EndLocation)
{
	if (!GetWorld()->IsServer())
	{
		if (FiringTime > UKismetSystemLibrary::GetGameTimeInSeconds(this)) //check if time is valid
		{
			return false;
		}
		if (bIsProjectile) //check if it's raycast
		{
			return false;
		}
		if (MaxDistanceAfterPenetration == 0 && Hit.Num() > 1) //check there's only one hit if no penetration set
		{
			return false;
		}
		if (FiringTime + 1.0/Firerate/20.f < NextTimeFireable) //check if it can fire
		{
			return false;
		}
		for (AActor* Actor : Hit)
		{
			if (Actor->GetDistanceTo(this) > MaximumRange * 1.1)
			{
				return false;
			}
			if(IgnoredActors.Contains(Actor))
			{
				return false;
			}
		}
	}
	return true;
}

void AShootableGun::PlayFiringAnimationServer_Implementation(FVector StartLocation, FVector EndLocation)
{
	if(!GetInstigator()->IsLocallyControlled())
	{
		PlayFiringAnimation(StartLocation, EndLocation);
	}
}

void AShootableGun::PlayFiringAnimation_Implementation(FVector StartLocation, FVector EndLocation)
{
}