// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"


#include "Chaos/AABB.h"
#include "Chaos/AABB.h"

void AProjectile::InitializeProjectile(FVector Velocity, float InDamage, float Lifespan, TArray<AActor*>& InIgnoreList, bool bVisibleToOwner)
{
	ProjectileMeshComponent->SetPhysicsLinearVelocity(Velocity);
	SetLifeSpan(Lifespan);
	Damage = InDamage;
	IgnoreList = InIgnoreList;
	ProjectileMeshComponent->SetOwnerNoSee(!bVisibleToOwner);
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
	RootComponent = ProjectileMeshComponent;
	ProjectileMeshComponent->SetGenerateOverlapEvents(true);
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
	if (IgnoreList.Contains(OtherActor) || !GetWorld()->IsServer())
	{
		return;
	}
	OtherActor->TakeDamage(Damage, FDamageEvent(), GetInstigatorController(), this);
	Destroy();
}

void AProjectile::OnOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                            int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IgnoreList.Contains(OtherActor) || !GetWorld()->IsServer())
	{
		return;
	}
	OtherActor->TakeDamage(Damage, FDamageEvent(), GetInstigatorController(), this);
	Destroy();
}

