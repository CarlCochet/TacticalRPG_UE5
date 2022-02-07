// Fill out your copyright notice in the Description page of Project Settings.

#include "Grid.h"

#define GROUND_CHANNEL ECC_GameTraceChannel1
#define OBSTACLE_CHANNEL ECC_GameTraceChannel2

// Sets default values
AGrid::AGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGrid::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector AGrid::GetBottomLeft() const
{
	const USceneComponent* GridScene = Cast<USceneComponent>(GetComponentByClass(USceneComponent::StaticClass()));

	FVector RightVector{ GridScene->GetRightVector() };
	FVector ForwardVector{ GridScene->GetForwardVector() };

	RightVector *= GridSizeWorld.X;
	ForwardVector *= GridSizeWorld.Y;

	const FVector BottomLeft{ GridLocation - RightVector - ForwardVector };

	return BottomLeft;
}

void AGrid::DrawTiles()
{
	TArray<FVector> TilesVertices;
	TArray<int32> TilesTriangles;

	for (auto& Tile : Tiles)
	{
		switch (Tile.Value.GroundType)
		{
		case EGroundType::Ground:
			CreateTile(Tile.Value.GridIndex, TilesVertices, TilesTriangles);
			break;
		case EGroundType::Obstacle:
			CreateTile(Tile.Value.GridIndex, TilesVertices, TilesTriangles);
			break;
		case EGroundType::Hole:
			break;
		default:
			break;
		}
	}
}

// void AGrid::GenerateMapFromWorld()
// {
// 	Tiles.Empty();
// 	
// 	const USceneComponent* GridScene = Cast<USceneComponent>(GetComponentByClass(USceneComponent::StaticClass()));
// 	FVector BottomLeft{ GetBottomLeft() };
// 	
// 	BottomLeft.X += TileSize;
// 	BottomLeft.Y += TileSize;
// 	const float TileSizeReduction{ TileSize * 0.1f };
//
// 	const FVector GridRight{ GridScene->GetRightVector() };
// 	const FVector GridForward{ GridScene->GetForwardVector() };
//
// 	for (int32 x{ 0 }; x < NumberTiles.X; ++x)
// 	{
// 		for (int32 y{ 0 }; y < NumberTiles.Y; ++y)
// 		{
// 			FVector AddX{ x * TileSize * 2.f * GridRight };
// 			FVector AddY{ y * TileSize * 2.f * GridForward };
//
// 			FVector TilePosition{ BottomLeft + AddX + AddY };
//
// 			FHitResult GroundHit{};
// 			FHitResult ObstacleHit{};
// 			
// 			bool const bHitGround = GetWorld()->SweepSingleByChannel(
// 				GroundHit,
// 				TilePosition,
// 				TilePosition + FVector{ 0.f, 0.f, 0.01f },
// 				FQuat::Identity,
// 				GROUND_CHANNEL,
// 				FCollisionShape::MakeSphere(TileSize - TileSizeReduction),
// 				FCollisionQueryParams{FName{TEXT("SphereTraceSingle")}});
//
// 			bool const bHitObstacle = GetWorld()->SweepSingleByChannel(
// 				ObstacleHit,
// 				TilePosition,
// 				TilePosition + FVector{ 0.f, 0.f, 0.01f },
// 				FQuat::Identity,
// 				OBSTACLE_CHANNEL,
// 				FCollisionShape::MakeSphere(TileSize - TileSizeReduction),
// 				FCollisionQueryParams{FName{TEXT("SphereTraceSingle")}});
// 			
// 			FTile Tile;
// 			Tile.WorldLocation = TilePosition;
// 			Tile.GridIndex = FIntPoint{x, y};
// 			
// 			if (bHitObstacle)
// 				Tile.GroundType = EGroundType::Obstacle;
// 			else if (bHitGround)
// 				Tile.GroundType = EGroundType::Ground;
// 			else
// 				Tile.GroundType = EGroundType::Hole;
//
// 			Tiles.Add(FIntPoint{x, y}, Tile);
// 		}
// 	}
// }

void AGrid::GenerateMapFromWorld()
{
	Tiles.Empty();
	float TileSizeReduction { TileSize * 0.1f };

	for (int32 x{ 0 }; x < NumberTiles.X; ++x)
	{
		for (int32 y{ 0 }; y < NumberTiles.Y; ++y)
		{
			
			FVector2D TempPosition;
			TileToGridLocation(FIntPoint{x, y}, TempPosition, true);
			FVector TilePosition {TempPosition, 0.f};

			FHitResult GroundHit{};
			FHitResult ObstacleHit{};
			
			bool const bHitGround = GetWorld()->SweepSingleByChannel(
				GroundHit,
				TilePosition,
				TilePosition + FVector{ 0.f, 0.f, 0.01f },
				FQuat::Identity,
				GROUND_CHANNEL,
				FCollisionShape::MakeSphere(TileSize - TileSizeReduction),
				FCollisionQueryParams{FName{TEXT("SphereTraceSingle")}});

			bool const bHitObstacle = GetWorld()->SweepSingleByChannel(
				ObstacleHit,
				TilePosition,
				TilePosition + FVector{ 0.f, 0.f, 0.01f },
				FQuat::Identity,
				OBSTACLE_CHANNEL,
				FCollisionShape::MakeSphere(TileSize - TileSizeReduction),
				FCollisionQueryParams{FName{TEXT("SphereTraceSingle")}});
			
			FTile Tile;
			Tile.GridLocation = TilePosition;
			Tile.GridIndex = FIntPoint{x, y};
			
			if (bHitObstacle)
				Tile.GroundType = EGroundType::Obstacle;
			else if (bHitGround)
				Tile.GroundType = EGroundType::Ground;
			else
				Tile.GroundType = EGroundType::Hole;

			Tiles.Add(FIntPoint{x, y}, Tile);
		}
	}
}

