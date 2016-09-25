// Fill out your copyright notice in the Description page of Project Settings.
#include "UE_Dungeon.h"
#include "DungeonGenerator.h"


#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>

// TODO zrobic porz¹dek z kierunkami N ma byc do góry!!
// std::wstringstream ws; //debug
// ws << closed_dir;
// OutputDebugString(ws.str().c_str());

// Sets default values
ADungeonGenerator::ADungeonGenerator() :
    m_MaxWidth(50), m_MaxHeight(50),
    m_TorchIncrementator(0),
    m_TorchModulo(FMath::RandRange(3, 8))
{
    PrimaryActorTick.bCanEverTick = true;// /*true*/;
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

    for (SRoom room : m_RoomsVec)
        PlaceRoom(room);

    // Maze
    //////////////////////////////////////////////////////////////////////////
    //GenMazeGrowingTreeAlgorithm(4, 4);//(int)m_RoomsVec[0].RoomDoors[0].OutsideDoor.X, (int)m_RoomsVec[0].RoomDoors[0].OutsideDoor.Y);
    //////////////////////////////////////////////////////////////////////////

    // AStar
    //////////////////////////////////////////////////////////////////////////
    //GenerateMinimumSpanningTree();
    //for (SRoom r : m_RoomsVec)
    //{
    //    for (SDoor d : r.RoomDoors)
    //    {
    //        if (d.SpanningTreeDoor)
    //        {
    //            FindAStarPaths(&d, d.SpanningTreeDoor);
    //        }
    //    }
    //}
    //////////////////////////////////////////////////////////////////////////


    
    // Place passages
    //////////////////////////////////////////////////////////////////////////
//     for (int i = 0; i < m_MaxWidth; i++)
//     {
//         for (int j = 0; j < m_MaxHeight; j++)
//         {
//             if (m_MazeArr[i][j] != Room)
//                 if (m_MazeArr[i][j])
//                     PlacePassage(i, j, m_MazeArr[i][j]);
//         }
//     }
    //////////////////////////////////////////////////////////////////////////



    // DrawDebugBoxes();
    DrawDebugStrings();

    SpawnPlayerInRoom(m_RoomsVec[0]);
}

// Called every frame
void ADungeonGenerator::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    static std::vector <SCell> cells;
    static bool boo = true;
    if (boo)
    {
        cells.push_back(GenSCell(4, 4));
        boo = false;
    }

   
    if (cells.size() > 0)
    {
        if (cells.back().unvisited.size() == 0)
        {
            cells.pop_back();
            return;
        }

        SPoint pos = cells.back().pos;
        SPoint npos = cells.back().unvisited.back();
        cells.back().unvisited.pop_back();

        if (m_MazeArr[npos.X][npos.Y] != Nothing)
            return;

        int dir = 0;
        int old_dir = 0;
        if (pos.X != npos.X)
        {
            dir |= pos.X > npos.X ? W : E;
            old_dir |= pos.X < npos.X ? W : E;
        }
        if (pos.Y != npos.Y)
        {
            dir |= pos.Y > npos.Y ? N : S;
            old_dir |= pos.Y < npos.Y ? N : S;
        }

        m_MazeArr[pos.X][pos.Y] |= dir | old_dir; // old_dir opposite
         //m_MazeArr[npos.X][npos.Y] |= old_dir/* | old_dir*/;

        // Debug
        //////////////////////////////////////////////////////////////////////////
        DrawDebugSolidBox(GetWorld(), FVector(A2W(pos.X), A2W(pos.Y), 300.0), FVector(TILE_SIZE / 2.0, TILE_SIZE / 2.0, TILE_SIZE / 2.0), FColor(0, 0, 255, 155), true);
        //////////////////////////////////////////////////////////////////////////
        int dirs = m_MazeArr[pos.X][pos.Y];
        FString str("");

        if (dirs & N)
            str += "|N";
        if (dirs & S)
            str += "|S";
        if (dirs & W)
            str += "|W";
        if (dirs & E)
            str += "|E";

        DrawDebugString(GetWorld(), FVector(A2W(pos.X), A2W(pos.Y), 300.0), str, nullptr, FColor::White, 10000.0F);
        //////////////////////////////////////////////////////////////////////////

        cells.push_back(GenSCell(npos.X, npos.Y));
        old_dir = dir;
    }
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
    m_DirXArr[1] = -1;
    m_DirXArr[N] = 0;
    m_DirXArr[E] = 1;
    m_DirXArr[S] = 0;
    m_DirXArr[W] = -1;
    m_DirXArr[32] = 0;

    m_DirYArr[1] = 0;
    m_DirYArr[N] = -1;
    m_DirYArr[E] = 0;
    m_DirYArr[S] = 1;
    m_DirYArr[W] = 0;
    m_DirYArr[32] = -1;

    m_DirOppositeArr[1] = E;
    m_DirOppositeArr[N] = S;
    m_DirOppositeArr[E] = W;
    m_DirOppositeArr[S] = N;
    m_DirOppositeArr[W] = E;
    m_DirOppositeArr[32] = S;
}

