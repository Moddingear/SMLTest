// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SMLPlayerController.generated.h"

class ARespawnPoint;
class ADamageableCharacter;
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

	UFUNCTION(Client, Reliable)
	virtual void OnPossessOwner(APawn* InPawn);
	
	virtual void OnUnPossess() override;

	UPROPERTY(BlueprintReadOnly)
	int32 LastTeam;

	UPROPERTY(BlueprintReadOnly)
	TSubclassOf<ADamageableCharacter> LastClass;

	UPROPERTY(BlueprintReadOnly)
	ARespawnPoint* LastSpawnPoint;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float DeathTime;
	
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

	UFUNCTION(BlueprintPure)
	float GetTimeUntilRespawn(TSubclassOf<class ADamageableCharacter> Class);

	UFUNCTION(BlueprintCallable)
	void RespawnAndRemember(TSubclassOf<class ADamageableCharacter> Class, int32 Team, ARespawnPoint* SpawnPoint);

	UFUNCTION(Server, Reliable, WithValidation)
	void AskRespawn(TSubclassOf<class ADamageableCharacter> Class, int32 Team, ARespawnPoint* SpawnPoint);
	
};
