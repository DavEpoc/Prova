#pragma once
#include <fstream>
#include <sstream>
#include <string>

#include <ctime>

class Scene
{
public:
	//rule of three
	Scene();

	Scene(const Scene &other) = delete;
	const Scene &operator=(Scene rhs) = delete;
	~Scene();

	//methods
	unsigned int getLevelNumber() const;
	void initSameLevel();

	void doPhysicStep();
	void checkAllCollisions() const;

	void startLevelRender() const;
	void render() const;

	void winningLevel();

	void initShader() const;
	void clearShader() const;
	//parsing
	bool initNewLevel(unsigned int levelNumber);
	unsigned int findRandomLevel() const;

private:
	Board            *sceneBoard;
	Ball             *sceneBall;
	BrickPool        *sceneBrickPool;

	Camera           *sceneCamera;
	ColorsPalette    *scenePalette;
	Shader           *sceneShader;

	unsigned int LevelsNumber;
	unsigned int actualLevelNumber;

	unsigned int uiBackgroundTextureId;

	char activeBrickGrid[BRICKNUM];
	char colorBrickGrid[BRICKNUM];
	char defaultColor;
};

/* scene.cpp */

//rule of three
Scene::Scene()
{
	std::ifstream myActiveBrickGrid("activeBrickGrid.txt");
	if (!myActiveBrickGrid.is_open()) quitGame = true;			

	std::string line;
	std::getline(myActiveBrickGrid, line);                 // on first line i read game's levels number
	std::istringstream iss0(line);
	iss0 >> LevelsNumber;					               // save it into class property

	std::ifstream myColorBrickGrid("colorBrickGrid.txt");  
	if (!myColorBrickGrid.is_open()) quitGame = true;

	std::string line2;
	std::getline(myColorBrickGrid, line2);                 // on first line i read game's objects default color
	std::istringstream iss1(line2);
	iss1 >> defaultColor;								   // save it into class property


	actualLevelNumber = 1;
	uiBackgroundTextureId = 0;

	float boardWidth = 100.0f;
	float boardHeight = 20.0f;
	float boardSpeed = 10.0f;
	float boardStartingXPos = 0.0f;
	float boardStartingYPos = -(windowHeight / 2) + 2 * brickHeight - (boardHeight / 2);
	float boardStartingZPos = 0.0f;

	float ballRadius = 15.0f;
	float ballStartingXPos = 0.0f;   
	float ballStartingYPos = -(windowHeight / 2) + 2 * brickHeight + (2 * ballRadius / 2);
	float ballStartingZPos = 0.0f;
	float ballSpeedX = -10.0f;
	float ballSpeedY = -10.0f;

	sceneBoard       = new Board(boardWidth, boardHeight, boardSpeed, boardStartingXPos, boardStartingYPos, boardStartingZPos);
	sceneBall        = new Ball(ballRadius, defaultColor, ballSpeedX, ballSpeedY ,ballStartingXPos, ballStartingYPos, ballStartingZPos);  
	sceneBrickPool   = new BrickPool(BRICKWIDTH, BRICKHEIGHT);

	sceneCamera      = new Camera();
	scenePalette     = new ColorsPalette();
	sceneShader      = new Shader();
	//set grids null at start, i'll initialize them with initNewLevel() and initSameLevel() methods
	for (int i = 0; i < BRICKNUM; ++i)
	{
		activeBrickGrid[i] = 'N';
		colorBrickGrid[i]  = 'N';
	}


}

Scene::~Scene()
{
	delete sceneBoard;
	delete sceneBall;
	delete sceneBrickPool;

	delete sceneCamera;
	delete scenePalette;
	delete sceneShader;
}

//methods

void Scene::initSameLevel()
{
	float ballSpeedX = -10.0f;
	float ballSpeedY = -10.0f;

	sceneBrickPool->freeAllBricks();
	startLevelRender();

	sceneBall->setNewSpeed(ballSpeedX, ballSpeedY, 0.0f);
}

/* physics.cpp */

void Scene::doPhysicStep()
{
	checkAllCollisions();
	
	sceneBall->doPhysicStep();
	sceneBoard->doPhysicStep();
}

/* render.cpp */

void Scene::startLevelRender() const
{

	sceneBoard->SetupOnBuffer();

	Colors *myBallColor = scenePalette->getColorFromMap(defaultColor);
	sceneBall->SetupOnBuffer(myBallColor);
	for (unsigned int i = 0; i < BRICKNUM; ++i)
	{
		if (activeBrickGrid[i] - '0' > 0)  // - '0' into int
		{
			Brick* currentBrick;
			currentBrick = sceneBrickPool->getBrickAtPos(i);

			currentBrick->setNewBrickHealth(activeBrickGrid[i] - '0'); // - '0' into int

			Colors *myBrickColor = scenePalette->getColorFromMap(colorBrickGrid[i]);
			currentBrick->SetupOnBuffer(i, myBrickColor);
		}
	}
}

