// Fill out your copyright notice in the Description page of Project Settings.


#include "CraftScale.h"

#define LOCTEXT_NAMESPACE "CraftScaleEnum"

FText UCraftScaleLibrary::GetCraftScaleName(ECraftScale InScale)
{
	switch (InScale) {
		case ECraftScale::None: return LOCTEXT("ErrNone", "ErrNone");
		case ECraftScale::Small: return LOCTEXT("Small", "Small");
		case ECraftScale::Medium: return LOCTEXT("Medium", "Medium");
		case ECraftScale::Large: return LOCTEXT("Large", "Large");
		case ECraftScale::Max: return LOCTEXT("ErrMax", "ErrMax");
		default: return LOCTEXT("ErrOutOfRange", "ErrOutOfRange");
	}
}

#undef LOCTEXT_NAMESPACE
