// Fill out your copyright notice in the Description page of Project Settings.
#include "UE_Dungeon.h"
#include "DungeonGenerator.h"


#include <iostream>
#include <fstream>
#include <sstream>

// TODO zrobic porz¹dek z kierunkami N ma byc do góry!!

// Sets default values
ADungeonGenerator::ADungeonGenerator() : 
	m_MaxWidth(50), m_MaxHeight(50), 
	m_TorchIncrementator(0),
	m_TorchModulo(FMath::RandRange(3, 8))
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
//	RemoveUnnecessaryTiles();


	for(SRoom room : m_RoomsVec)
		PlaceRoom(room);

	SpawnPlayerInRoom(m_RoomsVec[0]);


    carve_passage(1, 1);



// 

//  	PlacePassage(0.0, 0.0,  N | W);
//     PlacePassage(5.0, 0.0,  E | N);

   // PlacePassage(10.0, 0.0, S | E);
  //  PlacePassage(15.0, 0.0, W | S);
// 	PlacePassage(0.0, 0.0, S);
// 	PlacePassage(0.0, 0.0, E);
// 	PlacePassage(0.0, 0.0, W);
// 	PlacePassage(0.0, 0.0, N | S | E | W);
// 	PlacePassage(0.0, 0.0, N | S);
// 	PlacePassage(0.0, 0.0, N | W);

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

// Rooms
void ADungeonGenerator::PlaceRoom(const SRoom& room)
{
	// Place Walls
	int n0, n1, n2;
	n1 = (room.PosY - 1);
	n2 = (room.PosY + room.SizeY);
	for (int i = 0; i < room.SizeX; i++)
	{
		n0 = room.PosX + i;
		PlaceWall(A2W(n0), A2W(n1), N, m_MazeArr[n0][n1] == Doors ? &m_WallDoorOpen : nullptr);
		PlaceWall(A2W(n0), A2W(n2), S, m_MazeArr[n0][n2] == Doors ? &m_WallDoorOpen : nullptr);
	}

	n1 = (room.PosX - 1);
	n2 = (room.PosX + room.SizeX);
	for (int j = 0; j < room.SizeY; j++)
	{
		n0 = room.PosY + j;
		PlaceWall(A2W(n1), A2W(n0), W, m_MazeArr[n1][n0] == Doors ? &m_WallDoorOpen : nullptr);
		PlaceWall(A2W(n2), A2W(n0), E, m_MazeArr[n2][n0] == Doors ? &m_WallDoorOpen : nullptr);
	}

	// Place Room Corners
	n0 = A2W((room.PosX - 1));
	n1 = A2W((room.PosY - 1));
	PlaceWallCorner(n0, n1, SW);
	n0 = A2W((room.PosX - 1));
	n1 = A2W((room.PosY + room.SizeY));
	PlaceWallCorner(n0, n1, SE);
	n0 = A2W((room.PosX + room.SizeX));
	n1 = A2W((room.PosY - 1));
	PlaceWallCorner(n0, n1, NW);
	n0 = A2W((room.PosX + room.SizeX));
	n1 = A2W((room.PosY + room.SizeY));
	PlaceWallCorner(n0, n1, NE);

	// Place Outside Doors
	for (SDoor door : room.RoomDoors) //to do calkie inaczej to zrobic, ogolnie caly kod ten jest mega chujowy, nie podoba mis ie w chuj
	{
		PlaceWall(A2W(door.OutsideDoor.X), A2W(door.OutsideDoor.Y), door.OutsideDoorDir, &m_WallDoorOpen);

		if (door.OutsideDoorDir == N)
		{
			PlaceWallCorner(A2W((door.OutsideDoor.X - 1./*4*/)), A2W(door.OutsideDoor.Y), SW);
			PlaceWallCorner(A2W((door.OutsideDoor.X + 1./*4*/)), A2W(door.OutsideDoor.Y), NW);
		}
		else if (door.OutsideDoorDir == S)
		{
			PlaceWallCorner(A2W((static_cast<float>(door.OutsideDoor.X) + 1./*4*/)), A2W(door.OutsideDoor.Y), NE);
			PlaceWallCorner(A2W((static_cast<float>(door.OutsideDoor.X) - 1./*4*/)), A2W(door.OutsideDoor.Y), SE); //r
		}
		else if (door.OutsideDoorDir == W)
		{
			PlaceWallCorner(A2W(door.OutsideDoor.X), A2W((static_cast<float>(door.OutsideDoor.Y) - 1./*4*/)), SW);
			PlaceWallCorner(A2W(door.OutsideDoor.X), A2W((static_cast<float>(door.OutsideDoor.Y) + 1./*4*/)), SE);
		}
		else
		{
			PlaceWallCorner(A2W(door.OutsideDoor.X), A2W((static_cast<float>(door.OutsideDoor.Y) - 1./*4*/)), NW);
			PlaceWallCorner(A2W(door.OutsideDoor.X), A2W((static_cast<float>(door.OutsideDoor.Y) + 1./*4*/)), NE); //r
		}
	}
}

