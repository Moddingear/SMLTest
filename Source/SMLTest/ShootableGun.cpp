// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootableGun.h"

#include "DrawDebugHelpers.h"
#include "GeneratedCodeHelpers.h"
#include "Projectile.h"
#include "SMLTest.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AShootableGun::AShootableGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	FireRate = 1.f;
	Damage = 1.f;
	NextTimeFireable = 0;
	bIsProjectile = false;
	ProjectileSpeed = 10000;
	MaxDistanceAfterPenetration = 0;
	MaximumRange = 100 * 1000;
	bShootUnaligned = false;
	MaxHealth = 0.f;

	ProjectileClass = AProjectile::StaticClass();
	
	SetReplicates(true);
	SetReplicatingMovement(true);
	//bNetUseOwnerRelevancy = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BaseMeshHelper(TEXT("StaticMesh'/Game/drone/MeshDroneGunBase.MeshDroneGunBase'"));
	if(BaseMeshHelper.Succeeded())
	{
		BaseMesh = BaseMeshHelper.Object;
	}
	
	Base = CreateDefaultSubobject<UStaticMeshComponent>("Base");
	Base->SetStaticMesh(BaseMesh);
	RootComponent = Base;
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> GunMeshHelper(TEXT("StaticMesh'/Game/drone/MeshDroneGun.MeshDroneGun'"));
	if(GunMeshHelper.Succeeded())
	{
		GunMesh = GunMeshHelper.Object;
	}
	
	Gun = CreateDefaultSubobject<UStaticMeshComponent>("Gun");
	Gun->SetStaticMesh(GunMesh);
	Gun->SetupAttachment(RootComponent);
}

void AShootableGun::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (!CanTakeDamage())
	{
		Gun->SetCollisionProfileName("NoCollision");
		Base->SetCollisionProfileName("NoCollision");
	}
	else
	{
		Gun->SetCollisionProfileName("GunDestructible");
		Base->SetCollisionProfileName("GunDestructible");
	}
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
	if (!IsBroken())
	{
		AimAt(Target);
	}
}

void AShootableGun::SendTarget_Implementation(const FVector NewTarget)
{
	Target = NewTarget;
}

bool AShootableGun::SendTarget_Validate(FVector NewTarget)
{
	return true;
}

bool AShootableGun::IsInRotationRange(const FVector WorldLocation)
{
	return false;
}

bool AShootableGun::IsRecharged()
{
	return UKismetSystemLibrary::GetGameTimeInSeconds(this) > NextTimeFireable;
}

bool AShootableGun::CanFireAt(FVector WorldLocation)
{
	return IsRecharged() && (IsInRotationRange(WorldLocation) || bShootUnaligned)  && !IsBroken();
}

FQuat AShootableGun::GetRotationInRange(FVector WorldLocation)
{
	return GetActorQuat();
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
	if (!IsBroken())
	{
		Gun->SetWorldRotation(GetRotationInRange(WorldLocation));
        if(GetOwner())
        {
        	SendTarget(WorldLocation);
        }
        return IsInRotationRange(WorldLocation);
	}
	return false;
}