void Scene::winningLevel()
{
	for (unsigned int i = 0; i < BRICKNUM; ++i)
	{
		if (activeBrickGrid[i] - '0' > 0)  // - '0' into int
		{
			Brick* currentBrick;
			currentBrick = sceneBrickPool->getBrickAtPos(i);
			if (currentBrick->getHealth() != 0)
			{
				return;
			}
		}
	}

	unsigned int randLevel = findRandomLevel();
	//std::cout << randLevel << std::endl;
	if (randLevel != getLevelNumber())
	{
		initNewLevel(randLevel);
	}
	else
	{
		initSameLevel();
	}
}

void Scene::render() const
{
	sceneBoard->updateMVP(sceneCamera);
	sceneBoard->linkToShaderMVP(sceneShader); 
	sceneBoard->render(*sceneShader);

	sceneBall->updateMVP(sceneCamera);
	sceneBall->linkToShaderMVP(sceneShader);
	sceneBall->render(*sceneShader);


	for (unsigned int i = 0; i < BRICKNUM; ++i)
	{
		if (activeBrickGrid[i] - '0' > 0)			          //if is a level brick
		{
			Brick* currentBrick;
			currentBrick = sceneBrickPool->getBrickAtPos(i);
			if (currentBrick->getHealth() > 0)				  //and has more then 0 health
			{
				currentBrick->updateMVP(sceneCamera);
				currentBrick->linkToShaderMVP(sceneShader);
				currentBrick->render(*sceneShader);           //keep render it
			}
		}
	}
}

/* asset_manager.cpp */

bool Scene::initNewLevel(unsigned int levelNumber = 1)
{
	// reset
	float ballSpeedX = -10.0f;
	float ballSpeedY = -10.0f;
	actualLevelNumber = levelNumber;
	sceneBrickPool->freeAllBricks();

	sceneBall->setNewSpeed(ballSpeedX, ballSpeedY, 0.0f);

	// parsing
	std::ifstream myActiveBrickGrid("activeBrickGrid.txt");                                 // each time a change level i do parce again
	std::ifstream myColorBrickGrid("colorBrickGrid.txt");
	if (!myActiveBrickGrid.is_open() | !myColorBrickGrid.is_open()) return false;			
	
	std::string line;
	std::string line2;

	for (unsigned int i = 0; i <= levelNumber; ++i)	//reading lines concerning levels info
	{
		std::getline(myActiveBrickGrid, line);      //CAREFULL: if line missing the last computed will remain
		std::getline(myColorBrickGrid, line2);
	}

	std::istringstream iss(line);
	std::istringstream iss2(line2);

	char value;
	char value2;
	unsigned int count = 0;

	while (iss.get(value)) // get(value) already does : iss >> value;
	{
		iss2 >> value2;   // do shift lines of position and color at the same time

		activeBrickGrid[count] = value;
		colorBrickGrid[count] = 'N';
		if (value != '0')
		{
			colorBrickGrid[count] = value2;
		}
		++count;
	}
	// render at start
	startLevelRender();
	return true;
}

unsigned int Scene::findRandomLevel() const
{
	unsigned int randLevelNumber;
	std::cout << LevelsNumber << std::endl;
	randLevelNumber = (rand() % LevelsNumber) + 1;
	return randLevelNumber;
}

unsigned int Scene::getLevelNumber() const
{
	return actualLevelNumber;
}


void Scene::checkAllCollisions() const
{
	if (PhysObject::collides(*sceneBall, *sceneBoard)) {

		std::cout << "collides with board" << std::endl;
		sceneBall->setNewSpeed(sceneBall->getSpeed().x, -(sceneBall->getSpeed().y), 0.0f);

		return;
	}
	
	
	for (unsigned int i = 0; i < BRICKNUM; ++i)
	{		  
		if (activeBrickGrid[i] - '0' > 0)			  
		{
			Brick* currentBrick;
			currentBrick = sceneBrickPool->getBrickAtPos(i);
			if (currentBrick->getHealth() > 0 && PhysObject::collides(*sceneBall,*currentBrick))
			{
				std::cout << "collides with brick" << std::endl;

				currentBrick->decreaseHealth();

				sceneBall->setNewSpeed(sceneBall->getSpeed().x, -(sceneBall->getSpeed().y), 0.0f);

				return;
			}
		}
	}

}

void Scene::initShader() const
{
	if (!sceneShader->Init())                               // initialize shaders
		quitGame = true;
}

void Scene::clearShader() const								// call cleanup
{
	sceneShader->CleanUp();
}