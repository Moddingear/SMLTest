// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShootableGun.h"
#include "ThetaPhiGun.generated.h"

/**
 * 
 */
UCLASS()
class SMLTEST_API AThetaPhiGun : public AShootableGun
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float PhiRange; //How far left/right can the gun go [0, 180]
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ThetaMin; //How high can the gun go [0, 180]
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ThetaMax; //How low can the gun go [0, 180]

	FVector2D GetSphericalCoordinates(FVector WorldLocation) const;
	
	virtual bool IsInRotationRange(FVector WorldLocation) override;

	virtual FQuat GetRotationInRange(FVector WorldLocation) override;

	virtual bool AimAt(FVector WorldLocation) override;
};
