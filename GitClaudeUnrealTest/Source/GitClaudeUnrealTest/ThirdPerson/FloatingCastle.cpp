// Copyright Epic Games, Inc. All Rights Reserved.

#include "FloatingCastle.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"

AFloatingCastle::AFloatingCastle()
{
	PrimaryActorTick.bCanEverTick = true;

	CastleRoot = CreateDefaultSubobject<USceneComponent>(TEXT("CastleRoot"));
	RootComponent = CastleRoot;

	BuildCastle();
}

void AFloatingCastle::BeginPlay()
{
	Super::BeginPlay();
	BaseZ = GetActorLocation().Z;

	// Rotate the castle 45 degrees to the right (clockwise yaw, viewed from above)
	AddActorWorldRotation(FRotator(0.0f, 45.0f, 0.0f));
}

void AFloatingCastle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;

	// Gentle vertical bob
	FVector Location = GetActorLocation();
	Location.Z = BaseZ + FMath::Sin(RunningTime * BobSpeed) * BobAmplitude;
	SetActorLocation(Location);

	// Slow rotation
	FRotator Rotation = GetActorRotation();
	Rotation.Yaw += RotationSpeed * DeltaTime;
	SetActorRotation(Rotation);
}

UStaticMeshComponent* AFloatingCastle::CreateCubePart(const FString& Name, const FVector& Location, const FVector& Scale)
{
	UStaticMeshComponent* Comp = CreateDefaultSubobject<UStaticMeshComponent>(*Name);
	Comp->SetupAttachment(CastleRoot);
	Comp->SetRelativeLocation(Location * CastleScale);
	Comp->SetRelativeScale3D(Scale * CastleScale);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		Comp->SetStaticMesh(CubeMesh.Object);
	}

	Comp->SetCollisionProfileName(FName("BlockAll"));
	Comp->SetCastShadow(true);

	return Comp;
}

UStaticMeshComponent* AFloatingCastle::CreateCylinderPart(const FString& Name, const FVector& Location, const FVector& Scale)
{
	UStaticMeshComponent* Comp = CreateDefaultSubobject<UStaticMeshComponent>(*Name);
	Comp->SetupAttachment(CastleRoot);
	Comp->SetRelativeLocation(Location * CastleScale);
	Comp->SetRelativeScale3D(Scale * CastleScale);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (CylinderMesh.Succeeded())
	{
		Comp->SetStaticMesh(CylinderMesh.Object);
	}

	Comp->SetCollisionProfileName(FName("BlockAll"));
	Comp->SetCastShadow(true);

	return Comp;
}

void AFloatingCastle::BuildCastle()
{
	// === FLOATING BASE (rocky island underside) ===
	// A large inverted-pyramid-style base to suggest the castle is ripped from the earth
	CreateCubePart(TEXT("BaseRock1"), FVector(0, 0, -100), FVector(12, 12, 1.5));
	CreateCubePart(TEXT("BaseRock2"), FVector(0, 0, -180), FVector(9, 9, 1.0));
	CreateCubePart(TEXT("BaseRock3"), FVector(0, 0, -240), FVector(6, 6, 0.7));
	CreateCubePart(TEXT("BaseRockTip"), FVector(0, 0, -290), FVector(3, 3, 0.5));

	// === COURTYARD FLOOR ===
	CreateCubePart(TEXT("CourtyardFloor"), FVector(0, 0, -20), FVector(11, 11, 0.4));

	// === OUTER WALLS ===
	// Four walls around the perimeter
	BuildWall(TEXT("WallNorth"), FVector(-500, -500, 0), FVector(500, -500, 0));
	BuildWall(TEXT("WallSouth"), FVector(-500, 500, 0), FVector(500, 500, 0));
	BuildWall(TEXT("WallEast"), FVector(500, -500, 0), FVector(500, 500, 0));
	BuildWall(TEXT("WallWest"), FVector(-500, -500, 0), FVector(-500, 500, 0));

	// === BATTLEMENTS ===
	BuildBattlements(TEXT("BattNorth"), FVector(-500, -500, 0), FVector(500, -500, 0), 8);
	BuildBattlements(TEXT("BattSouth"), FVector(-500, 500, 0), FVector(500, 500, 0), 8);
	BuildBattlements(TEXT("BattEast"), FVector(500, -500, 0), FVector(500, 500, 0), 8);
	BuildBattlements(TEXT("BattWest"), FVector(-500, -500, 0), FVector(-500, 500, 0), 8);

	// === CORNER TOWERS ===
	BuildTower(TEXT("TowerNE"), FVector(500, -500, 0));
	BuildTower(TEXT("TowerNW"), FVector(-500, -500, 0));
	BuildTower(TEXT("TowerSE"), FVector(500, 500, 0));
	BuildTower(TEXT("TowerSW"), FVector(-500, 500, 0));

	// === CENTRAL KEEP ===
	// Main keep building - a large central structure
	CreateCubePart(TEXT("KeepBase"), FVector(0, 0, 150), FVector(4, 4, 3));
	CreateCubePart(TEXT("KeepUpper"), FVector(0, 0, 380), FVector(3.5, 3.5, 1.5));

	// Keep roof (pyramid-like, using a scaled cube rotated would be complex, so we stack)
	CreateCubePart(TEXT("KeepRoof1"), FVector(0, 0, 470), FVector(3.8, 3.8, 0.3));
	CreateCubePart(TEXT("KeepRoof2"), FVector(0, 0, 500), FVector(3.0, 3.0, 0.3));
	CreateCubePart(TEXT("KeepRoof3"), FVector(0, 0, 530), FVector(2.2, 2.2, 0.3));
	CreateCubePart(TEXT("KeepRoof4"), FVector(0, 0, 555), FVector(1.4, 1.4, 0.25));
	CreateCubePart(TEXT("KeepRoofPeak"), FVector(0, 0, 575), FVector(0.6, 0.6, 0.2));

	// Keep tower (tall spire on top)
	CreateCylinderPart(TEXT("KeepSpire"), FVector(0, 0, 620), FVector(0.5, 0.5, 1.0));

	// === GATEHOUSE ===
	// Front gate structure on the south wall
	CreateCubePart(TEXT("GateTowerL"), FVector(-120, 500, 100), FVector(1.2, 1.2, 2.5));
	CreateCubePart(TEXT("GateTowerR"), FVector(120, 500, 100), FVector(1.2, 1.2, 2.5));
	CreateCubePart(TEXT("GateArch"), FVector(0, 500, 230), FVector(2.8, 1.2, 0.5));

	// === INNER BUILDINGS ===
	// Small buildings inside the courtyard
	CreateCubePart(TEXT("Barracks"), FVector(-250, -200, 60), FVector(2.0, 1.5, 1.2));
	CreateCubePart(TEXT("BarracksRoof"), FVector(-250, -200, 130), FVector(2.2, 1.7, 0.2));

	CreateCubePart(TEXT("Chapel"), FVector(250, -200, 80), FVector(1.5, 2.0, 1.5));
	CreateCubePart(TEXT("ChapelRoof1"), FVector(250, -200, 170), FVector(1.7, 2.2, 0.2));
	CreateCubePart(TEXT("ChapelRoof2"), FVector(250, -200, 195), FVector(1.2, 1.7, 0.2));
	CreateCylinderPart(TEXT("ChapelSpire"), FVector(250, -200, 240), FVector(0.3, 0.3, 0.8));

	CreateCubePart(TEXT("Stables"), FVector(250, 200, 50), FVector(2.5, 1.2, 1.0));
	CreateCubePart(TEXT("StablesRoof"), FVector(250, 200, 110), FVector(2.7, 1.4, 0.15));

	// === FLOATING DEBRIS ===
	// Small rocks floating around the base for visual flair
	CreateCubePart(TEXT("FloatRock1"), FVector(700, 100, -150), FVector(1.5, 1.0, 0.8));
	CreateCubePart(TEXT("FloatRock2"), FVector(-600, -300, -200), FVector(1.0, 1.3, 0.6));
	CreateCubePart(TEXT("FloatRock3"), FVector(200, 650, -120), FVector(0.8, 0.8, 1.0));
	CreateCubePart(TEXT("FloatRock4"), FVector(-400, 500, -250), FVector(1.2, 0.7, 0.5));
}

