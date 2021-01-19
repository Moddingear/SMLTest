// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SMLPlayerController.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class SMLTEST_API ASMLPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void SetupInputComponent() override;
	
public:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	UFUNCTION(BlueprintNativeEvent)
	void InputMenuPressed();
	virtual void InputMenuPressed_Implementation();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OpenEscMenu();
	virtual void OpenEscMenu_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void CloseEscMenu();
	virtual void CloseEscMenu_Implementation();

	UFUNCTION(Server, BlueprintCallable, WithValidation, Reliable)
	void ForceRespawn();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OpenRespawnMenu();
	void OpenRespawnMenu_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void CloseRespawnMenu();
	virtual void CloseRespawnMenu_Implementation();

	UFUNCTION(Server, BlueprintCallable, Reliable, WithValidation)
	void AskRespawn(TSubclassOf<class ADamageableCharacter> Class);
	
};