// Corridors
void ADungeonGenerator::PlacePassage(int x, int y, int dirs)
{
	int counter = NumberOfSetBits(dirs);
	switch (counter)
	{
	case 1:
		PlaceDeadEnd(x, y, dirs);
		break;
	case 2:
        PlaceCorridor(x, y, dirs);
		break;
	case 3:
		break;
	case 4:
		break;
	default:
		break;
	}
}

void ADungeonGenerator::PlaceCorridor(int x, int y, int open_dirs) //todo chujoza totalnie wymienic
{
    if (open_dirs & N)
    {
        if (open_dirs & S)
        {
            PlaceWall(A2W((x + m_DirXArr[W])), A2W((y + m_DirYArr[W])), (EDir)W, nullptr);
            PlaceWall(A2W((x + m_DirXArr[E])), A2W((y + m_DirYArr[E])), (EDir)E, nullptr);
        }
        else
        {
            if (open_dirs & E)
            {
                PlaceWall(A2W((x + m_DirXArr[S])), A2W((y + m_DirYArr[S])), (EDir)S, nullptr);
                PlaceWall(A2W((x + m_DirXArr[W])), A2W((y + m_DirYArr[W])), (EDir)W, nullptr);
            }
            else
            {
                PlaceWall(A2W((x + m_DirXArr[S])), A2W((y + m_DirYArr[S])), (EDir)S, nullptr);
                PlaceWall(A2W((x + m_DirXArr[E])), A2W((y + m_DirYArr[E])), (EDir)E, nullptr);
            }
        }
    }
    else if (open_dirs & S)
    {
        if (open_dirs & N)
        {
            PlaceWall(A2W((x + m_DirXArr[W])), A2W((y + m_DirYArr[W])), (EDir)W, nullptr);
            PlaceWall(A2W((x + m_DirXArr[E])), A2W((y + m_DirYArr[E])), (EDir)E, nullptr);
        }
        else
        {
            if (open_dirs & E)
            {
                PlaceWall(A2W((x + m_DirXArr[N])), A2W((y + m_DirYArr[N])), (EDir)N, nullptr);
                PlaceWall(A2W((x + m_DirXArr[W])), A2W((y + m_DirYArr[W])), (EDir)W, nullptr);
            }
            else
            {
                PlaceWall(A2W((x + m_DirXArr[N])), A2W((y + m_DirYArr[N])), (EDir)N, nullptr);
                PlaceWall(A2W((x + m_DirXArr[E])), A2W((y + m_DirYArr[E])), (EDir)E, nullptr);
            }
        }
    }
    else if (open_dirs & E)
    {
        if (open_dirs & W)
        {
            PlaceWall(A2W((x + m_DirXArr[N])), A2W((y + m_DirYArr[N])), (EDir)N, nullptr);
            PlaceWall(A2W((x + m_DirXArr[S])), A2W((y + m_DirYArr[S])), (EDir)S, nullptr);
        }
        else
        {
            if (open_dirs & N)
            {
                PlaceWall(A2W((x + m_DirXArr[W])), A2W((y + m_DirYArr[W])), (EDir)W, nullptr);
                PlaceWall(A2W((x + m_DirXArr[S])), A2W((y + m_DirYArr[S])), (EDir)S, nullptr);
            }
            else
            {
                PlaceWall(A2W((x + m_DirXArr[W])), A2W((y + m_DirYArr[W])), (EDir)W, nullptr);
                PlaceWall(A2W((x + m_DirXArr[N])), A2W((y + m_DirYArr[N])), (EDir)N, nullptr);
            }
        }
    }
    else if (open_dirs & W)
    {
        if (open_dirs & E)
        {
            PlaceWall(A2W((x + m_DirXArr[N])), A2W((y + m_DirYArr[N])), (EDir)N, nullptr);
            PlaceWall(A2W((x + m_DirXArr[S])), A2W((y + m_DirYArr[S])), (EDir)S, nullptr);
        }
        else
        {
            if (open_dirs & N)
            {
                PlaceWall(A2W((x + m_DirXArr[E])), A2W((y + m_DirYArr[E])), (EDir)E, nullptr);
                PlaceWall(A2W((x + m_DirXArr[S])), A2W((y + m_DirYArr[S])), (EDir)S, nullptr);
            }
            else
            {
                PlaceWall(A2W((x + m_DirXArr[E])), A2W((y + m_DirYArr[E])), (EDir)E, nullptr);
                PlaceWall(A2W((x + m_DirXArr[N])), A2W((y + m_DirYArr[N])), (EDir)N, nullptr);
            }
        }
    }
}