void AGrid::CreateLinesGeometry(TArray<FVector>& Vertices, TArray<int32>& Triangles)
{
	// Shift the grid to be built around the center of the object
	CenterShift.X = GetGridHeight()/2.f;
	CenterShift.Y = GetGridWidth()/2.f;

	// Add the horizontal lines vertices & triangles
	for(int i{0} ; i <= NumberTiles.X ; ++i)
	{
		const float LineStart = TileSize * i;
		const float LineEnd = GetGridWidth();

		CreateLine(
			FVector{LineStart - CenterShift.X, -CenterShift.Y, 0.f},
			FVector{LineStart - CenterShift.X, LineEnd - CenterShift.Y, 0.f},
			LineThickness, Vertices, Triangles);
	}

	// Add the vertical lines vertices & triangles
	for(int i{0} ; i <= NumberTiles.Y ; ++i)
	{
		const float LineStart = TileSize * i;
		const float LineEnd = GetGridHeight();

		CreateLine(
			FVector{-CenterShift.X, LineStart - CenterShift.Y, 0.f},
			FVector{LineEnd - CenterShift.X, LineStart - CenterShift.Y, 0.f},
			LineThickness, Vertices, Triangles);
	}
}

void AGrid::CreateLine(const FVector Start, const FVector End, const float Thickness,
	TArray<FVector>& Vertices, TArray<int32>& Triangles)
{
	const float HalfThickness {Thickness * 0.5f};
	
	FVector LineDirection {(End - Start)};
	LineDirection.Normalize(0.0001f);
	const FVector ThicknessDirection {FVector::CrossProduct(LineDirection, FVector::UpVector)};
	
	const TArray<int32> TrianglesToAdd {
		// First Triangle vertices
		Vertices.Num() + 2,
		Vertices.Num() + 1,
		Vertices.Num() + 0,

		// Second Triangle vertices
		Vertices.Num() + 2,
		Vertices.Num() + 3,
		Vertices.Num() + 1
	};
	Triangles.Append(TrianglesToAdd);

	const TArray<FVector> VerticesToAdd {
		Start + ThicknessDirection * HalfThickness, // Vertex 0
		End + ThicknessDirection * HalfThickness,	// Vertex 1
		Start - ThicknessDirection * HalfThickness, // Vertex 2
		End - ThicknessDirection * HalfThickness,	// Vertex 3
	};
	Vertices.Append(VerticesToAdd);
}

void AGrid::CreateTile(const FIntPoint TileIndex, TArray<FVector>& Vertices, TArray<int32>& Triangles)
{
	FVector2D TileLocation;
	const bool ValidTile = TileToGridLocation(TileIndex, TileLocation, false);

	if (ValidTile)
	{
		CreateLine(
			FVector{TileLocation.X + LineThickness/2.f, TileLocation.Y + TileSize/2.f, 0.f},
			FVector{TileLocation.X + TileSize - LineThickness/2.f, TileLocation.Y + TileSize/2.f, 0.f},
			TileSize - LineThickness, Vertices, Triangles
		);
	}
}

float AGrid::GetGridWidth() const
{
	return NumberTiles.Y * TileSize;
}

float AGrid::GetGridHeight() const
{
	return NumberTiles.X * TileSize;
}

bool AGrid::LocationToTile(const FVector Location, FIntPoint& TileIndex) const
{
	if (Location.X < -CenterShift.X || Location.Y < -CenterShift.Y ||
		Location.X >= NumberTiles.X - CenterShift.X || Location.Y >= NumberTiles.Y - CenterShift.Y )
		return false;

	TileIndex.X = (Location.X + CenterShift.X) / TileSize;
	TileIndex.Y = (Location.Y + CenterShift.Y) / TileSize;
	
	return true;
}

bool AGrid::TileToGridLocation(const FIntPoint TileIndex, FVector2D& Location, const bool IsCenter) const
{
	if (TileIndex.X < 0 || TileIndex.Y < 0 ||
		TileIndex.X >= NumberTiles.X || TileIndex.Y >= NumberTiles.Y )
		return false;

	Location.X = TileIndex.X * TileSize - CenterShift.X;
	Location.Y = TileIndex.Y * TileSize - CenterShift.Y;
	
	if (IsCenter)
		Location = Location + TileSize/2.f;
	
	return true;
}

void AGrid::SetSelectedTile(FIntPoint TileIndex)
{
	
}