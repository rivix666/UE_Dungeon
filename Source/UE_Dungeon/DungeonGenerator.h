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

	UPROPERTY(EditDefaultsOnly, Category = "Wall Corner 0")
		TSubclassOf<AWall> m_WallCorner0;

protected:
	// Maze array controll methods
	void                        InitMazeArray();
	void                        InitDirectionArrays();
	void                        ClearMazeArray();
	void                        ShuffleDirArray(uint* arr, uint size);

	// Dungeon Generation 
	void                        GenRooms(int attempts);
	void                        GenMazeRecursiveBacktracking(uint pos_x, uint pos_y);
	void                        CarveRoom(uint x, uint y, uint size_x, uint size_y);
	void                        CarveCorridorsBetweenRooms(uint attempts = 0); // with attempts == 0, slow but check every possibility, with attempts > 0 faster but may result with empty spaces
	void                        ConnectRooms(); // -1 mean that it will be randomly draw from m_RoomsVec
	void                        ConnectRoom(SRoom& room);
	void                        UncarveDungeon(int when_stop = -1); // if when_stop == -1 the uncarve to perfect dungeon
	void                        UncarveCorridor(uint x, uint y, int when_stop);
	bool                        NextTileInCorridor(uint& nx, uint& ny); // false == end of corridor
	void                        RemoveUnnecessaryTiles();

	int                         CheckNeighbours(uint dir, uint x, uint y);
	int                         CheckNeighbours(uint x, uint y, ETileType type);
	int                         CheckNeighboursCross(uint x, uint y, ETileType type);
	int                         IsThereAnyNeighbour(uint x, uint y, int type);
	bool                        AreFieldsEmpty(uint x, uint y, uint size_x, uint size_y);

	// Mesh Placers
	void						SpawnRooms();


	void						PlaceWalls();
	void						PlaceWall(uint x, uint y);
	void						PlaceWallCorner(uint x, uint y);
	void						SpawnWall(float x, float y, FRotator rot);
	void						SpawnWallCorner(float x, float y, FRotator rot);

private:
	uint                        m_MaxWidth;
	uint                        m_MaxHeight;

	const float					TILE_SIZE = 200.0f;

	// Containers
	uint**                      m_MazeArr;
	uint                        m_DirXArr[9];
	uint                        m_DirYArr[9];
	uint                        m_DirOppositeArr[9];

	std::vector <SRoom>         m_RoomsVec;


};
