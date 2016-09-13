#pragma once
#include <stdlib.h> 

class PhysObject {
public:
	//rule of three
	PhysObject();
	~PhysObject();
	//methods
	void updateMVP(Camera *camera);
	void linkToShaderMVP(Shader *shader);
	
	void setTextureData(const char *fileName, unsigned int texPosition,int *texWidth,int *texHeight, int *texComponents);

	void setNewSpeed(float speedx, float speedy, float speedz);
	glm::vec3 getSpeed() const;

	void checkForBoundaries(glm::vec3 speed, float correctWidth, float correctHeight);

	static bool collides(const PhysObject &a, const PhysObject &b);

protected:
	glm::mat4 getMVP() const;
	MeshComponent* getMeshComponent() const;
	Collider* getCollider() const;
	GLubyte* getTextureData(unsigned int texPosition) const;


private:
	MeshComponent* meshComponent;
	Collider* coll;

	glm::mat4 mvp;
	GLubyte *textureData[2];
	glm::vec3 vSpeed;

	// Id witch links code to shader uniform mvp
	GLuint matrixID; 
	// if true the physObject is linked to the shader
	bool matrixIDSet;

};

//rule of three
PhysObject::PhysObject() : matrixID(0) , matrixIDSet(false)
{
	meshComponent = new MeshComponent();
	coll = new Collider();

	textureData[0] = nullptr;
	textureData[1] = nullptr;

	vSpeed = glm::vec3(0.0f, 0.0f, 0.0f);

	//updateMVP();
}


PhysObject::~PhysObject()
{
	if (textureData[0])
	{
		delete textureData[0];
	}
	if (textureData[1])
	{
		delete textureData[1];
	}

	delete meshComponent;
	delete coll;
}

glm::mat4 PhysObject::getMVP() const
{
	return mvp;
}

MeshComponent* PhysObject::getMeshComponent() const
{
	return meshComponent;
}

Collider* PhysObject::getCollider() const
{
	return coll;
}

glm::vec3 PhysObject::getSpeed() const
{
	return vSpeed;
}

void PhysObject::setNewSpeed(float speedx, float speedy, float speedz)
{
	vSpeed = glm::vec3(speedx, speedy, speedz);
}

GLubyte* PhysObject::getTextureData(unsigned int texPosition) const
{
	return textureData[texPosition];
}

void PhysObject::updateMVP(Camera *camera)
{
	mvp = camera->getProjection() * camera->getView() * getMeshComponent()->t.getModel();
}


void PhysObject::linkToShaderMVP(Shader *shader)
{
	if (!matrixIDSet)
	{
		matrixID = glGetUniformLocation(shader->getShaderProgram(), "mvp");
		matrixIDSet = true;
	}

	// Send our transformation to the currently bound shader,
	// in the "MVP" uniform
	// For each model you render, since the MVP will be different (at least the M part)
	glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvp[0][0]);

}

void PhysObject::setTextureData(const char *fileName, unsigned int texPosition, int *texWidth, int *texHeight, int *texComponents)
{
	textureData[texPosition] = stbi_load(fileName, texWidth, texHeight, texComponents, 0);
	if(textureData[texPosition] == NULL)
	{
		quitGame = true;
	}
}

bool PhysObject::collides(const PhysObject &a, const PhysObject &b)
{

	float currDistX = abs(a.getMeshComponent()->t.getModel()[3][0] - b.getMeshComponent()->t.getModel()[3][0]);
	float currDistY = abs(a.getMeshComponent()->t.getModel()[3][1] - b.getMeshComponent()->t.getModel()[3][1]);

	float minDistX = a.getCollider()->distanceX + b.getCollider()->distanceX;
	float minDistY = a.getCollider()->distanceY + b.getCollider()->distanceY;

	float AnormalizedSpeedX = (a.vSpeed.x / (windowWidth / 2));
	float AnormalizedSpeedY = (a.vSpeed.y / (windowHeight / 2));
	float BnormalizedSpeedX = (b.vSpeed.x / (windowWidth / 2));
	float BnormalizedSpeedY = (b.vSpeed.y / (windowHeight / 2));

	float nextStepX;
	if (AnormalizedSpeedX > 0.0f && BnormalizedSpeedX > 0.0f || AnormalizedSpeedX < 0.0f && BnormalizedSpeedX < 0.0f)
	{
		nextStepX = abs(AnormalizedSpeedX - BnormalizedSpeedX);
	}
	else if (AnormalizedSpeedX > 0.0f && BnormalizedSpeedX < 0.0f || AnormalizedSpeedX < 0.0f && BnormalizedSpeedX > 0.0f)
	{
		nextStepX = AnormalizedSpeedX + BnormalizedSpeedX;
	}
	else
	{
		nextStepX = 0.0f;
	}

	float nextStepY;
	if (AnormalizedSpeedY > 0.0f && BnormalizedSpeedY > 0.0f || AnormalizedSpeedY < 0.0f && BnormalizedSpeedY < 0.0f)
	{
		nextStepY = abs(AnormalizedSpeedY - BnormalizedSpeedY);
	}
	else if (AnormalizedSpeedY > 0.0f && BnormalizedSpeedY < 0.0f || AnormalizedSpeedY < 0.0f && BnormalizedSpeedY > 0.0f)
	{
		nextStepY = AnormalizedSpeedY + BnormalizedSpeedY;
	}
	else
	{
		nextStepY = 0.0f;
	}

	if (currDistX + nextStepX < minDistX && currDistY + nextStepY < minDistY)
	{
		std::cout << "COLLIDES" << std::endl;
		return true;
	}
	return false;
}

void PhysObject::checkForBoundaries(glm::vec3 speed,float correctWidth, float correctHeight)
{
	float dispX = speed.x;
	float dispX2 = (speed.x / (windowWidth / 2));

	//std::cout << "vSpeed.x: " << speed.x << std::endl;

	glm::vec3 displacementX = glm::vec3(dispX, 0, 0);

	float limitX = (((windowWidth / 2) - brickWidth - correctWidth) / (windowWidth / 2));

	if (getMeshComponent()->t.getModel()[3][0] + dispX2 < limitX && getMeshComponent()->t.getModel()[3][0] + dispX2 > -limitX)
	{
		//call mesh method
		getMeshComponent()->t.shiftModel(displacementX);
	}
	else
	{

		if (dispX2 > 0)
		{
			//mvp[3][0] = limitX;
			getMeshComponent()->t.setModelPositionsX(limitX);
		}
		else if (dispX2 < 0)
		{
			//mvp[3][0] = -limitX;
			getMeshComponent()->t.setModelPositionsX(-limitX);
		}
		vSpeed.x = -vSpeed.x;
	}
	
	float dispY = speed.y;
	float dispY2 = (speed.y / (windowHeight / 2));

	glm::vec3 displacementY = glm::vec3(0, dispY, 0);

	float limitY = (((windowHeight / 2) + correctHeight) / (windowHeight / 2));

	if (getMeshComponent()->t.getModel()[3][1] + dispY2 < limitY && getMeshComponent()->t.getModel()[3][1] + dispY2 > -limitY)
	{
		getMeshComponent()->t.shiftModel(displacementY);
	}
	else
	{

		if (dispY2 > 0)
		{
			//mvp[3][1] = limitY;
			getMeshComponent()->t.setModelPositionsY(limitY);
		}
		else if (dispY2 < 0)
		{
			//mvp[3][1] = -limitY;
			getMeshComponent()->t.setModelPositionsY(-limitY);
			quitGame = true;
		}
		vSpeed.y = -vSpeed.y;
	}
}