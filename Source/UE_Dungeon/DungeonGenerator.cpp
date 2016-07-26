// Fill out your copyright notice in the Description page of Project Settings.
#include "UE_Dungeon.h"
#include "DungeonGenerator.h"


#include <iostream>
#include <fstream>
#include <sstream>

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
	GenRooms(50, true);
	RemoveUnnecessaryTiles();


	for(SRoom room : m_RoomsVec)
		SpawnRoom(room);



	SpawnPlayerInRoom(m_RoomsVec[0]);

	//PlaceWalls();
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
	m_MazeArr = new int*[m_MaxWidth];
	for (int i = 0; i < m_MaxWidth; i++)
	{
		m_MazeArr[i] = new int[m_MaxHeight];
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
	for (int i = 0; i < m_MaxWidth; i++)
	{
		for (int j = 0; j < m_MaxHeight; j++)
		{
			m_MazeArr[i][j] = SolidRock;
		}
	}

	m_RoomsVec.clear();
}

void ADungeonGenerator::ShuffleDirArray(int* arr, int size)
{
	for (int i = 0; i < (size - 1); i++)
	{
		int r = i + (FMath::RandRange(0, size - i));
		int temp = arr[i];
		arr[i] = arr[r];
		arr[r] = temp;
	}
}

// Dungeon Generation Methods
//////////////////////////////////////////////////////////////////////////
void ADungeonGenerator::GenRooms(int attempts, bool first /*=false*/)
{
	int nx, ny;
	int size_x, size_y;

	if (first)
	{
		size_x = 7;
		size_y = 7;
		nx = (m_MaxWidth - 6) / 2;
		ny = (m_MaxHeight - 6) / 2;

		CarveRoom(nx, ny, size_x, size_y);
		m_RootRoom = &m_RoomsVec[m_RoomsVec.size() - 1];
		AddDoors(nx + 3, ny - 1, N, *m_RootRoom);
	}

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

		nx = FMath::RandRange(6, (m_MaxWidth - size_x - 1) - 12);
		ny = FMath::RandRange(6, (m_MaxHeight - size_y - 1) - 12);

		if (AreFieldsEmpty(nx - 5, ny - 5, size_x + 10, size_y + 10)) // chceck space a little bigger
		{
			CreateDoors(CarveRoom(nx, ny, size_x, size_y));
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//	DEBUG	
	//////////////////////////////////////////////////////////////////////////
// 	 	std::ofstream myfile;
// 	 	myfile.open("example.txt");
// 	 	for (int i = 0; i < m_MaxWidth; i++)
// 	 	{
// 	 		std::string aha;
// 	 		for (int j = 0; j < m_MaxHeight; j++)
// 	 		{
// 	 			std::string tmp;
// 	 
// 	 			std::stringstream out;
// 	 			out << m_MazeArr[i][j];
// 	 			tmp = out.str();
// 	 
// 	 			aha += tmp;
// 	 		}
// 	 		aha += "\n";
// 	 		myfile << aha.c_str();
// 	 	}
// 	 	myfile.close();
	//////////////////////////////////////////////////////////////////////////
}


void ADungeonGenerator::AddDoors(int x, int y, EDir dir, SRoom& room)
{
	m_MazeArr[x][y] = Doors;
	room.RoomDoors.push_back(SDoor(FVector2D(x, y), dir));
}

void ADungeonGenerator::CreateDoors(SRoom& room)
{
	EDir dir;
	int side, nx, ny;
	while (true)
	{
		side = FMath::RandRange(0, 1);
		if (side > 0)
		{
			nx = FMath::RandRange((room.PosX + 2), (room.PosX + room.SizeX - 2)); // working FMath::RandRange((room.PosX), (room.PosX + room.SizeX)); // +/- 2 by nei bylo po bokach za bardzo
			side = FMath::RandRange(0, 1);
			if (side > 0)
			{
				ny = room.PosY - 1; // S
				dir = N;
			}
			else
			{
				ny = room.PosY + room.SizeY; // N
				dir = S;
			}
		}
		else
		{
			ny = FMath::RandRange((room.PosY + 2), (room.PosY + room.SizeY - 2)); //  workking FMath::RandRange((room.PosY), (room.PosY + room.SizeY));
			side = FMath::RandRange(0, 1);
			if (side > 0)
			{
				nx = room.PosX - 1; // E
				dir = W;
			}
			else
			{
				nx = room.PosX + room.SizeX; // W
				dir = E;
			}
		}

		if (m_MazeArr[nx][ny] == Doors) // if there are doors, repeat and find another spot
			continue;

		AddDoors(nx, ny, dir, room); // add doors

		side = FMath::RandRange(0, 5); // 20% chance to create another doors in this room
		if (side < 5)
			return;
	}	
}

SRoom& ADungeonGenerator::CarveRoom(int x, int y, int size_x, int size_y)
{
	for (int i = 0; i < size_x; i++)
	{
		for (int j = 0; j < size_y; j++)
		{
			m_MazeArr[x + i][y + j] = Room;
		}
	}

	m_RoomsVec.push_back(SRoom(x, y, size_x, size_y));
	return m_RoomsVec.back();
}

void ADungeonGenerator::RemoveUnnecessaryTiles()
{
	std::vector <int*> tiles_to_remove;

	for (int i = 1; i < m_MaxWidth - 1; i++)
	{
		for (int j = 1; j < m_MaxHeight - 1; j++)
		{
			if (m_MazeArr[i][j] != SolidRock)
				continue;

			if (CheckNeighbours(i, j, SolidRock) == 4)
				tiles_to_remove.push_back(&m_MazeArr[i][j]);
		}
	}

	for (int* t : tiles_to_remove)
		*t = Nothing;

	for (int i = 0; i < m_MaxWidth; i++)
	{
		if (m_MazeArr[i][1] == Nothing || m_MazeArr[i][1] == SolidRock)
			m_MazeArr[i][0] = Nothing;

		int h = m_MaxHeight - 2;
		if (m_MazeArr[i][h] == Nothing || m_MazeArr[i][h] == SolidRock)
			m_MazeArr[i][++h] = Nothing;
	}

	for (int i = 0; i < m_MaxHeight; i++)
	{
		if (m_MazeArr[1][i] == Nothing || m_MazeArr[1][i] == SolidRock)
			m_MazeArr[0][i] = Nothing;


		int w = m_MaxWidth - 2;
		if (m_MazeArr[w][i] == Nothing || m_MazeArr[w][i] == SolidRock)
			m_MazeArr[++w][i] = Nothing;
	}
}

int ADungeonGenerator::CheckNeighbours(int dir, int x, int y)
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

int ADungeonGenerator::CheckNeighbours(int x, int y, ETileType type)
{
	int walls = 0;
	if (m_MazeArr[x + 1][y] == type) walls++;
	if (m_MazeArr[x - 1][y] == type) walls++;
	if (m_MazeArr[x][y + 1] == type) walls++;
	if (m_MazeArr[x][y - 1] == type) walls++;
	return walls;
}

int ADungeonGenerator::CheckNeighboursCross(int x, int y, ETileType type)
{
	int walls = 0;
	if (m_MazeArr[x + 1][y + 1] == type) walls++;
	if (m_MazeArr[x + 1][y - 1] == type) walls++;
	if (m_MazeArr[x - 1][y + 1] == type) walls++;
	if (m_MazeArr[x - 1][y - 1] == type) walls++;
	return walls;
}

int ADungeonGenerator::IsThereAnyNeighbour(int x, int y, int type)
{
	int walls = 0;
	if (m_MazeArr[x + 1][y] != type) walls++;
	if (m_MazeArr[x - 1][y] != type) walls++;
	if (m_MazeArr[x][y + 1] != type) walls++;
	if (m_MazeArr[x][y - 1] != type) walls++;
	return walls;
}

bool ADungeonGenerator::AreFieldsEmpty(int x, int y, int size_x, int size_y)
{
	int nx = x - 1;
	int ny = y - 1;
	int size_nx = size_x + 2;
	int size_ny = size_y + 2;

	for (int i = 0; i < size_nx; i++)
	{
		for (int j = 0; j < size_ny; j++)
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

void ADungeonGenerator::SpawnRoom(const SRoom& room)
{
	// Place Walls
	int n0, n1, n2;
	n1 = (room.PosY - 1);
	n2 = (room.PosY + room.SizeY);
	for (int i = 0; i < room.SizeX; i++)
	{
		n0 = room.PosX + i;
		PlaceWall(n0 * TILE_SIZE, n1 * TILE_SIZE, N, m_MazeArr[n0][n1] == Doors ? &m_WallDoorOpen : nullptr);
		PlaceWall(n0 * TILE_SIZE, n2 * TILE_SIZE, S, m_MazeArr[n0][n2] == Doors ? &m_WallDoorOpen : nullptr);
	}

	n1 = (room.PosX - 1);
	n2 = (room.PosX + room.SizeX);
	for (int j = 0; j < room.SizeY; j++)
	{
		n0 = room.PosY + j;
		PlaceWall(n1 * TILE_SIZE, n0 * TILE_SIZE, W, m_MazeArr[n1][n0] == Doors ? &m_WallDoorOpen : nullptr);
		PlaceWall(n2 * TILE_SIZE, n0 * TILE_SIZE, E, m_MazeArr[n2][n0] == Doors ? &m_WallDoorOpen : nullptr);
	}

	// Place Room Corners
	n0 = (room.PosX - 1) * TILE_SIZE;
	n1 = (room.PosY - 1) * TILE_SIZE;
	PlaceWallCorner(n0, n1, SW);
	n0 = (room.PosX - 1) * TILE_SIZE;
	n1 = (room.PosY + room.SizeY) * TILE_SIZE;
	PlaceWallCorner(n0, n1, SE);
	n0 = (room.PosX + room.SizeX) * TILE_SIZE;
	n1 = (room.PosY - 1) * TILE_SIZE;
	PlaceWallCorner(n0, n1, NW);
	n0 = (room.PosX + room.SizeX) * TILE_SIZE;
	n1 = (room.PosY + room.SizeY) * TILE_SIZE;
	PlaceWallCorner(n0, n1, NE);

	// Place Outside Doors
	for (SDoor door : room.RoomDoors) //to do calkie inaczej to zrobic, ogolnie caly kod ten jest mega chujowy, nie podoba mis ie w chuj
	{
		PlaceWall(door.OutsideDoor.X * TILE_SIZE, door.OutsideDoor.Y * TILE_SIZE, door.OutsideDoorDir, &m_WallDoorOpen);

		if (door.OutsideDoorDir == N)
		{
			PlaceWallCorner((door.OutsideDoor.X - 1.4) * TILE_SIZE, door.OutsideDoor.Y * TILE_SIZE, SW);
			PlaceWallCorner((door.OutsideDoor.X + 1.4) * TILE_SIZE, door.OutsideDoor.Y * TILE_SIZE, NW);
		}
		else if (door.OutsideDoorDir == S)
		{
			PlaceWallCorner((static_cast<float>(door.OutsideDoor.X) + 1.4) * TILE_SIZE, door.OutsideDoor.Y * TILE_SIZE, NE);
			PlaceWallCorner((static_cast<float>(door.OutsideDoor.X) - 1.4) * TILE_SIZE, door.OutsideDoor.Y * TILE_SIZE, SE); //r
		}
		else if (door.OutsideDoorDir == W)
		{
			PlaceWallCorner(door.OutsideDoor.X * TILE_SIZE, (static_cast<float>(door.OutsideDoor.Y) - 1.4) * TILE_SIZE, SW);
			PlaceWallCorner(door.OutsideDoor.X * TILE_SIZE, (static_cast<float>(door.OutsideDoor.Y) + 1.4) * TILE_SIZE, SE);
		}
		else
		{
			PlaceWallCorner(door.OutsideDoor.X * TILE_SIZE * TILE_SIZE, (static_cast<float>(door.OutsideDoor.Y) - 1.4) * TILE_SIZE, NE);
			PlaceWallCorner(door.OutsideDoor.X * TILE_SIZE * TILE_SIZE, (static_cast<float>(door.OutsideDoor.Y) + 1.4) * TILE_SIZE, NW); //r
		}


	}
}

void ADungeonGenerator::PlaceWalls()
{
// 	for (int i = 1; i < m_MaxWidth - 1; i++)
// 	{
// 		for (int j = 1; j < m_MaxHeight - 1; j++)
// 		{
// 			if (m_MazeArr[i][j] == SolidRock)
// 			{
// 				PlaceWall(i, j);
// 				continue;
// 			}
// 
// 			if (m_MazeArr[i][j] == Nothing)
// 			{
// 				PlaceWallCorner(i, j);
// 				continue;
// 			}
// 		}
// 	}
}

void ADungeonGenerator::PlaceWall(int x, int y, EDir dir, const TSubclassOf<AWall>* wall)
{
	float fx = (float)x;
	float fy = (float)y;

	switch (dir)
	{
	case N:
		SpawnWall(fx, fy, FRotator::ZeroRotator, wall);
		break;
	case S:
		SpawnWall(fx, fy, FRotator(0.0f, 180.0f, 0.0f), wall);
		break;
	case W:
		SpawnWall(fx, fy, FRotator(0.0f, 270.0f, 0.0f), wall);
		break;
	case E:
		SpawnWall(fx, fy, FRotator(0.0f, 90.0f, 0.0f), wall);
		break;
	}
}

void ADungeonGenerator::PlaceWallCorner(int x, int y, ECornerDir dir)
{
	float fx = (float)x;
	float fy = (float)y;

	switch (dir)
	{
	case NW:
		SpawnWallCorner(fx - 40.0f, fy + 40.0f, FRotator::ZeroRotator);
		break;
	case NE:
		SpawnWallCorner(fx - 40.0f, fy - 40.0f, FRotator(0.0f, 90.0f, 0.0f));
		break;
	case SW:
		SpawnWallCorner(fx + 40.0f, fy + 40.0f, FRotator(0.0f, 270.0f, 0.0f));
		break;
	case SE:
		SpawnWallCorner(fx + 40.0f, fy - 40.0f, FRotator(0.0f, 180.0f, 0.0f));
		break;
	}
}

AActor* ADungeonGenerator::SpawnWall(float x, float y, FRotator rot, const TSubclassOf<AWall>* wall)
{
	AActor* newActor = nullptr;
	const UWorld* world = GetWorld();

	if (world)
	{
		FActorSpawnParameters spawnParams;
		spawnParams.Owner = this;
		spawnParams.Instigator = Instigator;

		FVector newVec;
		newVec.Set(x, y, 0.0f);

		if(wall)
			newActor = GetWorld()->SpawnActor<AWall>(*wall, newVec, rot, spawnParams);
		else
		{
			if (FMath::RandRange(0, 3) != 0)
				newActor = GetWorld()->SpawnActor<AWall>(m_Wall0, newVec, rot, spawnParams);
			else
				newActor = GetWorld()->SpawnActor<AWall>(m_Wall1, newVec, rot, spawnParams);
		}
	}

	return newActor;
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

// Others
void ADungeonGenerator::SpawnPlayerInRoom(const SRoom& room)
{
	ACharacter* myCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (myCharacter)
	{
		float x = (float)room.PosX + ((float)room.SizeX / 2.0f);
		float y = (float)room.PosY + ((float)room.SizeY / 2.0f);

		myCharacter->SetActorLocation(FVector(x, y, 0.0f));
	}
}

