// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <map>
#include <list>
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
	//////////////////////////////////////////////////////////////////////////
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

	// Maze gen
	void                        CarveCorridorsBetweenRooms(int attempts = 0); // with attempts == 0, slow but check every possibility, with attempts > 0 faster but may result with empty spaces


	// A Star
	void						GenerateMinimumSpanningTree();
	void						CreatePathsBetweenRooms();
	void						FindAStarPaths(SDoor* d1, SDoor* d2);
	int							GetAStarG(const SPoint& start, const SPoint& end);
	void						CheckNeighbours(SPath* cur, const SPoint& start, const SPoint& end, std::list <SPath*>& open_list, std::list<SPath*>& closed_list);

	// Mesh Placers
	//////////////////////////////////////////////////////////////////////////
	void						PlaceRoom(const SRoom& room);
	void						PlacePassage(int x, int y, int dirs); // if turn/crossroad next block need to be set 2 spaces away | WE - X Axis, NS - Y Axis

	// Corridors
	void						PlaceCorridor(int x, int y, int open_dirs);
	void						PlaceDeadEnd(int x, int y, int open_dir);
	void						PlaceCrossRoad(int x, int y);
	void						PlaceTCrossRoad(int x, int y, int open_dirs);

	// Walls
	void						PlaceWall(int x, int y, int dir, const TSubclassOf<AWall>* wall = nullptr);
	void						PlaceWallCorner(int x, int y, int dir, const TSubclassOf<AWall>* wall = nullptr);

	// Spawners
	AActor*						SpawnWall(float x, float y, FRotator rot, const TSubclassOf<AWall>* wall = nullptr);
	AActor*						SpawnWallCorner(float x, float y, FRotator rot, const TSubclassOf<AWall>* wall = nullptr);

	void						SpawnPlayerInRoom(const SRoom& room);

	// Others
	int							NumberOfSetBits(int i);

    int shuffle_array(int *arr, int size);
    int carve_passage(int cx, int cy);


private:
	int                        m_MaxWidth;
	int                        m_MaxHeight;

    SRoom*					   m_RootRoom;

	// Containers
	int**                      m_MazeArr;
	std::map <int, int>        m_DirXArr;
	std::map <int, int>        m_DirYArr;
	std::map <int, int>        m_DirOppositeArr;

	std::vector <SRoom>        m_RoomsVec;
    std::vector <AActor*>      m_LastWalls;

	uint					   m_TorchIncrementator;;
	uint					   m_TorchModulo;

    // Debug
    //////////////////////////////////////////////////////////////////////////
    void DrawDebugBoxes();
    void DrawDebugStrings();

    //////////////////////////////////////////////////////////////////////////
	void                        GenMazeRecursiveBacktracking(int pos_x, int pos_y);
};
