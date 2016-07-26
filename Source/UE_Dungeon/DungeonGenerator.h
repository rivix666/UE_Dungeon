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
	void						SpawnRooms();
	void						SpawnRoom(const SRoom& room);

	void						PlaceWalls();
	void						PlaceWall(int x, int y, EDir dir, const TSubclassOf<AWall>* wall);
	void						PlaceWallCorner(int x, int y, ECornerDir dir);

	AActor*						SpawnWall(float x, float y, FRotator rot, const TSubclassOf<AWall>* wall);
	void						SpawnWallCorner(float x, float y, FRotator rot);

	// Others
	void						SpawnPlayerInRoom(const SRoom& room);

private:
	int                        m_MaxWidth;
	int                        m_MaxHeight;

	const float				   TILE_SIZE = 200.0f;
    SRoom*					   m_RootRoom;

	// Containers
	int**                      m_MazeArr;
	int                        m_DirXArr[9];
	int                        m_DirYArr[9];
	int                        m_DirOppositeArr[9];

	std::vector <SRoom>         m_RoomsVec;


};
