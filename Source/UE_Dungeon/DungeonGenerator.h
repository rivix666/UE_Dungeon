// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Wall.h"
#include "Misc.h"

#include "DungeonGenerator.generated.h"

UCLASS()
class UE_DUNGEON_API ADungeonGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADungeonGenerator();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// UE Properties
	UPROPERTY(EditDefaultsOnly, Category = "Simple Wall 0")
		TSubclassOf<AWall> m_Wall0;
	UPROPERTY(EditDefaultsOnly, Category = "Simple Wall 1")
		TSubclassOf<AWall> m_Wall1;
	UPROPERTY(EditDefaultsOnly, Category = "Simple Wall Open Doors")
		TSubclassOf<AWall> m_WallDoorOpen;

	UPROPERTY(EditDefaultsOnly, Category = "Wall Corner 0")
		TSubclassOf<AWall> m_WallCorner0;
	UPROPERTY(EditDefaultsOnly, Category = "Wall Corner 1 In")
		TSubclassOf<AWall> m_WallCorner1_In;
	UPROPERTY(EditDefaultsOnly, Category = "Wall Corner 2 Out")
		TSubclassOf<AWall> m_WallCorner2_Out;

protected:
	// Maze array controll methods
	void                        InitMazeArray();
	void                        InitDirectionArrays();
	void                        ClearMazeArray();
	void                        ShuffleDirArray(int* arr, int size);

	// Dungeon Generation 
	void                        GenRooms(int attempts, bool first = false);
	void						AddDoors(int x, int y, EDir dir, SRoom& room);
	void						CreateDoors(SRoom& room);
	SRoom&                      CarveRoom(int x, int y, int size_x, int size_y);
	void                        RemoveUnnecessaryTiles();

	int                         CheckNeighbours(int dir, int x, int y);
	int                         CheckNeighbours(int x, int y, ETileType type);
	int                         CheckNeighboursCross(int x, int y, ETileType type);
	int                         IsThereAnyNeighbour(int x, int y, int type);
	bool                        AreFieldsEmpty(int x, int y, int size_x, int size_y);

	// Mesh Placers
	void						PlaceRoom(const SRoom& room);
	void						PlacePassage(int x, int y, int dirs);

	// Corridors
	void						PlaceCorridor(int x, int y, EDir open_dirs);
	void						PlaceDeadEnd(int x, int y, EDir open_dir);
	void						PlaceCrossRoad(int x, int y);
	void						PlaceTCrossRoad(int x, int y, EDir open_dirs);

	// Walls
	void						PlaceWall(int x, int y, EDir dir, const TSubclassOf<AWall>* wall = nullptr);
	void						PlaceWallCorner(int x, int y, ECornerDir dir, const TSubclassOf<AWall>* wall = nullptr);

	// Spawners
	AActor*						SpawnWall(float x, float y, FRotator rot, const TSubclassOf<AWall>* wall = nullptr);
	AActor*						SpawnWallCorner(float x, float y, FRotator rot, const TSubclassOf<AWall>* wall = nullptr);

	void						SpawnPlayerInRoom(const SRoom& room);

	// Others
	int							NumberOfSetBits(int i);

private:
	int                        m_MaxWidth;
	int                        m_MaxHeight;

    SRoom*					   m_RootRoom;

	// Containers
	int**                      m_MazeArr;
	int                        m_DirXArr[9];
	int                        m_DirYArr[9];
	int                        m_DirOppositeArr[9];

	std::vector <SRoom>        m_RoomsVec;

	uint					   m_TorchIncrementator;;
	uint					   m_TorchModulo;


};
