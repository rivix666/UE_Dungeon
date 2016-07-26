#pragma once

#include <vector>
// 
// #define A2W(x) { x * 200.0f }

typedef unsigned int uint;

//SpawnWall(0.f, 0.f, FRotator(0.0f, 270.0f, 0.0f)); // Left
//SpawnWall(500.f, 0.f, FRotator(0.0f, 0.0f, 0.0f)); // Up
//SpawnWall(1000.f, 0.f, FRotator(0.0f, 180.0f, 0.0f)); // Down
//SpawnWall(1500.f, 0.f, FRotator(0.0f, 90.0f, 0.0f)); // Right

enum EDir
{
	N = 1,
	S = 2,
	E = 4,
	W = 8
};

//SpawnWallCorner(0.f + 40.0f, 0.f + 40.0f, FRotator(0.0f, 270.0f, 0.0f)); // Down/Left
//SpawnWallCorner(0.f - 40.0f, 0.f + 40.0f, FRotator(0.0f, 0.0f, 0.0f)); // Up/Left
//SpawnWallCorner(0.f + 40.0f, 0.f - 40.0f, FRotator(0.0f, 180.0f, 0.0f)); // Down/Right
//SpawnWallCorner(0.f - 40.0f, 0.f - 40.0f, FRotator(0.0f, 90.0f, 0.0f)); // Up/Right

enum ECornerDir
{
	NW = 1,
	NE = 2,
	SW = 4,
	SE = 8,
};

enum ETileType
{
	Nothing = 1,
	SolidRock = 2,
	Room = 4,
	Corridor = 8,
	Doors = 16,
};

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

	SDoor() :
		InsideDoor(FVector2D(0.0f, 0.0f)),
		OutsideDoor(FVector2D(0.0f, 0.0f)),
		InsideDoorDir(N),
		OutsideDoorDir(S) {}

	SDoor(FVector2D in, FVector2D out, EDir in_d, EDir out_d) :
		InsideDoor(in),
		OutsideDoor(out),
		InsideDoorDir(in_d),
		OutsideDoorDir(out_d) {}

	SDoor(FVector2D in, EDir in_d) :
		InsideDoor(in),
		InsideDoorDir(in_d)
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
