// Fill out your copyright notice in the Description page of Project Settings.


#include "ThetaPhiGun.h"


#include "ToolContextInterfaces.h"
#include "Kismet/KismetMathLibrary.h"


FVector2D AThetaPhiGun::GetSphericalCoordinates(const FVector WorldLocation) const
{
	const FVector Axis = WorldLocation - GetActorLocation();
	const FVector AxisLocal = GetActorTransform().InverseTransformVectorNoScale(Axis);
	const FVector AxisNormalised = AxisLocal.GetSafeNormal();
	const FVector2D Spherical = AxisNormalised.UnitCartesianToSpherical();
	return Spherical;
}

bool AThetaPhiGun::IsInRotationRange(const FVector WorldLocation)
{
	
	const FVector2D SphericalDeg = GetSphericalCoordinates(WorldLocation) * 180.0/PI;
	return SphericalDeg.X < ThetaMax && SphericalDeg.X > ThetaMin && abs(SphericalDeg.Y) < PhiRange;
}

FQuat AThetaPhiGun::GetRotationInRange(const FVector WorldLocation)
{
	FVector Forward = WorldLocation - GetActorLocation();
	if(!IsInRotationRange(WorldLocation))
	{
		FVector2D Spherical = GetSphericalCoordinates(WorldLocation);
        Spherical.X = FMath::Clamp(Spherical.X, ThetaMin * PI/180.f, ThetaMax * PI/180.f);
        Spherical.Y = FMath::Clamp(Spherical.Y, -PhiRange * PI/180.f, PhiRange * PI/180.f);
		const FVector ForwardLocal = UKismetMathLibrary::Spherical2DToUnitCartesian(Spherical);
		Forward = GetActorTransform().TransformVectorNoScale(ForwardLocal);
	}
	const FQuat Rotation = UKismetMathLibrary::MakeRotFromXZ(Forward, GetActorUpVector()).Quaternion();
	return Rotation;
}

bool AThetaPhiGun::AimAt(FVector WorldLocation)
{
	return Super::AimAt(WorldLocation);
	//todo add other rotating bit only in phi
}
