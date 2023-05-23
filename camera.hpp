#ifndef CAMERA_H
#define CAMERA_H

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "shader.hpp"

class Camera {
public:
	void Init(GLFWwindow* window, Shader* shader);
	void Update(Shader* shader, float deltaTime);

	void OnKeyPress(int key, int action);
	void OnMouseMove(float xPos, float yPos);
	void OnScroll(float yOffset);

	void Disable();
	void Enable();
	
private: 
	const float DISPLAY_WIDTH = 800.0f;
	const float DISPLAY_HEIGHT = 600.0f;

	const float MOVE_SPEED = 2.5f;
	const float MOUSE_SENSITIVITY = 0.1f;

	bool enabled = true;

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
	void _Tilt(Shader* shader);
	void _Zoom(Shader* shader);
};

#endif CAMERA_H