void AShootableGun::Fire(FVector WorldLocation)
{
	//UE_LOG(LogSML, Log, TEXT("Fire command received"))
	if((bShootUnaligned || IsInRotationRange(WorldLocation)) && !IsBroken())
	{
		const float TimeOfFiring = UKismetSystemLibrary::GetGameTimeInSeconds(this);
		//UE_LOG(LogSML, Log, TEXT("Is is firing cone; Time = %f, NextTimeFireable = %f"), TimeOfFiring, NextTimeFireable);
		if (TimeOfFiring > NextTimeFireable)
		{
			NextTimeFireable = TimeOfFiring + 1/FireRate;
			FQuat FiringRotation = GetRotationInRange(WorldLocation);
			FVector FiringStart = GetActorLocation() + FiringRotation.GetForwardVector() * DistanceFromCannon;
			if (bIsProjectile)
            {
            	//AProjectile* Projectile = SpawnProjectile(FTransform(FiringStart), FiringRotation.GetForwardVector()*ProjectileSpeed + GetVelocity(), GetWorld()->IsServer());
                if (!GetWorld()->IsServer())
                {
	                RegisterProjectile(FiringRotation.GetForwardVector(), TimeOfFiring);
                }
            }
            else
            {
            	FVector FiringEnd = FiringStart + FiringRotation.GetForwardVector() * MaximumRange;
            	TArray<FHitResult> Hits;
            	FCollisionQueryParams Params;
            	Params.bTraceComplex = false;
            	TArray<AActor*> ActorsHit;
            	GetWorld()->LineTraceMultiByProfile(Hits, FiringStart, FiringEnd, "ProjectileTrace");
                RemoveIgnoredFromLineTrace(Hits);
            	if(Hits.Num() > 0)
            	{
            		float StartDistance = Hits[0].Distance;
            		FiringEnd = Hits[0].Location + MaxDistanceAfterPenetration * FiringRotation.GetForwardVector();
            		for (FHitResult Hit : Hits)
            		{
            			//UE_LOG(LogTemp, Log, TEXT("Hit %s, distance %f < MaxDistance %f"), *Hit.GetActor()->GetName(), Hit.Distance, StartDistance + MaxDistanceAfterPenetration)
            			if (Hit.Distance <= StartDistance + MaxDistanceAfterPenetration)
            			{
            				ActorsHit.AddUnique(Hit.GetActor());
            				//DrawDebugPoint(GetWorld(), Hit.Location, 1, FColor::Green, false, 5);
            			}
            		}
            	}
            	//DrawDebugLine(GetWorld(), FiringStart, FiringEnd, FColor::Red, false, 5);
            	RegisterLineTraceHit(ActorsHit, TimeOfFiring, FiringStart, FiringEnd);
            	PlayFiringAnimation(FiringStart, FiringEnd);
            }
		}
	}
}

AProjectile* AShootableGun::SpawnProjectile(FTransform SpawnTransform, FVector InitialVelocity, bool bVisibleToOwner)
{
	UE_LOG(LogSML, Log, TEXT("Spawning projectile, bVisibleToOwner = %d"), bVisibleToOwner);
	AProjectile* Projectile = GetWorld()->SpawnActorDeferred<AProjectile>(ProjectileClass, SpawnTransform, this, GetInstigator(), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	Projectile->InitializeProjectile(InitialVelocity, Damage, MaximumRange/ProjectileSpeed, IgnoredActors, bVisibleToOwner);
	Projectile->FinishSpawning(SpawnTransform);
	//Projectile->InitializeProjectile(InitialVelocity, Damage, MaximumRange/ProjectileSpeed, IgnoredActors, GetWorld()->IsServer());
	return Projectile;
}

void AShootableGun::RegisterProjectile_Implementation(const FVector Axis, const float FiringTime)
{
	if (!IsBroken())
	{
		const FVector FiringStart = GetActorLocation() + Axis * DistanceFromCannon;
        SpawnProjectile(FTransform(FiringStart), Axis * ProjectileSpeed + GetVelocity(), true);
	}
}

bool AShootableGun::RegisterProjectile_Validate(const FVector Axis, const float FiringTime)
{
	if (!GetWorld()->IsServer())
	{
		if (FiringTime > UKismetSystemLibrary::GetGameTimeInSeconds(this)) //check if time is valid
		{
		return false;
		}
		if (!bIsProjectile) //check if it's raycast
		{
		return false;
		}
		if (FiringTime + 1.0/FireRate/20.f < NextTimeFireable) //check if it can fire
		{
		return false;
		}
	}
	return true;
}

void AShootableGun::RegisterLineTraceHit_Implementation(const TArray<AActor*>& Hit, const float FiringTime, FVector StartLocation, FVector EndLocation)
{
	if (!IsBroken())
	{
		NextTimeFireable = FiringTime + 1/FireRate;
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
        PlayFiringAnimationServer(StartLocation, EndLocation);
	}
}

bool AShootableGun::RegisterLineTraceHit_Validate(const TArray<AActor*>& Hit, const float FiringTime, FVector StartLocation, FVector EndLocation)
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
		if (FiringTime + 1.0/FireRate/20.f < NextTimeFireable) //check if it can fire
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
