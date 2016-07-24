// Fill out your copyright notice in the Description page of Project Settings.

#include "UE_Dungeon.h"
#include "DungeonGenerator.h"


// Sets default values
ADungeonGenerator::ADungeonGenerator() : m_MaxWidth(50), m_MaxHeight(50)
{
	PrimaryActorTick.bCanEverTick = false;
	InitMazeArray();
	InitDirectionArrays();
}

// Called when the game starts or when spawned
void ADungeonGenerator::BeginPlay()
{
	Super::BeginPlay();
	ClearMazeArray();
	GenRooms(20);
	CarveCorridorsBetweenRooms(0);
	ConnectRooms();
	UncarveDungeon(20);
	RemoveUnnecessaryTiles();

	PlaceWalls();
}

// Called every frame
void ADungeonGenerator::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Array Methods
//////////////////////////////////////////////////////////////////////////
void ADungeonGenerator::InitMazeArray()
{
	m_MazeArr = new uint*[m_MaxWidth];
	for (uint i = 0; i < m_MaxWidth; i++)
	{
		m_MazeArr[i] = new uint[m_MaxHeight];
	}

	ClearMazeArray();
}

void ADungeonGenerator::InitDirectionArrays()
{
	m_DirXArr[N] = 0;
	m_DirXArr[E] = 1;
	m_DirXArr[S] = 0;
	m_DirXArr[W] = -1;

	m_DirYArr[N] = -1;
	m_DirYArr[E] = 0;
	m_DirYArr[S] = 1;
	m_DirYArr[W] = 0;

	m_DirOppositeArr[N] = S;
	m_DirOppositeArr[E] = W;
	m_DirOppositeArr[S] = N;
	m_DirOppositeArr[W] = E;
}

void ADungeonGenerator::ClearMazeArray()
{
	for (uint i = 0; i < m_MaxWidth; i++)
	{
		for (uint j = 0; j < m_MaxHeight; j++)
		{
			m_MazeArr[i][j] = SolidRock;
		}
	}

	m_RoomsVec.clear();
}

void ADungeonGenerator::ShuffleDirArray(uint* arr, uint size)
{
	for (uint i = 0; i < (size - 1); i++)
	{
		int r = i + (FMath::RandRange(0, size - i));
		int temp = arr[i];
		arr[i] = arr[r];
		arr[r] = temp;
	}
}

// Dungeon Generation Methods
//////////////////////////////////////////////////////////////////////////
void ADungeonGenerator::GenRooms(int attempts)
{
	uint nx, ny;
	uint size_x, size_y;
	for (int i = 0; i < attempts; i++)
	{
		if (FMath::RandRange(0, 2) == SolidRock)
		{
			size_x = FMath::RandRange(0, 6) + 5;
			size_y = FMath::RandRange(0, (size_x / 2)) + 5;
		}
		else
		{
			size_y = FMath::RandRange(0, 6) + 5;
			size_x = FMath::RandRange(0, (size_y / 2)) + 5;
		}

		nx = FMath::RandRange(2, (m_MaxWidth - size_x - 2));
		ny = FMath::RandRange(2, (m_MaxHeight - size_y - 2));

		if (AreFieldsEmpty(nx, ny, size_x, size_y))
		{
			CarveRoom(nx, ny, size_x, size_y);
		}
	}
}

void ADungeonGenerator::GenMazeRecursiveBacktracking(uint pos_x, uint pos_y)
{

}

void ADungeonGenerator::CarveRoom(uint x, uint y, uint size_x, uint size_y)
{
	for (uint i = 0; i < size_x; i++)
	{
		for (uint j = 0; j < size_y; j++)
		{
			m_MazeArr[x + i][y + j] = Room;
		}
	}

	m_RoomsVec.push_back(SRoom(x, y, size_x, size_y));
}

void ADungeonGenerator::CarveCorridorsBetweenRooms(uint attempts /*= 0*/)
{

}

void ADungeonGenerator::ConnectRooms()
{

}

void ADungeonGenerator::ConnectRoom(SRoom& room)
{

}

void ADungeonGenerator::UncarveDungeon(int when_stop /*= -1*/)
{

}

void ADungeonGenerator::UncarveCorridor(uint x, uint y, int when_stop)
{

}

bool ADungeonGenerator::NextTileInCorridor(uint& nx, uint& ny)
{
	return false;
}

