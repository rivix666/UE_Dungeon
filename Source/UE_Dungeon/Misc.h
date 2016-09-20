#pragma once
#include <vector>

typedef unsigned int uint;

// Enums
//////////////////////////////////////////////////////////////////////////
enum EDir
{
	N = 2,
	E = 4,
	S = 8,
	W = 16
};

enum ECornerDir
{
	NW = 2,
	NE = 4,
	SE = 8,
	SW = 16,
};

enum ETileType
{
	Nothing = 0,
	SolidRock = 128,
	Room = 256,
	RoomWall = 2048,
	Corridor = 512,
	Doors = 1024,
};

// Structs
//////////////////////////////////////////////////////////////////////////
struct SPoint
{
	int X;
	int Y;

	SPoint() : X(0), Y(0) {}
	SPoint(int x, int y) : X(x), Y(y) {}
	~SPoint() {}

	bool operator==(const SPoint& r)
	{
		bool result = true;
		result = result && (X == r.X);
		result = result && (Y == r.Y);
		return result;
	}

	bool operator!=(const SPoint& r)
	{
		bool result = true;
		result = result && (X != r.X);
		result = result && (Y != r.Y);
		return result;
	}

	bool operator<(const SPoint& r)
	{
		return X < r.X;
	}

	bool operator() (const SPoint& r) const
	{
		return X < r.X;
	}
};


struct SDoor
{
	FVector2D InsideDoor;
	FVector2D OutsideDoor;
	EDir InsideDoorDir;
	EDir OutsideDoorDir;
	SDoor* SpanningTreeDoor;
	float  SpanningLength;

	SDoor() :
		InsideDoor(FVector2D(0.0f, 0.0f)),
		OutsideDoor(FVector2D(0.0f, 0.0f)),
		InsideDoorDir(N),
		OutsideDoorDir(S),
		SpanningTreeDoor(nullptr) {}

	SDoor(FVector2D in, FVector2D out, int in_d, int out_d) :
		InsideDoor(in),
		OutsideDoor(out),
		InsideDoorDir((EDir)in_d),
		OutsideDoorDir((EDir)out_d),
		SpanningTreeDoor(nullptr) {}

	SDoor(FVector2D in, int in_d) :
		InsideDoor(in),
		InsideDoorDir((EDir)in_d),
		SpanningTreeDoor(nullptr)
	{
		switch (in_d)
		{
		case N:
			OutsideDoorDir = S;
			OutsideDoor = FVector2D(InsideDoor.X, InsideDoor.Y - 1);
			break;
		case S:
			OutsideDoorDir = N;
			OutsideDoor = FVector2D(InsideDoor.X, InsideDoor.Y + 1);
			break;
		case E:
			OutsideDoorDir = W;
			OutsideDoor = FVector2D(InsideDoor.X + 1, InsideDoor.Y);
			break;
		case W:
			OutsideDoorDir = E;
			OutsideDoor = FVector2D(InsideDoor.X - 1, InsideDoor.Y);
			break;
		}
	}

	~SDoor() {}
};

struct SRoom
{
	int PosX;
	int PosY;
	int SizeX;
	int SizeY;

	std::vector <SDoor> RoomDoors;


	SRoom() : PosX(0), PosY(0), SizeX(0), SizeY(0) {}
	SRoom(int x, int y, int size_x, int size_y) :
		PosX(x), PosY(y), SizeX(size_x), SizeY(size_y) {}
	~SRoom() {}

	bool operator==(const SRoom& r)
	{
		bool result = true;
		result = result && (PosX == r.PosX);
		result = result && (PosY == r.PosY);
		result = result && (SizeX == r.SizeX);
		result = result && (SizeY == r.SizeY);
		return result;
	}
};

struct SPath
{
	SPoint pos;
	int g;
	int h;
	int f;
	SPath* parent;

	SPath(int x, int y, int G, int H, SPath* Parent = nullptr) : pos(SPoint(x, y)), g(G), h(H), f(g + h), parent(Parent) {}
	SPath(SPoint Pos, int G, int H, SPath* Parent = nullptr) : pos(Pos), g(G), h(H), f(g + h), parent(Parent) {}
	SPath() : pos(SPoint(0, 0)), g(0), h(0), f(0), parent(nullptr) {}

	bool operator==(const SPoint& r)
	{
		return pos == r;
	}

	bool operator==(const SPath& r)
	{
		//         int sum = g + h;
		//         int sum2 = r.g + r.h;
		//         return sum == sum2;

		return pos == r.pos;
	}

	bool operator>(const SPath& r)
	{
		return f > r.f;
	}

	bool operator<(const SPath& r)
	{
		return f < r.f;
	}
};


// Global Const
//////////////////////////////////////////////////////////////////////////
const float				   SCALE = 1.3f;
const float				   TILE_SIZE = 200.0f * SCALE;
const FVector			   SCALE_VEC = FVector(SCALE, SCALE, SCALE);

// Defines
//////////////////////////////////////////////////////////////////////////
#define A2W(x)  x * TILE_SIZE //Array to World pos converter





//SpawnWall(0.f, 0.f, FRotator(0.0f, 270.0f, 0.0f)); // Left
//SpawnWall(500.f, 0.f, FRotator(0.0f, 0.0f, 0.0f)); // Up
//SpawnWall(1000.f, 0.f, FRotator(0.0f, 180.0f, 0.0f)); // Down
//SpawnWall(1500.f, 0.f, FRotator(0.0f, 90.0f, 0.0f)); // Right
//SpawnWallCorner(0.f + 40.0f, 0.f + 40.0f, FRotator(0.0f, 270.0f, 0.0f)); // Down/Left
//SpawnWallCorner(0.f - 40.0f, 0.f + 40.0f, FRotator(0.0f, 0.0f, 0.0f)); // Up/Left
//SpawnWallCorner(0.f + 40.0f, 0.f - 40.0f, FRotator(0.0f, 180.0f, 0.0f)); // Down/Right
//SpawnWallCorner(0.f - 40.0f, 0.f - 40.0f, FRotator(0.0f, 90.0f, 0.0f)); // Up/Right