// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatingCastle.generated.h"

/**
 *  A floating castle assembled from basic geometry primitives.
 *  Hovers above the arena with a gentle bobbing animation.
 */
UCLASS()
class AFloatingCastle : public AActor
{
	GENERATED_BODY()

public:

	AFloatingCastle();

	virtual void Tick(float DeltaTime) override;

protected:

	virtual void BeginPlay() override;

	/** Root scene component for the whole castle */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* CastleRoot;

	/** Amplitude of the floating bob in units */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating")
	float BobAmplitude = 50.0f;

	/** Speed of the floating bob */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating")
	float BobSpeed = 1.0f;

	/** Slow rotation speed in degrees per second */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating")
	float RotationSpeed = 3.0f;

	/** Scale multiplier for the entire castle */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Castle")
	float CastleScale = 1.0f;

private:

	/** Tracks elapsed time for the bob animation */
	float RunningTime = 0.0f;

	/** The starting Z position, captured at BeginPlay */
	float BaseZ = 0.0f;

	/** Helper to create a cube mesh component */
	UStaticMeshComponent* CreateCubePart(const FString& Name, const FVector& Location, const FVector& Scale);

	/** Helper to create a cylinder mesh component */
	UStaticMeshComponent* CreateCylinderPart(const FString& Name, const FVector& Location, const FVector& Scale);

	/** Builds the castle geometry */
	void BuildCastle();

	/** Builds a single corner tower */
	void BuildTower(const FString& BaseName, const FVector& Location);

	/** Builds a wall segment between two points */
	void BuildWall(const FString& BaseName, const FVector& Start, const FVector& End);

	/** Builds battlements (merlons) along a wall */
	void BuildBattlements(const FString& BaseName, const FVector& Start, const FVector& End, int32 Count);
};