void ADungeonGenerator::RemoveUnnecessaryTiles()
{
	std::vector <uint*> tiles_to_remove;

	for (uint i = 1; i < m_MaxWidth - 1; i++)
	{
		for (uint j = 1; j < m_MaxHeight - 1; j++)
		{
			if (m_MazeArr[i][j] != SolidRock)
				continue;

			if (CheckNeighbours(i, j, SolidRock) == 4)
				tiles_to_remove.push_back(&m_MazeArr[i][j]);
		}
	}

	for (uint* t : tiles_to_remove)
		*t = Nothing;

	for (uint i = 0; i < m_MaxWidth; i++)
	{
		if (m_MazeArr[i][1] == Nothing || m_MazeArr[i][1] == SolidRock)
			m_MazeArr[i][0] = Nothing;

		uint h = m_MaxHeight - 2;
		if (m_MazeArr[i][h] == Nothing || m_MazeArr[i][h] == SolidRock)
			m_MazeArr[i][++h] = Nothing;
	}

	for (uint i = 0; i < m_MaxHeight; i++)
	{
		if (m_MazeArr[1][i] == Nothing || m_MazeArr[1][i] == SolidRock)
			m_MazeArr[0][i] = Nothing;


		uint w = m_MaxWidth - 2;
		if (m_MazeArr[w][i] == Nothing || m_MazeArr[w][i] == SolidRock)
			m_MazeArr[++w][i] = Nothing;
	}
}

int ADungeonGenerator::CheckNeighbours(uint dir, uint x, uint y)
{
	int walls = 0;
	switch (dir)
	{
	case N:
	{
		if (m_MazeArr[x - 1][y] == SolidRock)walls++;
		if (m_MazeArr[x + 1][y] == SolidRock)walls++;
		if (m_MazeArr[x][y - 1] == SolidRock)walls++;
		//             if(m_MazeArr[x - 1][y - 1] == SolidRock)walls++; // delete this two ever case to unlock cross walls
		//             if(m_MazeArr[x + 1][y - 1] == SolidRock)walls++;

		break;
	}
	case S:
	{
		if (m_MazeArr[x - 1][y] == SolidRock)walls++;
		if (m_MazeArr[x + 1][y] == SolidRock)walls++;
		if (m_MazeArr[x][y + 1] == SolidRock)walls++;
		//             if(m_MazeArr[x - 1][y + 1] == SolidRock)walls++;
		//             if(m_MazeArr[x + 1][y + 1] == SolidRock)walls++;
		break;
	}
	case E:
	{
		if (m_MazeArr[x][y + 1] == SolidRock)walls++;
		if (m_MazeArr[x][y - 1] == SolidRock)walls++;
		if (m_MazeArr[x + 1][y] == SolidRock)walls++;
		//             if(m_MazeArr[x + 1][y + 1] == SolidRock)walls++;
		//             if(m_MazeArr[x + 1][y - 1] == SolidRock)walls++;
		break;
	}
	case W:
	{
		if (m_MazeArr[x][y + 1] == SolidRock)walls++;
		if (m_MazeArr[x][y - 1] == SolidRock)walls++;
		if (m_MazeArr[x - 1][y] == SolidRock)walls++;
		//             if(m_MazeArr[x - 1][y + 1] == SolidRock)walls++;
		//             if(m_MazeArr[x - 1][y - 1] == SolidRock)walls++;
		break;
	}
	}
	return walls;
}

int ADungeonGenerator::CheckNeighbours(uint x, uint y, ETileType type)
{
	uint walls = 0;
	if (m_MazeArr[x + 1][y] == type) walls++;
	if (m_MazeArr[x - 1][y] == type) walls++;
	if (m_MazeArr[x][y + 1] == type) walls++;
	if (m_MazeArr[x][y - 1] == type) walls++;
	return walls;
}

int ADungeonGenerator::CheckNeighboursCross(uint x, uint y, ETileType type)
{
	uint walls = 0;
	if (m_MazeArr[x + 1][y + 1] == type) walls++;
	if (m_MazeArr[x + 1][y - 1] == type) walls++;
	if (m_MazeArr[x - 1][y + 1] == type) walls++;
	if (m_MazeArr[x - 1][y - 1] == type) walls++;
	return walls;
}

int ADungeonGenerator::IsThereAnyNeighbour(uint x, uint y, int type)
{
	uint walls = 0;
	if (m_MazeArr[x + 1][y] != type) walls++;
	if (m_MazeArr[x - 1][y] != type) walls++;
	if (m_MazeArr[x][y + 1] != type) walls++;
	if (m_MazeArr[x][y - 1] != type) walls++;
	return walls;
}

bool ADungeonGenerator::AreFieldsEmpty(uint x, uint y, uint size_x, uint size_y)
{
	uint nx = x - 1;
	uint ny = y - 1;
	uint size_nx = size_x + 2;
	uint size_ny = size_y + 2;

	for (uint i = 0; i < size_nx; i++)
	{
		for (uint j = 0; j < size_ny; j++)
		{
			if (m_MazeArr[nx + i][ny + j] != SolidRock)
				return false;
		}
	}

	return true;
}

