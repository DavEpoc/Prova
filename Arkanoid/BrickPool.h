#pragma once
class BrickPool
{
public:
	//rule of three
	BrickPool(float width,float height);

	BrickPool(const BrickPool &other) = delete;
	const BrickPool &operator=(BrickPool rhs) = delete;
	~BrickPool();

	//methods
	Brick* getBrick();
	void setBrickFree(Brick* brickToFree);
	void freeAllBricks();

	Brick* getBrickAtPos(unsigned int position);
	bool isAvailableAtPos(unsigned int position) const;

private:
	Brick *brickPool[BRICKNUM];
	bool bIsAvailable[BRICKNUM];
};		

/* brickpool.cpp */

//rule of three
BrickPool::BrickPool(float width, float height)
{
	for (int i = 0; i < BRICKNUM; ++i)
	{
		brickPool[i] = new Brick(width, height, 1);
		bIsAvailable[i] = true;
	}
}

BrickPool::~BrickPool()
{
	for (int i = 0; i < BRICKNUM; ++i)
	{
		delete brickPool[i];
	}
}

//methods
Brick* BrickPool::getBrick()
{
	for (int i = 0; i < BRICKNUM; ++i)
	{
		if (bIsAvailable[i])
		{
			bIsAvailable[i] = false;
			return brickPool[i];
		}
	}
	return brickPool[0];
}

void BrickPool::setBrickFree(Brick* brickToFree)
{
	//TODO: make if linkedlist
}

void BrickPool::freeAllBricks()
{
	for (int i = 0; i < BRICKNUM; ++i)
	{
		bIsAvailable[i] = true;
	}
}

Brick* BrickPool::getBrickAtPos(unsigned int position)
{
	bIsAvailable[position] = false;
	return brickPool[position];
}

bool BrickPool::isAvailableAtPos(unsigned int position) const
{
	if (bIsAvailable[position] == true)
	{
		return true;
	}
	return false;
}