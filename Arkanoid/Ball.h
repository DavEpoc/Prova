#pragma once
class Ball : public PhysObject
{
public:
	//rule of three
	Ball(int radius, char colorId, float speedX, float speedY, float xposition, float yposition, float zposition);

	Ball(const Ball &other) = default;
	//const Ball &operator=(Ball rhs) = default;
	~Ball();

	//methods
	void setNewBallRadius(float newRadius);
	void setNewBallPosition(float newXposition, float newYposition, float newZposition);
	void setNewBallColor(char color);

	void doPhysicStep();
	void render(Shader &shader) const;

	bool SetupOnBuffer(Colors *myColor);

private:
	float fRadius;
	float fXposition;
	float fYposition;
	float fZposition;
	char cColor;
};

/* ball.cpp */

//rule of three
Ball::Ball(int radius, char color, float speedX, float speedY, float xposition, float yposition, float zposition)
{
	setNewSpeed(speedX, speedY, 0.0f);
	fRadius = radius;
	cColor = color;

	fXposition = xposition;
	fYposition = yposition;
	fZposition = zposition;

	//set Collider
	getCollider()->distanceX = radius * 2 / windowWidth;
	getCollider()->distanceY = radius * 2 / windowHeight;
}

Ball::~Ball()
{
	//delete rendering buffers
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDeleteBuffers(3, getMeshComponent()->mesh.vboID);
	glDeleteVertexArrays(1, &getMeshComponent()->mesh.vaoID);
}

//methods
void Ball::setNewBallRadius(float newRadius)
{
	fRadius = newRadius;
}

void Ball::setNewBallColor(char color)
{
	cColor = color;
}

void Ball::setNewBallPosition(float newXposition, float newYposition, float newZposition)
{
	fXposition = newXposition;
	fYposition = newYposition;
	fZposition = newZposition;
}


/* physics.cpp */

void Ball::doPhysicStep()
{
	glm::vec3 speed = glm::vec3(getSpeed().x, getSpeed().y, getSpeed().z);
	checkForBoundaries(speed,fRadius, fRadius);
}

/* render.cpp */

