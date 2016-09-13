#pragma once

struct GpuMesh 
{
	GLuint vaoID;
	GLuint vboID[4];
};

/* render.cpp */

class Transform
{
public:
	//rule of three
	Transform();

	//methods
	glm::mat4 setModelMatrix();
	void setInitPosition(float initX, float initY, float initZ);

	void setModelPositionsX(float initZ);
	void setModelPositionsY(float initZ);
	void setModelPositionsZ(float initZ);

	void shiftModel(glm::vec3 displacement);

	glm::mat4 getModel() const;

private:
	glm::vec3 pos;
	glm::quat ori;
	float scale;
	glm::mat4 model;
};

//rule of three

Transform::Transform()
{
	pos = glm::vec3(0, 0, 0);
	ori = glm::quat(1, 0, 0, 0);
	scale = 1;
	model = setModelMatrix();
}

//methods 

/* render.cpp */
glm::mat4 Transform::setModelMatrix() {

	glm::mat4 rotMat = mat4_cast(ori);
	glm::mat4 traMat(1.0);
	glm::mat4 scaMat;

	scaMat[0][0] = 2/(windowWidth);
	scaMat[1][1] = 2/(windowHeight);
	scaMat[2][2] = scale;

	traMat[3] = glm::vec4(pos, 1);

	glm::mat4 m = traMat * rotMat * scaMat;

	return m;
}

void Transform::setInitPosition(float initX, float initY, float initZ)
{

	initX = initX * 2 / windowWidth;
	initY = initY * 2 / windowHeight;
	initZ = initZ * 2 / scale;

	glm::mat4 rotMat = mat4_cast(ori);
	glm::mat4 traMat(1.0);
	glm::mat4 scaMat;

	scaMat[0][0] = 2 / (windowWidth);
	scaMat[1][1] = 2 / (windowHeight);
	scaMat[2][2] = scale;

	pos = glm::vec3(initX, initY, initZ);
	traMat[3] = glm::vec4(pos, 1);

	model = traMat * rotMat * scaMat;
}

void Transform::setModelPositionsX(float initX)
{
	model[3][0] = initX;
}

void Transform::setModelPositionsY(float initY)
{
	model[3][1] = initY;
}

void Transform::setModelPositionsZ(float initZ)
{
	model[3][2] = initZ;
}

void Transform::shiftModel(glm::vec3 displacement)
{
	model = glm::translate(model, displacement);
}

glm::mat4 Transform::getModel() const
{
	return model;
}

struct MeshComponent {
	Transform t;
	GpuMesh mesh;
};

class Collider {
public:
	enum { RECTANGLE } type;
	float distanceX;
	float distanceY;
};
