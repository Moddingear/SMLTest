// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CraftScale.generated.h"
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

UCLASS()
class SMLTEST_API UCraftScaleLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	static FText GetCraftScaleName(ECraftScale InScale);
};
