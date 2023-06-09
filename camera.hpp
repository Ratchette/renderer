#ifndef CAMERA_H
#define CAMERA_H

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "shader.hpp"

extern bool showImGui;

class Camera {
public:
	void Init(float curX, float curY);
	void Update(float deltaTime);

	void OnKeyPress(int key, int action);
	void OnMouseMove(float xPos, float yPos);
	void OnScroll(float yOffset);

	void Disable();
	void Enable();

	glm::mat4 GetViewMatrix();
	glm::mat4 GetProjectionMatrix();

	glm::vec3 GetPosition();
	glm::vec3 GetDirection();
	
private: 
	const float DISPLAY_WIDTH = 1200.0f;
	const float DISPLAY_HEIGHT = 900.0f;

	const float MOVE_SPEED = 2.5f;
	const float MOUSE_SENSITIVITY = 0.1f;

	bool enabled = true;

	glm::mat4 view			= glm::mat4(1.0f);
	glm::mat4 projection	= glm::mat4(1.0f);

	// movement
	bool upKeyDown = false;
	bool downKeyDown = false;
	bool leftKeyDown = false;
	bool rightKeyDown = false;
	glm::vec3 cameraPos = glm::vec3(-3.1f, 3.2f, 5.2f);

	// camera direction
	float yaw = -58.6f;
	float pitch = -22.0f;
	float lastX = 400;
	float lastY = 300;
	glm::vec3 cameraFront = glm::vec3(5.0f, 1.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	// Zoom
	float fov = 45.0f;


	void _Move(float deltaTime);
	void _Tilt();
	void _Zoom();
};

#endif CAMERA_H