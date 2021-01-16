// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShootableGun.generated.h"

UCLASS()
class SMLTEST_API AShootableGun : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FireRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float NextTimeFireable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DistanceFromCannon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsProjectile;
	//cm/s, only if projectile
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ProjectileSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxDistanceAfterPenetration;
	//Used for length of raycast and lifetime of projectile
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaximumRange;
	//should this gun shoot if the reticule is out of the area ?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShootUnaligned;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	TArray<AActor*> IgnoredActors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	FVector Target; //Used to replicate gun rotation

	UPROPERTY(EditAnywhere)
	UStaticMesh* BaseMesh;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UStaticMeshComponent* Base;
	
	UPROPERTY(EditAnywhere)
	UStaticMesh* GunMesh;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UStaticMeshComponent* Gun;

	TSubclassOf<AActor> ProjectileClass;
	// Sets default values for this actor's properties
	AShootableGun();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Server, Unreliable, WithValidation)
	void SendTarget(FVector NewTarget); //Cosmetic, updates the aimed location for other players

	UFUNCTION(BlueprintPure)
	virtual bool IsInRotationRange(FVector WorldLocation);

	UFUNCTION(BlueprintPure)
	virtual bool IsRecharged();

	UFUNCTION(BlueprintPure)
	virtual bool CanFireAt(FVector WorldLocation);

	UFUNCTION(BlueprintPure)
	virtual FQuat GetRotationInRange(FVector WorldLocation); //world rotation

	UFUNCTION(BlueprintCallable)
	void RemoveIgnoredFromLineTrace(UPARAM(ref) TArray<FHitResult> &HitResults);
	
	UFUNCTION(BlueprintCallable)
	virtual bool AimAt(FVector WorldLocation);

	UFUNCTION(BlueprintCallable)
	void Fire(FVector WorldLocation);

	UFUNCTION(Server, Reliable, WithValidation)
	void RegisterLineTraceHit(const TArray<AActor*>& Hit, const float FiringTime, FVector StartLocation, FVector EndLocation);

	UFUNCTION(NetMulticast, Unreliable)
	void PlayFiringAnimationServer(FVector StartLocation, FVector EndLocation);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void PlayFiringAnimation(FVector StartLocation, FVector EndLocation);
	virtual void PlayFiringAnimation_Implementation(FVector StartLocation, FVector EndLocation);
};
