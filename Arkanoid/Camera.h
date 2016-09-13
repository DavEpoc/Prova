#pragma once
class Camera
{
public:
	//rule of three
	Camera();
	Camera(const Camera &other) = default;
	const Camera &operator=(Camera rhs) = delete;
	~Camera() {};

	//methods
	void initCamera();
	glm::mat4 getProjection() const;
	glm::mat4 getView() const;
private:
	// Matricies
	glm::mat4 projection;
	glm::mat4 view;

};

//rule of three
Camera::Camera()
{
	initCamera();
}

//methods
void Camera::initCamera()   // Set up projection matric
{
	projection = glm::perspective
	(
		45.0f,       // Field of view ( how far out to the sides we can see
		1.0f / 1.0f, // Aspect ratio ( stretch image in widh or height )
		0.1f,        // Near plane ( anything close than this will be cut off )
		100.0f       // Far plane ( anything further away than this will be cut off )
	);

	// Set up view matric
	view = glm::lookAt
	(
		glm::vec3(0, 0, 2),  // Camera is at (0,0,2), in World Space
		glm::vec3(0, 0, 0),  // And looks at the origin
		glm::vec3(0, 1, 0)   // Head is up ( set to 0,-1,0 to look upside-down )
	);
}

glm::mat4 Camera::getProjection() const
{
	return projection;
}
glm::mat4 Camera::getView() const
{
	return view;
}