void ADungeonGenerator::PlaceDeadEnd(int x, int y, int open_dir)
{
    int d = m_DirOppositeArr[open_dir];
    int d1, d2;


    // todo repair this shit
    //////////////////////////////////////////////////////////////////////////
    if (d == E || d == S)
    {
        d1 = d >> 1;
        d2 = d << 1;

        if (d == S) // todo zrobic inaczej calkiem bo chujowe a chodzi tylko o zamiane x,y d12, d
        {
            PlaceWallCorner(A2W((x + m_DirXArr[d1])), A2W((y + m_DirYArr[d])), (ECornerDir)d1, nullptr);
            PlaceWallCorner(A2W((x + m_DirXArr[d2])), A2W((y + m_DirYArr[d])), (ECornerDir)d, nullptr);
        }
        else
        {
            PlaceWallCorner(A2W((x + m_DirXArr[d])), A2W((y + m_DirYArr[d1])), (ECornerDir)d1, nullptr);
            PlaceWallCorner(A2W((x + m_DirXArr[d])), A2W((y + m_DirYArr[d2])), (ECornerDir)d, nullptr);
        }
    }
    else
    {
        d1 = open_dir >> 1; // workaround cause EDir ends on 8 bits
        d2 = open_dir << 1;

        if (d == N) // todo zrobic inaczej calkiem bo chujowe a chodzi tylko o zamiane x,y d12, d
        {
            PlaceWallCorner(A2W((x + m_DirXArr[d1])), A2W((y + m_DirYArr[d])), (ECornerDir)d, nullptr);
            PlaceWallCorner(A2W((x + m_DirXArr[d2])), A2W((y + m_DirYArr[d])), (ECornerDir)d2, nullptr);
        }
        else
        {
            PlaceWallCorner(A2W((x + m_DirXArr[d])), A2W((y + m_DirYArr[d1])), (ECornerDir)d, nullptr);
            PlaceWallCorner(A2W((x + m_DirXArr[d])), A2W((y + m_DirYArr[d2])), (ECornerDir)d2, nullptr);
        }
    }
    //////////////////////////////////////////////////////////////////////////

    // place walls
    //////////////////////////////////////////////////////////////////////////
    PlaceWall(A2W((x + m_DirXArr[d])), A2W((y + m_DirYArr[d])), (EDir)d, nullptr);
    PlaceWall(A2W((x + m_DirXArr[d1])), A2W((y + m_DirYArr[d1])), (EDir)d1, nullptr);
    PlaceWall(A2W((x + m_DirXArr[d2])), A2W((y + m_DirYArr[d2])), (EDir)d2, nullptr);
}

void ADungeonGenerator::PlaceCrossRoad(int x, int y)
{

}

void ADungeonGenerator::PlaceTCrossRoad(int x, int y, int open_dirs)
{

}

