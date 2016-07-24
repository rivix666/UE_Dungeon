#pragma once

#include <vector>
typedef unsigned int uint;

enum EDirections
{
	N = 1,
	S = 2,
	E = 4,
	W = 8
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
};

struct SRoom
{
	int PosX;
	int PosY;
	int SizeX;
	int SizeY;

	std::vector <SPoint> RoomDoors;

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
