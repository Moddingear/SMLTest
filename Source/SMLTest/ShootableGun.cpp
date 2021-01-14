// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootableGun.h"



#include "GeneratedCodeHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Math/UnrealMathUtility.h"

// Sets default values
AShootableGun::AShootableGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
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
	DOREPLIFETIME_CONDITION(AShootableGun, IgnoredActors, COND_OwnerOnly);
}

// Called every frame
void AShootableGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AShootableGun::IsInFiringCone(FVector WorldLocation)
{
	FVector vector = WorldLocation - GetActorLocation();
	FVector axis = vector.GetSafeNormal();
	float scalar = axis | GetActorForwardVector();
	return scalar > cosf(FMath::DegreesToRadians<float>(MaximumAngleFromForward));
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

bool AShootableGun::AimAt(FVector WorldLocation)
{
	
	Gun->SetWorldRotation(GetRotationInCone(WorldLocation));
	return IsInFiringCone(WorldLocation);
}

void AShootableGun::Fire(FVector WorldLocation)
{
	if(bShootUnaligned || IsInFiringCone(WorldLocation))
	{
		const float TimeOfFiring = UKismetSystemLibrary::GetGameTimeInSeconds(this);
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
            	Params.AddIgnoredActors(IgnoredActors);
            	TArray<AActor*> ActorsHit;
            	if(GetWorld()->LineTraceMultiByProfile(Hits, FiringStart, FiringEnd, "Projectile"))
            	{
            		float StartDistance = Hits[0].Distance;
            		
            		for (FHitResult Hit : Hits)
            		{
            			if (Hit.Distance <= StartDistance + MaxDistanceAfterPenetration)
            			{
            				ActorsHit.AddUnique(Hit.GetActor());
            				FiringEnd = Hit.Location;
            			}
            		}
            	}
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
    	UE_LOG(LogTemp, Log, TEXT("Instigator is valid"));
        DamageInstigator = GetInstigator()->GetController();
    }
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Instigator is invalid"));
	}
	for (int i = 0; i < Hit.Num(); ++i)
	{
		UE_LOG(LogTemp, Log, TEXT("%s was hit"), *Hit[i]->GetName());
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