// Fill out your copyright notice in the Description page of Project Settings.


#include "SixDOFController.h"

ASixDOFController::ASixDOFController(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

}

void ASixDOFController::SetupInputComponent()
{
	Super::SetupInputComponent();
	FInputVectorAxisBinding Translation;

	//InputComponent->AddActionBinding()
}
