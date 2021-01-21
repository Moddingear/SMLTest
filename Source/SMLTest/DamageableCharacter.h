// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "GameFramework/Character.h"
#include "DamageableCharacter.generated.h"

class ASpawnPoint;
enum class ECraftScale : uint8;
UCLASS(BlueprintType, Blueprintable)
class SMLTEST_API ADamageableCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RespawnDelay;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ECraftScale CraftScale;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Replicated)
	int32 Team;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTexture2D* ClassIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText ClassName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText ClassDescription;
	
	// Sets default values for this character's properties
	ADamageableCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintNativeEvent)
	void InputAxisX(float Value);
	virtual void InputAxisX_Implementation(float Value);
	UFUNCTION(BlueprintNativeEvent)
	void InputAxisY(float Value);
	virtual void InputAxisY_Implementation(float Value);
	UFUNCTION(BlueprintNativeEvent)
	void InputAxisZ(float Value);
	virtual void InputAxisZ_Implementation(float Value);
	UFUNCTION(BlueprintNativeEvent)
	void InputMouseHorizontal(float Value);
	virtual void InputMouseHorizontal_Implementation(float Value);
	UFUNCTION(BlueprintNativeEvent)
	void InputMouseVertical(float Value);
	virtual void InputMouseVertical_Implementation(float Value);
	UFUNCTION(BlueprintNativeEvent)
    void InputRotation(float Value);
	virtual void InputRotation_Implementation(float Value);

	UFUNCTION(BlueprintNativeEvent)
	void InputFirePressed();
	virtual void InputFirePressed_Implementation();
	
	UFUNCTION(BlueprintNativeEvent)
    void InputFireReleased();
	virtual void InputFireReleased_Implementation();

	UFUNCTION(BlueprintNativeEvent)
	void OnDeath();
	virtual void OnDeath_Implementation();

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
};
