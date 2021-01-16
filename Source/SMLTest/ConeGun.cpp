// Fill out your copyright notice in the Description page of Project Settings.


#include "ConeGun.h"

#include "Kismet/KismetMathLibrary.h"

AConeGun::AConeGun()
{
	MaximumAngleFromForward = 90.f;
}

bool AConeGun::IsInRotationRange(FVector WorldLocation)
{
	const FVector Vector = WorldLocation - GetActorLocation();
	const FVector Axis = Vector.GetSafeNormal();
	const float Scalar = Axis | GetActorForwardVector();
	return Scalar > cosf(FMath::DegreesToRadians<float>(MaximumAngleFromForward));
}

FQuat AConeGun::GetRotationInRange(FVector WorldLocation)
{
	const FVector vector = WorldLocation - GetActorLocation();
	const FVector axis = vector.GetSafeNormal();
	const FQuat IdealRotation = UKismetMathLibrary::MakeRotFromXZ(axis, GetActorUpVector()).Quaternion();
	if (IsInRotationRange(WorldLocation))
	{
		//is in allowed cone
		return IdealRotation;
	}
	else
	{
		const FQuat ForwardRotator = GetActorRotation().Quaternion();
		const float Angle = IdealRotation.AngularDistance(ForwardRotator);
		const FQuat TargetRotator = FQuat::Slerp(ForwardRotator, IdealRotation, FMath::DegreesToRadians<float>(MaximumAngleFromForward) / Angle /2.0);
		return TargetRotator;
	}
}

bool AConeGun::AimAt(const FVector WorldLocation)
{
	return Super::AimAt(WorldLocation);
}
