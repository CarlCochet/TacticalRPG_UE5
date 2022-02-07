// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridStructs.generated.h"

UENUM()
enum EGroundType
{
	Hole		UMETA(DisplayName = "Hole"),
	Ground		UMETA(DisplayName = "Ground"),
	Obstacle	UMETA(DisplayName = "Obstacle"),
};

USTRUCT(BlueprintType)
struct FTile
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TEnumAsByte<EGroundType> GroundType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector GridLocation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FIntPoint GridIndex;
};