void AFloatingCastle::BuildTower(const FString& BaseName, const FVector& Location)
{
	// Tower body - tall cylinder
	CreateCylinderPart(BaseName + TEXT("_Body"), Location + FVector(0, 0, 150), FVector(1.5, 1.5, 3.5));

	// Tower top - wider platform
	CreateCylinderPart(BaseName + TEXT("_Top"), Location + FVector(0, 0, 350), FVector(1.8, 1.8, 0.3));

	// Tower roof - stacked cones approximated by shrinking cylinders
	CreateCylinderPart(BaseName + TEXT("_Roof1"), Location + FVector(0, 0, 385), FVector(1.6, 1.6, 0.3));
	CreateCylinderPart(BaseName + TEXT("_Roof2"), Location + FVector(0, 0, 415), FVector(1.2, 1.2, 0.3));
	CreateCylinderPart(BaseName + TEXT("_Roof3"), Location + FVector(0, 0, 445), FVector(0.8, 0.8, 0.3));
	CreateCylinderPart(BaseName + TEXT("_RoofPeak"), Location + FVector(0, 0, 470), FVector(0.4, 0.4, 0.2));
}

void AFloatingCastle::BuildWall(const FString& BaseName, const FVector& Start, const FVector& End)
{
	FVector Center = (Start + End) * 0.5f;
	FVector Diff = End - Start;
	float Length = Diff.Size();

	// Wall center position, raised to wall mid-height
	FVector WallPos = Center + FVector(0, 0, 100);

	// Determine wall scale - length along the wall direction, thin depth, and wall height
	FVector WallScale;
	if (FMath::Abs(Diff.X) > FMath::Abs(Diff.Y))
	{
		// Runs along X
		WallScale = FVector(Length / 100.0f, 0.3f, 2.0f);
	}
	else
	{
		// Runs along Y
		WallScale = FVector(0.3f, Length / 100.0f, 2.0f);
	}

	CreateCubePart(BaseName, WallPos, WallScale);
}

void AFloatingCastle::BuildBattlements(const FString& BaseName, const FVector& Start, const FVector& End, int32 Count)
{
	for (int32 i = 0; i < Count; ++i)
	{
		// Only place merlons at even indices to create the gap pattern
		if (i % 2 == 0)
		{
			float Alpha = (float)(i + 0.5f) / (float)Count;
			FVector Pos = FMath::Lerp(Start, End, Alpha);
			Pos.Z += 220; // Top of the wall

			FString MerlonName = FString::Printf(TEXT("%s_Merlon%d"), *BaseName, i);
			CreateCubePart(MerlonName, Pos, FVector(0.5f, 0.5f, 0.4f));
		}
	}
}