void ADungeonGenerator::ClearMazeArray()
{
    for (int i = 0; i < m_MaxWidth; i++)
    {
        for (int j = 0; j < m_MaxHeight; j++)
        {
            m_MazeArr[i][j] = Nothing; // SolidRock; //0;//SolidRock todo obadac
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
}


void ADungeonGenerator::AddDoors(int x, int y, EDir dir, SRoom& room)
{
    m_MazeArr[x][y] = Doors; // inside
    room.RoomDoors.push_back(SDoor(FVector2D(x, y), dir));

    SDoor d = room.RoomDoors.back();
    int nx = (int)d.OutsideDoor.X;
    int ny = (int)d.OutsideDoor.Y;
    m_MazeArr[nx][ny] = Doors; // outside
    m_MazeArr[nx + m_DirXArr[dir >> 1]][ny + m_DirYArr[dir >> 1]] = RoomWall; // outside
    m_MazeArr[nx + m_DirXArr[dir << 1]][ny + m_DirYArr[dir << 1]] = RoomWall; // outside
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
        if (m_MazeArr[nx + 1][ny] == Doors) // if there are doors, repeat and find another spot
            continue;
        if (m_MazeArr[nx - 1][ny] == Doors) // if there are doors, repeat and find another spot
            continue;
        if (m_MazeArr[nx][ny + 1] == Doors) // if there are doors, repeat and find another spot
            continue;
        if (m_MazeArr[nx][ny - 1] == Doors) // if there are doors, repeat and find another spot
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

    for (int i = -1; i < size_x + 1; i++)
    {
        m_MazeArr[x + i][y - 1] = RoomWall;
        m_MazeArr[x + i][y + size_y] = RoomWall;
    }

    for (int i = -1; i < size_y + 1; i++)
    {
        m_MazeArr[x - 1][y + i] = RoomWall;
        m_MazeArr[x + size_x][y + i] = RoomWall;
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
            if (m_MazeArr[nx + i][ny + j] != Nothing) //solidrock
                return false;
        }
    }

    return true;
}

// Maze Gen
//////////////////////////////////////////////////////////////////////////
void ADungeonGenerator::GenMazeGrowingTreeAlgorithm(int start_x, int start_y)
{
    std::vector <SCell> cells;
    cells.push_back(GenSCell(start_x, start_y));

    while(cells.size() > 0)
    {
        if (cells.back().unvisited.size() == 0)
        {
            cells.pop_back();
            continue;
        }

        SPoint pos = cells.back().pos;
        SPoint npos = cells.back().unvisited.back();
        cells.back().unvisited.pop_back();

        if (m_MazeArr[npos.X][npos.Y] != Nothing)
            continue;

        int dir = 0;
        int old_dir = 0;
        if (pos.X != npos.X)
        {
            dir |= pos.X > npos.X ? W : E;
            old_dir |= pos.X < npos.X ? W : E;
        }
        else if (pos.Y != npos.Y)
        {
            dir |= pos.Y > npos.Y ? N : S;
            old_dir |= pos.Y < npos.Y ? N : S;
        }

        m_MazeArr[pos.X][pos.Y] |= dir; // old_dir opposite
        m_MazeArr[npos.X][npos.Y] |= old_dir/* | old_dir*/;

        cells.push_back(GenSCell(npos.X, npos.Y));
    }



//     std::vector <SCell> cells;
//     cells.push_back(GenSCell(start_x, start_y));
// 
//     while (cells.size() > 0)
//     {
//         if (cells.back().unvisited.size() == 0)
//         {
//             cells.pop_back();
//             continue;
//         }
// 
//         SPoint pos = cells.back().pos;
//         SPoint npos = cells.back().unvisited.back();
//         cells.back().unvisited.pop_back();
// 
//         int dir = 0;
//         int old_dir = 0;
//         if (pos.X != npos.X)
//         {
//             dir |= pos.X > npos.X ? W : E;
//             old_dir |= pos.X < npos.X ? W : E;
//         }
//         else if (pos.Y != npos.Y)
//         {
//             dir |= pos.Y > npos.Y ? N : S;
//             old_dir |= pos.Y < npos.Y ? N : S;
//         }
// 
//         m_MazeArr[pos.X][pos.Y] |= dir; // old_dir opposite
//        // m_MazeArr[npos.X][npos.Y] |= old_dir/* | old_dir*/;
// 
//         cells.push_back(GenSCell(npos.X, npos.Y));
//     }



}

SCell ADungeonGenerator::GenSCell(int x, int y)
{
    SCell cell(x, y);

    static int arr[4] = { 0, 1, 2, 3 };

    for (int i = 0; i < (4 - 1); i++) {
        int r = i + (rand() % (4 - i));
        int temp = arr[i];
        arr[i] = arr[r];
        arr[r] = temp;
    }

    for (int i = 0; i < 4; i++)
    {
        switch (arr[i])
        {
        case 0:
        {
            if (x > 1)
                if (m_MazeArr[x - 1][y] == Nothing)
                    if(CheckNeighbours(x - 1, y, Nothing) > 3)
                        cell.unvisited.push_back(SPoint(x - 1, y));
            continue;
        }

        case 1:
        {
            if (x < m_MaxWidth - 2)
                if (m_MazeArr[x + 1][y] == Nothing)
                    if (CheckNeighbours(x + 1, y, Nothing) > 3)
                    cell.unvisited.push_back(SPoint(x + 1, y));
            continue;
        }
        case 2:
        {
            if (y > 1)
                if (m_MazeArr[x][y - 1] == Nothing)
                    if (CheckNeighbours(x, y - 1, Nothing) > 3)
                    cell.unvisited.push_back(SPoint(x, y - 1));
            continue;
        }
        case 3:
        {
            if (y < m_MaxHeight - 2)
                if (m_MazeArr[x][y + 1] == Nothing)
                    if (CheckNeighbours(x, y + 1, Nothing) > 3)
                    cell.unvisited.push_back(SPoint(x, y + 1));
            continue;
        }
        }
    }

    return cell;
}

void ADungeonGenerator::CarveCorridorsBetweenRooms(int attempts /*= 0*/)
{
    if (attempts > 0)
    {
        int nx, ny;
        for (int i = 0; i < attempts; i++)
        {
            nx = FMath::RandRange(0, m_MaxWidth - 1);
            ny = FMath::RandRange(0, m_MaxHeight - 1);
            GenMazeRecursiveBacktracking(nx, ny);
        }
    }
    else
    {
        for (int i = 1; i < m_MaxWidth - 1; i += 2)
        {
            for (int j = 1; j < m_MaxHeight - 1; j += 2)
            {
                if (CheckNeighbours(i, j, SolidRock) > 2)
                {
                    GenMazeRecursiveBacktracking(i, j);
                }
            }
        }
    }
}

// AStar
//////////////////////////////////////////////////////////////////////////
void ADungeonGenerator::GenerateMinimumSpanningTree()
{
    int rooms_count = m_RoomsVec.size();
    for (int i = 0; i < rooms_count; i++)
    {
        for (SDoor& d : m_RoomsVec[i].RoomDoors)
        {
            // 			if (d.SpanningTreeDoor)
            // 				continue;

            float distance = 100000000.0;

            for (int j = i + 1; j < rooms_count; j++)
            {
                for (SDoor& d2 : m_RoomsVec[j].RoomDoors)
                {
                    float temp = FVector2D::Distance(d.OutsideDoor, d2.OutsideDoor);
                    if (temp < distance)
                    {
                        distance = temp;
                        d.SpanningLength = temp;
                        d2.SpanningLength = temp;

                        // 						if (d.SpanningTreeDoor)
                        // 							d.SpanningTreeDoor->SpanningTreeDoor = nullptr;

                        d.SpanningTreeDoor = &d2;
                        d2.SpanningTreeDoor = &d;
                    }
                }
            }
        }
    }

    //#DEBUG_CLEAN
// 	for (int i = 0; i < rooms_count; i++)
// 	{
// 		int count = 0;
// 		for (SDoor d : m_RoomsVec[i].RoomDoors)
// 		{
// 			count++;
// 			std::wstringstream ws;
// 			ws << count << ". " << d.SpanningLength;
// 
// 			if (!d.SpanningTreeDoor)
// 			{
// 				ws << " szatan";				
// 			}
// 
// 			OutputDebugString(ws.str().c_str());
// 		}
// 	}
}

void ADungeonGenerator::CreatePathsBetweenRooms()
{
    for (int i = 0; i < m_RoomsVec.size(); i++)
    {
        for (const SDoor& d : m_RoomsVec[i].RoomDoors)
        {

            if (!d.SpanningTreeDoor)
                continue;

            //FindAStarPaths(d, *d.SpanningTreeDoor);
        }
    }
}

void ADungeonGenerator::FindAStarPaths(SDoor* d1, SDoor* d2)
{
    if (!d1 || !d2)
        return;

    SPath* current;
    SPoint start_pos(d1->OutsideDoor.X + m_DirXArr[d1->InsideDoorDir] * 2, d1->OutsideDoor.Y + m_DirYArr[d1->InsideDoorDir] * 2); //todo nie wime czy dziala
    SPoint end_pos(d2->OutsideDoor.X + m_DirXArr[d2->InsideDoorDir] * 2, d2->OutsideDoor.Y + m_DirYArr[d2->InsideDoorDir] * 2);

    m_MazeArr[start_pos.X][start_pos.Y] = RoomWall;
    m_MazeArr[end_pos.X][end_pos.Y] = RoomWall;

    //m_MazeArr[(int)d1->OutsideDoor.X + m_DirXArr[d1->InsideDoorDir]][(int)d1->OutsideDoor.Y + m_DirYArr[d1->InsideDoorDir]] = d1->InsideDoorDir | d1->OutsideDoorDir; //todo zobaczyc


    std::list <SPath*> open_list;
    std::list <SPath*> closed_list;
    open_list.push_back(new SPath(start_pos, GetAStarG(start_pos, end_pos), 0));
    current = open_list.front();
    current->f = current->g + current->h;

    int temp = 0;
    while (true)
    {
        SPath* old_cur = current;
        current = open_list.front();
        for (SPath* p : open_list)
        {
            if (p->f < current->f)
                current = p;
        }

        if (current->pos == end_pos)
            break;

        closed_list.push_back(old_cur);
        open_list.remove(current);

        //////////////////////////////////////////////////////////////////////////
        SPoint pos = current->pos;
        pos.X++;
        if (pos.X < m_MaxWidth)
            CheckNeighbours(current, pos, end_pos, open_list, closed_list);
        //////////////////////////////////////////////////////////////////////////
        pos.X--;
        pos.Y++;
        if (pos.Y < m_MaxHeight)
            CheckNeighbours(current, pos, end_pos, open_list, closed_list);
        //////////////////////////////////////////////////////////////////////////
        pos.X--;
        pos.Y--;
        if (pos.X >= 0)
            CheckNeighbours(current, pos, end_pos, open_list, closed_list);
        //////////////////////////////////////////////////////////////////////////
        pos.X++;
        pos.Y--;
        if (pos.Y >= 0)
            CheckNeighbours(current, pos, end_pos, open_list, closed_list);
        //////////////////////////////////////////////////////////////////////////

        if (open_list.size() == 0)
            break;
    }

    SPath* old_cur = current;
    current = current->parent;
    // int old_dir = d2->OutsideDoorDir;
    int dir = 0;
    int old_dir = 0;

    while (current)
    {
        dir = 0;
        old_dir = 0;



        if (old_cur->pos.X != current->pos.X)
        {
            dir |= old_cur->pos.X > current->pos.X ? W : E;
            old_dir |= old_cur->pos.X < current->pos.X ? W : E;
        }
        if (old_cur->pos.Y != current->pos.Y)
        {
            dir |= old_cur->pos.Y > current->pos.Y ? N : S;
            old_dir |= old_cur->pos.Y < current->pos.Y ? N : S;
        }

        //if (dir == m_DirOppositeArr[old_dir])
       //  {
        m_MazeArr[old_cur->pos.X][old_cur->pos.Y] |= old_dir;
        m_MazeArr[current->pos.X][current->pos.Y] |= dir;
        //  }
        old_cur = current;
        current = current->parent;
        //old_dir = dir;
    }

    //m_MazeArr[old_cur->pos.X][old_cur->pos.Y] = dir | d2->OutsideDoorDir; //|= d2->InsideDoorDir;


    m_MazeArr[old_cur->pos.X][old_cur->pos.Y] |= d2->InsideDoorDir;
    m_MazeArr[end_pos.X][end_pos.Y] = old_dir | d2->OutsideDoorDir;//old_dir /*d2->InsideDoorDir*/ | d2->OutsideDoorDir;
// 	for (SPath* p : open_list) //todo obadac czemu crashe
// 		if (p)
// 			delete p;
// 
// 	for (SPath* p : closed_list)
// 		if (p)
// 			delete p;
}

int ADungeonGenerator::GetAStarG(const SPoint& start, const SPoint& end)
{
    int x = std::abs(start.X - end.X);
    int y = std::abs(start.Y - end.Y);
    return (x + y) * 10;
}

void ADungeonGenerator::CheckNeighbours(SPath* cur, const SPoint& start, const SPoint& end, std::list <SPath*>& open_list, std::list<SPath*>& closed_list)
{
    //SPoint cur_pos = cur->pos;
    SPoint pos = start;
    if ((m_MazeArr[pos.X][pos.Y] != Room && m_MazeArr[pos.X][pos.Y] != RoomWall) || pos == end)
    {
        if (m_MazeArr[pos.X][pos.Y] == Doors)
            return;

        SPath* path = new SPath(pos, 0, 0, cur);
        if (std::find_if(closed_list.begin(), closed_list.end(), [=](SPath* p) { return p->pos == path->pos; }) == closed_list.end())
        {
            if (std::find_if(open_list.begin(), open_list.end(), [=](SPath* p) { return p->pos == path->pos; }) == open_list.end())
            {
                path->g = GetAStarG(pos, end);
                path->h = m_MazeArr[pos.X][pos.Y] == Nothing ? cur->h + 10 : cur->h;
                path->f = path->g + path->h;
                path->parent = cur;
                open_list.push_back(path);
            }
            //else // mozna porownac starego rodzica czy aktualny nei ejst krotszy ale czy ja napewnoc chce najkrotsza??
            else
            {
                delete path;
            }
        }
        else
        {
            delete path;
        }
    }
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
    PlaceWallCorner(n0, n1, NW);
    n0 = A2W((room.PosX - 1));
    n1 = A2W((room.PosY + room.SizeY));
    PlaceWallCorner(n0, n1, SW);
    n0 = A2W((room.PosX + room.SizeX));
    n1 = A2W((room.PosY - 1));
    PlaceWallCorner(n0, n1, NE);
    n0 = A2W((room.PosX + room.SizeX));
    n1 = A2W((room.PosY + room.SizeY));
    PlaceWallCorner(n0, n1, SE);

    // Place Outside Doors
    for (SDoor door : room.RoomDoors) //to do calkie inaczej to zrobic, ogolnie caly kod ten jest mega chujowy, nie podoba mis ie w chuj
    {
        PlaceWall(A2W(door.OutsideDoor.X), A2W(door.OutsideDoor.Y), door.OutsideDoorDir, &m_WallDoorOpen);

        if (door.OutsideDoorDir == N)
        {
            PlaceWallCorner(A2W((door.OutsideDoor.X - 1./*4*/)), A2W(door.OutsideDoor.Y), NW);
            PlaceWallCorner(A2W((door.OutsideDoor.X + 1./*4*/)), A2W(door.OutsideDoor.Y), NE);
        }
        else if (door.OutsideDoorDir == S)
        {
            PlaceWallCorner(A2W((static_cast<float>(door.OutsideDoor.X) + 1./*4*/)), A2W(door.OutsideDoor.Y), SE);
            PlaceWallCorner(A2W((static_cast<float>(door.OutsideDoor.X) - 1./*4*/)), A2W(door.OutsideDoor.Y), SW); //r
        }
        else if (door.OutsideDoorDir == W)
        {
            PlaceWallCorner(A2W(door.OutsideDoor.X), A2W((static_cast<float>(door.OutsideDoor.Y) - 1./*4*/)), NW);
            PlaceWallCorner(A2W(door.OutsideDoor.X), A2W((static_cast<float>(door.OutsideDoor.Y) + 1./*4*/)), SW);
        }
        else
        {
            PlaceWallCorner(A2W(door.OutsideDoor.X), A2W((static_cast<float>(door.OutsideDoor.Y) - 1./*4*/)), NE);
            PlaceWallCorner(A2W(door.OutsideDoor.X), A2W((static_cast<float>(door.OutsideDoor.Y) + 1./*4*/)), SE); //r
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
        PlaceTCrossRoad(x, y, dirs);
        break;
    case 4:
        PlaceCrossRoad(x, y);
        break;
    default:
        break;
    }
}

void ADungeonGenerator::PlaceCorridor(int x, int y, int open_dirs) //todo chujoza totalnie wymienic
{
    int dir1 = 0;
    int dir2 = 0;
    for (int i = 0; i < 6; i++)
    {
        int temp_bit = (open_dirs >> i) & 1;
        if (temp_bit != 0)
        {
            dir1 == 0 ? dir1 |= 1 << i : dir2 |= 1 << i;
        }
    }

    if (m_DirOppositeArr[dir1] == dir2) // if straight corridor
    {
        if (dir1 == W || dir1 == E)
            SpawnWall(A2W(x), A2W(y), FRotator::ZeroRotator, &m_Corridor);
        else
            SpawnWall(A2W(x), A2W(y), FRotator(0.0f, 90.0f, 0.0f), &m_Corridor);

        // 		int nd1 = dir1 >> 1;
        // 		int nd2 = dir2 >> 1;
        // 		PlaceWall(A2W((x + m_DirXArr[nd1])), A2W((y + m_DirYArr[nd1])), nd1, nullptr);
        // 		PlaceWall(A2W((x + m_DirXArr[nd2])), A2W((y + m_DirYArr[nd2])), nd2, nullptr);
    }
    else
    {
        if ((open_dirs & (N | W)) == (N | W))
        {
            SpawnWall(A2W(x), A2W(y), FRotator(0.0f, 270.0f, 0.0f), &m_Turn); //ok
        }
        else if ((open_dirs & (N | E)) == (N | E))
        {
            SpawnWall(A2W(x), A2W(y), FRotator::ZeroRotator, &m_Turn);
        }
        else if ((open_dirs & (S | W)) == (S | W))
        {
            SpawnWall(A2W(x), A2W(y), FRotator(0.0f, 180.0f, 0.0f), &m_Turn);
        }
        else if ((open_dirs & (S | E)) == (S | E))
        {
            SpawnWall(A2W(x), A2W(y), FRotator(0.0f, 90.0f, 0.0f), &m_Turn);//ok
        }

        // 		/* Ten if jest spowodowany tym ze dir1 zawsze bedzie posiadal mniejsza wartosc, w przypadku zakretu N|W weszloby tutaj i bylo zle a powinno powyzej */
        // 		// todo znalezc sposob by to obejsc
        // 		if (dir1 == N && dir2 == W)
        // 		{
        // 			int nd1 = dir1;
        // 			int nd2 = dir2;
        // 			PlaceWall(A2W((x + m_DirXArr[nd1])), A2W((y + m_DirYArr[nd1])), nd1, nullptr);
        // 			PlaceWall(A2W((x + m_DirXArr[nd2])), A2W((y + m_DirYArr[nd2])), nd2, nullptr);
        // 			PlaceWallCorner(A2W((x + m_DirXArr[nd2])), A2W((y + m_DirYArr[nd1])), nd1, nullptr);
        // 			PlaceWallCorner(A2W((x + m_DirXArr[m_DirOppositeArr[nd2]])), A2W((y + m_DirYArr[m_DirOppositeArr[nd1]])), m_DirOppositeArr[nd1], &m_WallCorner2_Out);
        // 			PlaceWall(A2W((x + m_DirXArr[nd1 << 1])), A2W((y + m_DirYArr[nd1])), nd1, nullptr);
        // 			PlaceWall(A2W((x + m_DirXArr[nd2])), A2W((y + m_DirYArr[nd2 >> 1])), nd2, nullptr);
        // 		}
        // 		else  // todo fix that shit: we need to remove this fucking if
        // 		{
        // 			int nd1 = dir1 << 1;
        // 			int nd2 = dir2 >> 1;
        // 			PlaceWall(A2W((x + m_DirXArr[nd1])), A2W((y + m_DirYArr[nd1])), nd1, nullptr);
        // 			PlaceWall(A2W((x + m_DirXArr[nd2])), A2W((y + m_DirYArr[nd2])), nd2, nullptr);
        // 
        // 			if (dir1 == E || dir1 == W)
        // 			{
        // 	 			PlaceWallCorner(A2W((x + m_DirXArr[nd2])), A2W((y + m_DirYArr[nd1])), nd1, nullptr);
        // 	 			PlaceWallCorner(A2W((x + m_DirXArr[m_DirOppositeArr[nd2]])), A2W((y + m_DirYArr[m_DirOppositeArr[nd1]])), m_DirOppositeArr[nd1], &m_WallCorner2_Out);
        // 				PlaceWall(A2W((x + m_DirXArr[nd1 << 1])), A2W((y + m_DirYArr[nd1])), nd1, nullptr);
        // 				PlaceWall(A2W((x + m_DirXArr[nd2])), A2W((y + m_DirYArr[nd2 >> 1])), nd2, nullptr);
        // 			}
        // 			else
        // 			{
        // 				
        //   				PlaceWallCorner(A2W((x + m_DirXArr[nd1])), A2W((y + m_DirYArr[nd2])), nd1, nullptr);
        //   				PlaceWallCorner(A2W((x + m_DirXArr[m_DirOppositeArr[nd1]])), A2W((y + m_DirYArr[m_DirOppositeArr[nd2]])), m_DirOppositeArr[nd1], &m_WallCorner2_Out);
        // 				PlaceWall(A2W((x + m_DirXArr[nd1])), A2W((y + m_DirYArr[nd1 << 1])), nd1, nullptr);
        // 				PlaceWall(A2W((x + m_DirXArr[nd2 >> 1])), A2W((y + m_DirYArr[nd2])), nd2, nullptr);
        // 			}
        // 		}

    }
}

void ADungeonGenerator::PlaceDeadEnd(int x, int y, int open_dir)
{
    float fx = (float)x;
    float fy = (float)y;
    fx = A2W(fx);
    fy = A2W(fy);

    switch (open_dir)
    {
    case N:
        SpawnWall(fx, fy, FRotator::ZeroRotator, &m_DeadEnd);
        return;
    case S:
        SpawnWall(fx, fy, FRotator(0.0f, 180.0f, 0.0f), &m_DeadEnd);
        return;
    case W:
        SpawnWall(fx, fy, FRotator(0.0f, 270.0f, 0.0f), &m_DeadEnd);
        return;
    case E:
        SpawnWall(fx, fy, FRotator(0.0f, 90.0f, 0.0f), &m_DeadEnd);
        return;
    }

    //     int d = m_DirOppositeArr[open_dir];
    //     int d1, d2;
    //     d1 = d >> 1;
    //     d2 = d << 1;
    // 
    //     // place walls
    //     //////////////////////////////////////////////////////////////////////////
    //     PlaceWall(A2W((x + m_DirXArr[d])), A2W((y + m_DirYArr[d])), d, nullptr);
    //     PlaceWall(A2W((x + m_DirXArr[d1])), A2W((y + m_DirYArr[d1])), d1, nullptr);
    //     PlaceWall(A2W((x + m_DirXArr[d2])), A2W((y + m_DirYArr[d2])), d2, nullptr);
    // 
    // 	// place corners
    // 	//////////////////////////////////////////////////////////////////////////
    // 	if (d == E || d == W) // todo fix that shit: we need to remove this fucking if
    // 	{
    // 		PlaceWallCorner(A2W((x + m_DirXArr[d])), A2W((y + m_DirYArr[d1])), d, nullptr);
    // 		PlaceWallCorner(A2W((x + m_DirXArr[d])), A2W((y + m_DirYArr[d2])), d2, nullptr);
    // 	}
    // 	else
    // 	{
    // 		PlaceWallCorner(A2W((x + m_DirXArr[d1])), A2W((y + m_DirYArr[d])), d, nullptr);
    // 		PlaceWallCorner(A2W((x + m_DirXArr[d2])), A2W((y + m_DirYArr[d])), d2, nullptr);
    // 	}
}

void ADungeonGenerator::PlaceCrossRoad(int x, int y)
{
    SpawnWall(A2W(x), A2W(y), FRotator::ZeroRotator, &m_Cross);
    // 	PlaceWallCorner(A2W((x + m_DirXArr[NE])), A2W((y + m_DirYArr[NE << 1])), SE, &m_WallCorner2_Out);
    // 	PlaceWallCorner(A2W((x + m_DirXArr[SE << 1])), A2W((y + m_DirYArr[SE])), SW, &m_WallCorner2_Out);
    // 	PlaceWallCorner(A2W((x + m_DirXArr[SW])), A2W((y + m_DirYArr[SW << 1])), NW, &m_WallCorner2_Out);
    // 	PlaceWallCorner(A2W((x + m_DirXArr[NW << 1])), A2W((y + m_DirYArr[NW])), NE, &m_WallCorner2_Out);
}

void ADungeonGenerator::PlaceTCrossRoad(int x, int y, int open_dirs)
{
    if ((open_dirs & (N | S | W)) == (N | S | W))
    {
        SpawnWall(A2W(x), A2W(y), FRotator(0.0f, 270.0f, 0.0f), &m_T_Cross); //ok
    }
    else if ((open_dirs & (N | S | E)) == (N | S | E))
    {
        SpawnWall(A2W(x), A2W(y), FRotator(0.0f, 90.0f, 0.0f), &m_T_Cross);
    }
    else if ((open_dirs & (E | W | S)) == (E | W | S))
    {
        SpawnWall(A2W(x), A2W(y), FRotator::ZeroRotator, &m_T_Cross);
    }
    else if ((open_dirs & (E | W | N)) == (E | W | N))
    {
        SpawnWall(A2W(x), A2W(y), FRotator(0.0f, 180.0f, 0.0f), &m_T_Cross);//ok
    }




    // 	int closed_dir = 0;
    // 	for (int i = 1; i < 5; i++)
    // 	{
    // 		int temp_bit = (open_dirs >> i) & 1;
    // 		if (temp_bit == 0)
    // 		{
    // 			closed_dir |= 1 << i;
    // 		}

    // 		if (closed_dir == E || closed_dir == W)
    // 		{
    // 			PlaceWall(A2W((x + m_DirXArr[closed_dir])), A2W((y + m_DirYArr[closed_dir])), closed_dir, nullptr);
    // 			PlaceWall(A2W((x + m_DirXArr[closed_dir])), A2W((y + m_DirYArr[closed_dir >> 1])), closed_dir, nullptr);
    // 			PlaceWall(A2W((x + m_DirXArr[closed_dir])), A2W((y + m_DirYArr[closed_dir << 1])), closed_dir, nullptr);
    // 			PlaceWallCorner(A2W((x + m_DirXArr[m_DirOppositeArr[closed_dir]])), A2W((y + m_DirYArr[closed_dir >> 1])), closed_dir >> 1, &m_WallCorner2_Out);
    // 			PlaceWallCorner(A2W((x + m_DirXArr[m_DirOppositeArr[closed_dir]])), A2W((y + m_DirYArr[closed_dir << 1])), m_DirOppositeArr[closed_dir], &m_WallCorner2_Out);
    // 
    // 		}
    // 		else
    // 		{
    // 			PlaceWall(A2W((x + m_DirXArr[closed_dir])), A2W((y + m_DirYArr[closed_dir])), closed_dir, nullptr);
    // 			PlaceWall(A2W((x + m_DirXArr[closed_dir >> 1])), A2W((y + m_DirYArr[closed_dir])), closed_dir, nullptr);
    // 			PlaceWall(A2W((x + m_DirXArr[closed_dir << 1])), A2W((y + m_DirYArr[closed_dir])), closed_dir, nullptr);
    // 			PlaceWallCorner(A2W((x + m_DirXArr[closed_dir >> 1])), A2W((y + m_DirYArr[m_DirOppositeArr[closed_dir]])), closed_dir >> 1, &m_WallCorner2_Out);
    // 			PlaceWallCorner(A2W((x + m_DirXArr[closed_dir << 1])), A2W((y + m_DirYArr[m_DirOppositeArr[closed_dir]])), m_DirOppositeArr[closed_dir], &m_WallCorner2_Out);
    // 		}
    //	}
}

// Walls
void ADungeonGenerator::PlaceWall(int x, int y, int dir, const TSubclassOf<AWall>* wall /*= nullptr*/)
{
    float fx = (float)x;
    float fy = (float)y;

    switch (dir)
    {
    case N:
        SpawnWall(fx, fy, FRotator::ZeroRotator, wall);
        return;
    case S:
        SpawnWall(fx, fy, FRotator(0.0f, 180.0f, 0.0f), wall);
        return;
    case W:
        SpawnWall(fx, fy, FRotator(0.0f, 270.0f, 0.0f), wall);
        return;
    case E:
        SpawnWall(fx, fy, FRotator(0.0f, 90.0f, 0.0f), wall);
        return;
    }

    if (dir == 1) // like w
    {
        SpawnWall(fx, fy, FRotator(0.0f, 270.0f, 0.0f), wall);
        return;
    }

    if (dir == 32) // like n
    {
        SpawnWall(fx, fy, FRotator::ZeroRotator, wall);
        return;
    }
}

void ADungeonGenerator::PlaceWallCorner(int x, int y, int dir, const TSubclassOf<AWall>* wall /*= nullptr*/)
{
    float fx = (float)x;
    float fy = (float)y;

    switch (dir)
    {
    case NW:
        SpawnWallCorner(fx, fy, FRotator(0.0f, 270.0f, 0.0f), wall);
        return;
    case NE:
        SpawnWallCorner(fx, fy, FRotator::ZeroRotator, wall);
        return;
    case SW:
        SpawnWallCorner(fx, fy, FRotator(0.0f, 180.0f, 0.0f), wall);
        return;
    case SE:
        SpawnWallCorner(fx, fy, FRotator(0.0f, 90.0f, 0.0f), wall);
        return;
    }

    if (dir == 1) // like sw
    {
        SpawnWallCorner(fx, fy, FRotator(0.0f, 180.0f, 0.0f), wall);
        return;
    }

    if (dir == 32) // like nw
    {
        SpawnWallCorner(fx, fy, FRotator(0.0f, 270.0f, 0.0f), wall);
        return;
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

        if (wall)
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

        if (wall)
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

// Debug
//////////////////////////////////////////////////////////////////////////
void ADungeonGenerator::DrawDebugBoxes()
{
    for (int i = 0; i < m_MaxWidth; i++)
    {
        for (int j = 0; j < m_MaxHeight; j++)
        {
            if (m_MazeArr[i][j] == RoomWall)
            {
                DrawDebugSolidBox(GetWorld(), FVector(A2W(i), A2W(j), 300.0), FVector(TILE_SIZE / 2.0, TILE_SIZE / 2.0, TILE_SIZE / 2.0), FColor(255, 0, 0, 155), true);
                continue;
            }
            else if (m_MazeArr[i][j] == Doors)
            {
                DrawDebugSolidBox(GetWorld(), FVector(A2W(i), A2W(j), 300.0), FVector(TILE_SIZE / 2.0, TILE_SIZE / 2.0, TILE_SIZE / 2.0), FColor(0, 255, 0, 155), true);
                continue;
            }
            else if (m_MazeArr[i][j] == Room)
            {
                continue;
            }
            else if (m_MazeArr[i][j] == Corridor)
            {
                DrawDebugSolidBox(GetWorld(), FVector(A2W(i), A2W(j), 300.0), FVector(TILE_SIZE / 2.0, TILE_SIZE / 2.0, TILE_SIZE / 2.0), FColor(200, 150, 50, 155), true);
                continue;
            }
            else if (m_MazeArr[i][j] != Nothing)
            {
                DrawDebugSolidBox(GetWorld(), FVector(A2W(i), A2W(j), 300.0), FVector(TILE_SIZE / 2.0, TILE_SIZE / 2.0, TILE_SIZE / 2.0), FColor(0, 0, 255, 155), true);
                continue;
            }
        }
    }
}

void ADungeonGenerator::DrawDebugStrings()
{
    for (int i = 0; i < m_MaxWidth; i++)
    {
        for (int j = 0; j < m_MaxHeight; j++)
        {
            if (m_MazeArr[i][j] == RoomWall)
            {
                continue;
            }
            else if (m_MazeArr[i][j] == Doors)
            {
                continue;
            }
            else if (m_MazeArr[i][j] == Room)
            {
                continue;
            }
            else if (m_MazeArr[i][j] != Nothing)
            {
                int dirs = m_MazeArr[i][j];
                FString str("");

                if (dirs & N)
                    str += "|N";
                if (dirs & S)
                    str += "|S";
                if (dirs & W)
                    str += "|W";
                if (dirs & E)
                    str += "|E";

                DrawDebugString(GetWorld(), FVector(A2W(i), A2W(j), 300.0), str, nullptr, FColor::White, 10000.0F);
                continue;
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void ADungeonGenerator::GenMazeRecursiveBacktracking(int pos_x, int pos_y)
{
    int nx, ny;
    int directions[4]{ N, S, E, W };
    for (int i = 0; i < 100; i++)
    {
        ShuffleDirArray(directions, 4);
        for (int dir : directions)
        {
            nx = pos_x + m_DirXArr[dir];
            ny = pos_y + m_DirYArr[dir];

            if (((nx < m_MaxWidth - 1) & (nx >= 1)) & ((ny < m_MaxHeight - 1) & (ny >= 1)))
            {
                if (m_MazeArr[nx][ny] == SolidRock)
                {
                    if (CheckNeighbours(dir, nx, ny) > 2) // 2 with cross walls //with this type we have more open spaces
                    {
                        m_MazeArr[nx][ny] = Corridor;
                        GenMazeRecursiveBacktracking(nx, ny);
                    }
                }
            }
        }
    }
}


