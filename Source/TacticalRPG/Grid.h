// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DrawDebugHelpers.h"
#include "Obstacle.h"
#include "GridStructs.h"
#include "GameFramework/Actor.h"
#include "Grid.generated.h"

UCLASS()
class TACTICALRPG_API AGrid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrid();

	UPROPERTY(BlueprintReadWrite, Category = Grid)
	FVector GridLocation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Grid)
	FVector2D GridSizeWorld {
		500.f, 500.f
	};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Constants)
	float TileSize{50};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Grid)
	TMap<FIntPoint, FTile> Tiles;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Constants)
	FIntPoint NumberTiles{40, 40};

	UPROPERTY(BlueprintReadWrite, Category = Constants)
	FVector2D CenterShift;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Constants)
	float LineThickness{3};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Constants)
	float LineOpacity{0.5f};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Constants)
	float SelectionOpacity{0.5f};
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Constants)
	FLinearColor LineColor{FColor::Black};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Constants)
	FLinearColor SelectionColor{FColor::Green};

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
		void CreateLinesGeometry(UPARAM(ref) TArray<FVector>& Vertices, UPARAM(ref) TArray<int32>& Triangles);
	
	UFUNCTION(BlueprintCallable)
		void CreateLine(const FVector Start, const FVector End, const float Thickness,
			UPARAM(ref) TArray<FVector>& Vertices, UPARAM(ref) TArray<int32>& Triangles);
	
	UFUNCTION(BlueprintCallable)
		float GetGridWidth() const;

	UFUNCTION(BlueprintCallable)
		float GetGridHeight() const;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		FVector GetBottomLeft() const;

	UFUNCTION(BlueprintCallable)
		void DrawTiles();

	UFUNCTION(BlueprintCallable)
		void GenerateMapFromWorld();

	UFUNCTION(BlueprintCallable)
		bool LocationToTile(const FVector Location, FIntPoint& TileIndex) const;

	UFUNCTION(BlueprintCallable)
		bool TileToGridLocation(const FIntPoint TileIndex, FVector2D& Location, const bool IsCenter) const;

	UFUNCTION(BlueprintCallable)
		void SetSelectedTile(FIntPoint TileIndex);

	UFUNCTION(BlueprintCallable)
		void CreateTile(const FIntPoint TileIndex, UPARAM(ref) TArray<FVector>& Vertices, UPARAM(ref) TArray<int32>& Triangles);
	
};