// Walls
void ADungeonGenerator::PlaceWall(int x, int y, EDir dir, const TSubclassOf<AWall>* wall /*= nullptr*/)
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

void ADungeonGenerator::PlaceWallCorner(int x, int y, ECornerDir dir, const TSubclassOf<AWall>* wall /*= nullptr*/)
{
	float fx = (float)x;
	float fy = (float)y;

	switch (dir)
	{
	case NW:
		SpawnWallCorner(fx, fy, FRotator::ZeroRotator, wall);
		break;
	case NE:
		SpawnWallCorner(fx, fy, FRotator(0.0f, 90.0f, 0.0f), wall);
		break;
	case SW:
		SpawnWallCorner(fx, fy, FRotator(0.0f, 270.0f, 0.0f), wall);
		break;
	case SE:
		SpawnWallCorner(fx, fy, FRotator(0.0f, 180.0f, 0.0f), wall);
		break;
	}
}

// Spawners
//////////////////////////////////////////////////////////////////////////
AActor* ADungeonGenerator::SpawnWall(float x, float y, FRotator rot, const TSubclassOf<AWall>* wall /*= nullptr*/)
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
			if (m_TorchIncrementator % m_TorchModulo != 0)
				newActor = GetWorld()->SpawnActor<AWall>(m_Wall0, newVec, rot, spawnParams);
			else
				newActor = GetWorld()->SpawnActor<AWall>(m_Wall1, newVec, rot, spawnParams);
		}

		newActor->SetActorScale3D(SCALE_VEC); // set object scale
	}

	m_TorchIncrementator++;
	return newActor;
}

AActor* ADungeonGenerator::SpawnWallCorner(float x, float y, FRotator rot, const TSubclassOf<AWall>* wall /*= nullptr*/)
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
			newActor = GetWorld()->SpawnActor<AWall>(m_WallCorner1_In, newVec, rot, spawnParams);

		newActor->SetActorScale3D(SCALE_VEC); // set object scale
	}

	return newActor;
}

void ADungeonGenerator::SpawnPlayerInRoom(const SRoom& room)
{
	ACharacter* myCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (myCharacter)
	{
		float x = (float)room.PosX + ((float)room.SizeX / 2.0f);
		float y = (float)room.PosY + ((float)room.SizeY / 2.0f);

		myCharacter->SetActorLocation(FVector(A2W(x), A2W(y), 200.0f));
	}
}

// Others
//////////////////////////////////////////////////////////////////////////
int ADungeonGenerator::NumberOfSetBits(int i)
{
	i = i - ((i >> 1) & 0x55555555);
	i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
	return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

int ADungeonGenerator::shuffle_array(int *arr, int size)
{
    int i;

    for (i = 0; i < (size - 1); i++) {
        int r = i + (rand() % (size - i));
        int temp = arr[i];
        arr[i] = arr[r];
        arr[r] = temp;
    }
    return 0;
}

int ADungeonGenerator::carve_passage(int cx, int cy)
{

    int dx, dy, nx, ny;
    int directions[4] = { N, E, S, W };

    //shuffle the direction array
    int i;

    for (i = 0; i < (4 - 1); i++) {
        int r = i + (rand() % (4 - i));
        int temp = directions[i];
        directions[i] = directions[r];
        directions[r] = temp;
    }

    //iterates through the direction then test if the cell in that direction is valid and
    //within the bounds of the maze
    for (i = 0; i < 4; i++) {
        dx = m_DirXArr[directions[i]];
        dy = m_DirYArr[directions[i]];

        // check if the cell is valid
        nx = cx + dx;
        ny = cy + dy;
        // check if we are on valid grid
        if (((nx < m_MaxWidth) & (nx >= 0)) & ((ny < m_MaxHeight) & (ny >= 0))) {
            //check if grid is not visited
            if (m_MazeArr[nx][ny] == 0) {
                m_MazeArr[cx][cy] = (int)((int)m_MazeArr[cx][cy] | (int)directions[i]);
                m_MazeArr[nx][ny] = (int)((int)m_MazeArr[nx][ny] | (int)m_DirOppositeArr[directions[i]]);
                carve_passage(nx, ny);
            }
        }

    }
    return 0;
}



