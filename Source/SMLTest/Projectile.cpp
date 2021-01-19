// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"



#include "SMLTest.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"

void AProjectile::InitializeProjectile(FVector Velocity, float InDamage, float Lifespan, TArray<AActor*>& InIgnoreList, bool bVisibleToOwner)
{
	SetLifeSpan(Lifespan);
	Damage = InDamage;
	IgnoreList = InIgnoreList;
	ProjectileMeshComponent->SetOwnerNoSee(!bVisibleToOwner);
	ProjectileMeshComponent->SetPhysicsLinearVelocity(Velocity);
	for (AActor* Actor : IgnoreList)
	{
		ProjectileMeshComponent->IgnoreActorWhenMoving(Actor, true);
	}
	
}

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
	SetReplicatingMovement(true);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> ProjectileMeshHelper(TEXT(""));
	if(ProjectileMeshHelper.Succeeded())
	{
		ProjectileMesh = ProjectileMeshHelper.Object;
	}
	
	ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("ProjectileMesh");
	ProjectileMeshComponent->SetStaticMesh(ProjectileMesh);
	ProjectileMeshComponent->SetNotifyRigidBodyCollision(true); //needed for hit events (awful name)
	ProjectileMeshComponent->SetGenerateOverlapEvents(true);
	ProjectileMeshComponent->SetCollisionProfileName(FName("ProjectilePhysical"));
	ProjectileMeshComponent->SetSimulatePhysics(true);
	ProjectileMeshComponent->SetUseCCD(true);
	RootComponent = ProjectileMeshComponent;
	ProjectileMeshComponent->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
    ProjectileMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnOverlap);
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (!IgnoreList.Contains(OtherActor))
	{
		if (GetWorld()->IsServer())
		{
			//UE_LOG(LogSML, Log, TEXT("Hit registered between %s and %s, Instigator is %s"), *this->GetName(), *OtherActor->GetName(), *GetInstigatorController()->GetName());
			OtherActor->TakeDamage(Damage, FDamageEvent(), GetInstigatorController(), this);
		}
		Destroy();
	}
	else
	{
		if (GetWorld()->IsServer())
		{
			//UE_LOG(LogSML, Log, TEXT("Hit registered but ignored between %s and %s, Instigator is %s"), *this->GetName(), *OtherActor->GetName(), *GetInstigatorController()->GetName());
		}
	}
}

void AProjectile::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (!IgnoreList.Contains(OtherActor))
	{
		if (GetWorld()->IsServer())
		{
			//UE_LOG(LogSML, Log, TEXT("Overlap registered between %s and %s, Instigator is %s"), *this->GetName(), *OtherActor->GetName(), *GetInstigatorController()->GetName());
            OtherActor->TakeDamage(Damage, FDamageEvent(), GetInstigatorController(), this);
		}
        Destroy();
	}
	else
	{
		if (GetWorld()->IsServer())
		{
			//UE_LOG(LogSML, Log, TEXT("Overlap registered but ignored between %s and %s, Instigator is %s"), *this->GetName(), *OtherActor->GetName(), *GetInstigatorController()->GetName());
		}
	}
}

