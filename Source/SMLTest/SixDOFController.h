// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SixDOFController.generated.h"

/**
 * 
 */
UCLASS()
class SMLTEST_API ASixDOFController : public APlayerController
{
	GENERATED_BODY()

public :
	ASixDOFController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	virtual void SetupInputComponent() override;
};
