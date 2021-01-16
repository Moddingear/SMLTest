// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShootableGun.h"
#include "ConeGun.generated.h"

/**
 * 
 */
UCLASS()
class SMLTEST_API AConeGun : public AShootableGun
{
	GENERATED_BODY()
	
public:
	AConeGun();


	//goes from 0 (only straight forward) to 180 (anywhere around)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaximumAngleFromForward;
	
	virtual bool IsInRotationRange(FVector WorldLocation) override;

	virtual FQuat GetRotationInRange(FVector WorldLocation) override;

	virtual bool AimAt(FVector WorldLocation) override;
	
};