// Mesh Placer
//////////////////////////////////////////////////////////////////////////
void ADungeonGenerator::SpawnRooms()
{

}

void ADungeonGenerator::PlaceWalls()
{
	for (uint i = 1; i < m_MaxWidth - 1; i++)
	{
		for (uint j = 1; j < m_MaxHeight - 1; j++)
		{
			if (m_MazeArr[i][j] == SolidRock)
			{
				PlaceWall(i, j);
				continue;
			}

			if (m_MazeArr[i][j] == Nothing)
			{
				PlaceWallCorner(i, j);
				continue;
			}
		}
	}
}

void ADungeonGenerator::PlaceWall(uint x, uint y)
{
	float fx = (float)x;
	float fy = (float)y;
	fx *= TILE_SIZE;
	fy *= TILE_SIZE;

	// Horizontal
	// Left
	int temp = m_MazeArr[x - 1][y] & ~Nothing;
	temp = temp & ~SolidRock;
	if (temp)
	{
		SpawnWall(fx, fy, FRotator(0.0f, 90.0f, 0.0f));
	}

	// Right
	temp = m_MazeArr[x + 1][y] & ~Nothing;
	temp = temp & ~SolidRock;
	if (temp)
	{
		SpawnWall(fx, fy, FRotator(0.0f, 270.0f, 0.0f));
	}
 
	// Vertical
	// Down
	temp = m_MazeArr[x][y - 1] & ~Nothing;
	temp = temp & ~SolidRock;
	if (temp)
	{
		SpawnWall(fx, fy, FRotator(0.0f, 180.0f, 0.0f));
	}
 
	// Up
	temp = m_MazeArr[x][y + 1] & ~Nothing;
	temp = temp & ~SolidRock;
	if (temp)
	{
		SpawnWall(fx, fy, FRotator(0.0f, 0.0f, 0.0f));
	}
}

void ADungeonGenerator::PlaceWallCorner(uint x, uint y)
{
	float fx = (float)x;
	float fy = (float)y;
	fx *= TILE_SIZE;
	fy *= TILE_SIZE;

	// Horizontal
	// Left
	bool temp = m_MazeArr[x][y + 1] == SolidRock;
	temp = temp && m_MazeArr[x + 1][y] == SolidRock;
	if (temp)
	{
		SpawnWallCorner(fx + 40.0f, fy + 40.0f, FRotator(0.0f, 270.0f, 0.0f));
	}

	// Right
	temp = m_MazeArr[x][y + 1] == SolidRock;
	temp = temp && m_MazeArr[x - 1][y] == SolidRock;
	if (temp)
	{
		SpawnWallCorner(fx - 40.0f, fy + 40.0f, FRotator(0.0f, 0.0f, 0.0f));
	}

	// Vertical
	// Down
	temp = m_MazeArr[x][y - 1] == SolidRock;
	temp = temp && m_MazeArr[x + 1][y] == SolidRock;
	if (temp)
	{
		SpawnWallCorner(fx + 40.0f, fy - 40.0f, FRotator(0.0f, 180.0f, 0.0f));
	}

	// Up
	temp = m_MazeArr[x][y - 1] == SolidRock;
	temp = temp && m_MazeArr[x - 1][y] == SolidRock;
	if (temp)
	{
		SpawnWallCorner(fx - 40.0f, fy - 40.0f, FRotator(0.0f, 90.0f, 0.0f));
	}
}

void ADungeonGenerator::SpawnWall(float x, float y, FRotator rot)
{
	const UWorld* world = GetWorld();

	if (world)
	{
		FActorSpawnParameters spawnParams;
		spawnParams.Owner = this;
		spawnParams.Instigator = Instigator;

		FVector newVec;
		newVec.Set(x, y, 0.0f);


		if(FMath::RandRange(0, 5) == 0)
			AActor* newActor = GetWorld()->SpawnActor<AWall>(m_Wall1, newVec, rot, spawnParams);
		else
			AActor* newActor = GetWorld()->SpawnActor<AWall>(m_Wall0, newVec, rot, spawnParams);
	}
}

void ADungeonGenerator::SpawnWallCorner(float x, float y, FRotator rot)
{
	const UWorld* world = GetWorld();

	if (world)
	{
		FActorSpawnParameters spawnParams;
		spawnParams.Owner = this;
		spawnParams.Instigator = Instigator;

		FVector newVec;
		newVec.Set(x, y, 0.0f);

		AActor* newActor = GetWorld()->SpawnActor<AWall>(m_WallCorner0, newVec, rot, spawnParams);
	}
}

