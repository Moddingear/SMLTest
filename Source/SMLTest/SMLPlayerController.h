// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SMLPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SMLTEST_API ASMLPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void SetupInputComponent() override;
	
public:
	UFUNCTION(BlueprintNativeEvent)
	void InputMenuPressed();
	virtual void InputMenuPressed_Implementation();

	UFUNCTION(BlueprintNativeEvent)
	void OpenRespawnMenu();
	void OpenRespawnMenu_Implementation();
};
