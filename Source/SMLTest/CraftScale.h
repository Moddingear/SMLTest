// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ECraftScale : uint8
{
	None = 0,
    Small = 1,
    Medium = 2,
    Large = 3,
    Max = 4,
};