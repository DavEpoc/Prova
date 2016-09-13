#pragma once
class Brick : public PhysObject
{
public:
	//rule of three
	Brick(float width, float height, unsigned int health);

	Brick(const Brick &other) = delete;
	const Brick &operator=(Brick rhs) = delete;
	~Brick();

	//methods
	void setNewBrickDimentions(float newWidth, float newHeight);
	void setNewBrickHealth(unsigned int newHealth);

	void decreaseHealth();

	unsigned int getHealth() const;

	void render(Shader &shader) const;

	bool SetupOnBuffer(unsigned int equivalentPosNumber, Colors *myColor);

private:
	float fWidth;
	float fHeight;

	unsigned int uiHealth;
};

/* brick.cpp */

//rule of three
Brick::Brick(float width, float height, unsigned int health)
{
	fWidth = width;
	fHeight = height;

	uiHealth = health;

	//set Collider
	getCollider()->distanceX = (width / 2)  * 2 / windowWidth;
	getCollider()->distanceY = (height / 2) * 2 / windowHeight;
}

Brick::~Brick()
{
	//delete rendering buffers
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDeleteBuffers(3, getMeshComponent()->mesh.vboID);
	glDeleteVertexArrays(1, &getMeshComponent()->mesh.vaoID);
}

//methods
void Brick::setNewBrickDimentions(float newWidth, float newHeight)
{
	fWidth = newWidth;
	fHeight = newHeight;
}

void Brick::setNewBrickHealth(unsigned int newHealth)
{
	uiHealth = newHealth;
}

void Brick::decreaseHealth()
{
	--uiHealth;
}

unsigned int Brick::getHealth() const
{
	return uiHealth;
}

/* render.cpp */

bool Brick::SetupOnBuffer(unsigned int equivalentPosNumber, Colors *myColor)
{
	unsigned int row = (equivalentPosNumber / COLUMNSNUM) + 1;
	unsigned int column = equivalentPosNumber % COLUMNSNUM + 1;
	float floatColumn = static_cast<float>(column);
	float floatRow = static_cast<float>(row);

	const unsigned int points = 4;
	const unsigned int floatsPerPoint = 3;
	const unsigned int floatsPerColor = 4;
	const unsigned int texPoints = 4;
	const unsigned int texFloatsPerPoint = 2;
	unsigned int sizeInBytes = (points * floatsPerPoint) * sizeof(float);
	unsigned int sizeInColorBytes = (points * floatsPerColor) * sizeof(float);
	unsigned int texSizeInBytes = (texPoints * texFloatsPerPoint) * sizeof(float);
	
	float startBrickXpos = -(windowWidth / 2) + (3 * fWidth / 2) + fWidth * (floatColumn - 1.0f); //1001101001
	float startBrickYpos = (windowHeight / 2) - (fHeight / 2) + fHeight * (-floatRow + 1.0f);
	float startBrickZpos = 0.0f;

	getMeshComponent()->t.setInitPosition(startBrickXpos, startBrickYpos , startBrickZpos);

	const float brick[points][floatsPerPoint] =
	{
		{ -(fWidth / 2) ,  (fHeight / 2) ,  0 }, // Top left
		{  (fWidth / 2) ,  (fHeight / 2) ,  0 }, // Top right
		{  (fWidth / 2) , -(fHeight / 2) ,  0 }, // Bottom right 
		{ -(fWidth / 2) , -(fHeight / 2) ,  0 }, // Bottom left
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
			positions2.push_back(brick[i][k]);          
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
	int brickWidthTexture0 = fWidth;
	int brickHeightTexture0 = fHeight;
	int brickWidthTexture1 = fWidth;
	int brickHeightTexture1 = fHeight;
	int colorComponents = 3;

	setTextureData("textures/brickTex.jpg", 0, &brickWidthTexture0, &brickHeightTexture0, &colorComponents);
	setTextureData("textures/brickTex2.jpg", 1, &brickWidthTexture1, &brickHeightTexture1, &colorComponents);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, brickWidthTexture0, brickHeightTexture0, 0, GL_RGB, GL_UNSIGNED_BYTE, getTextureData(0));

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

void Brick::render(Shader &shader) const
{
	shader.UseProgram();
	glBindVertexArray(getMeshComponent()->mesh.vaoID);
	glBindTexture(GL_TEXTURE_2D, getMeshComponent()->mesh.vboID[3]);

	glDrawArrays(GL_QUADS, 0, 4);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}