// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SMLTEST_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> IgnoreList;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage;

	void InitializeProjectile(FVector Velocity, float InDamage, float Lifespan, TArray<AActor*>& InIgnoreList, bool bVisibleToOwner);

	UPROPERTY(EditAnywhere)
	UStaticMesh* ProjectileMesh;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UStaticMeshComponent* ProjectileMeshComponent;
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	virtual void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
};