bool Ball::SetupOnBuffer(Colors *myColor)
{

	//const unsigned int points = 181;
	const unsigned int points = 4;
	const unsigned int floatsPerPoint = 3;
	const unsigned int floatsPerColor = 4;
	const unsigned int texPoints = 4;
	const unsigned int texFloatsPerPoint = 2;
	unsigned int sizeInBytes = (points * floatsPerPoint) * sizeof(float);
	unsigned int sizeInColorBytes = (points * floatsPerColor) * sizeof(float);

	getMeshComponent()->t.setInitPosition(fXposition, fYposition, fZposition);

	const float ball[points][floatsPerPoint] =
	{
		{ -(2 * fRadius / 2)  ,  (2 * fRadius / 2) ,  0 }, // Top left
		{  (2 * fRadius / 2)  ,  (2 * fRadius / 2) ,  0 }, // Top right
		{  (2 * fRadius / 2)  , -(2 * fRadius / 2) ,  0 }, // Bottom right 
		{ -(2 * fRadius / 2)  , -(2 * fRadius / 2) ,  0 }, // Bottom left
	};


	const float colors[points][floatsPerColor] =
	{
		{ myColor->fRGBa[0], myColor->fRGBa[1], myColor->fRGBa[2], myColor->fRGBa[3] }, // Top left
		{ myColor->fRGBa[0], myColor->fRGBa[1], myColor->fRGBa[2], myColor->fRGBa[3] }, // Top right
		{ myColor->fRGBa[0], myColor->fRGBa[1], myColor->fRGBa[2], myColor->fRGBa[3] }, // Bottom right
		{ myColor->fRGBa[0], myColor->fRGBa[1], myColor->fRGBa[2], myColor->fRGBa[3] }, // Bottom left
	};
	
	float texcoord[texPoints][texFloatsPerPoint] = 
	{ 
		{ 0.0f, 0.0f },
		{ 1.0f, 0.0f },
		{ 1.0f, 1.0f },
		{ 0.0f, 1.0f },
	};
	

	std::vector<float> positions2;
	std::vector<float> colors2;
	std::vector<float> pixels2;

	for (unsigned int i = 0; i < points; ++i)
	{
		for (unsigned int k = 0; k < floatsPerPoint; ++k)
		{
			positions2.push_back(ball[i][k]);          
		}
	}

	for (unsigned int i = 0; i < points; ++i)
	{
		for (unsigned int k = 0; k < floatsPerColor; ++k)
		{
			colors2.push_back(colors[i][k]);            
		}
	}

	for (unsigned int i = 0; i < texPoints; ++i)
	{
		for (unsigned int k = 0; k < texFloatsPerPoint; ++k)
		{
			pixels2.push_back(texcoord[i][k]);         
		}
	}


	// Generate and assign three Vertex Buffer Objects to our handle
	glGenBuffers(3, getMeshComponent()->mesh.vboID);

	// Generate and assign a Vertex Array Object to our handle
	glGenVertexArrays(1, &getMeshComponent()->mesh.vaoID);

	// Bind our Vertex Array Object as the current used object
	glBindVertexArray(getMeshComponent()->mesh.vaoID);

	// Positions
	// ===================
	// Bind our first VBO as being the active buffer and storing vertex attributes (coordinates)
	glBindBuffer(GL_ARRAY_BUFFER, getMeshComponent()->mesh.vboID[0]);

	// Copy the vertex data from positions2 to our buffer  
	glBufferData(GL_ARRAY_BUFFER, positions2.size() * sizeof(float), &positions2[0], GL_STATIC_DRAW);

	unsigned int positionAttributeIndex = 0;
	// Specify that our coordinate data is going into attribute index 0, and contains three floats per vertex
	glVertexAttribPointer(positionAttributeIndex, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Colors
	// =======================
	glBindBuffer(GL_ARRAY_BUFFER, getMeshComponent()->mesh.vboID[1]);

	// Copy the vertex data from colors2 to our buffer  (points * floatsPerColor)
	glBufferData(GL_ARRAY_BUFFER, colors2.size() * sizeof(float), &colors2[0], GL_STATIC_DRAW);

	unsigned int colorAttributeIndex = 1;
	// Specify that our coordinate data is going into attribute index 1, and contains four floats per vertex
	glVertexAttribPointer(colorAttributeIndex, 4, GL_FLOAT, GL_FALSE, 0, 0);


	// Texture Coords

	// Bind our first VBO as being the active buffer and storing vertex attributes (coordinates)
	glBindBuffer(GL_ARRAY_BUFFER, getMeshComponent()->mesh.vboID[2]);

	// Copy the vertex data from pixels2 to our buffer 
	glBufferData(GL_ARRAY_BUFFER, pixels2.size() * sizeof(float), &pixels2[0], GL_STATIC_DRAW);

	unsigned int TexCoordAttributeIndex = 2; 
	 // Specify that our coordinate data is going into attribute index 2, and contains two floats per vertex
	glVertexAttribPointer(TexCoordAttributeIndex, 2, GL_FLOAT, GL_FALSE, 0, 0); 


	glGenTextures(1, &getMeshComponent()->mesh.vboID[3]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, getMeshComponent()->mesh.vboID[3]);

	float bordercolor[] = { 0.0f, 0.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, bordercolor); //what if out of 0-1 range-> if clamp will set that flat color

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); //what if out of 0-1 range to coordinate S(x)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER); //what if out of 0-1 range to coordinate T(y)

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);     //interpolation type (GL_NEAREST) when scaling image down
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);     //interpolation type (GL_NEAREST) when scaling image up

    //since texture is linked not only to data but also to height and width we need 1 for each texture
	int boardWidthTexture0  = 2 * fRadius;
	int boardHeightTexture0 = 2 * fRadius;
	int boardWidthTexture1  = 2 * fRadius;
	int boardHeightTexture1 = 2 * fRadius;
	int colorComponents = 4;

	setTextureData("textures/ballTex.png", 0, &boardWidthTexture0, &boardHeightTexture0, &colorComponents);
	setTextureData("textures/ballTex2.png", 1, &boardWidthTexture1, &boardHeightTexture1, &colorComponents);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, boardWidthTexture0, boardHeightTexture0, 0, GL_RGBA, GL_UNSIGNED_BYTE, getTextureData(0));

	// Enable our attribute within the current VAO
	glEnableVertexAttribArray(positionAttributeIndex);
	glEnableVertexAttribArray(colorAttributeIndex);
	glEnableVertexAttribArray(TexCoordAttributeIndex);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);

	glBindBuffer(GL_TEXTURE_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	return true;
}

void Ball::render(Shader &shader) const
{
	shader.UseProgram();

	glBindVertexArray(getMeshComponent()->mesh.vaoID);
	glBindTexture(GL_TEXTURE_2D, getMeshComponent()->mesh.vboID[3]);

	glDrawArrays(GL_QUADS, 0, 4);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}