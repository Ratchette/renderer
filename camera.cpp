#include "camera.h"

#include <GLFW/glfw3.h>

void Camera::Init(Shader* shader) {
	glEnable(GL_DEPTH_TEST);

	// Transform into world coordinates
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	shader->setMat4("modelTransform", model);

	// Transform into view coordinates
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
	shader->setMat4("viewTransform", view);

	// Projection Matrix
	glm::mat4 projection;
	projection = glm::perspective(glm::radians(fov), DISPLAY_WIDTH / DISPLAY_HEIGHT, 0.1f, 100.0f);
	shader->setMat4("perspectiveTransform", projection);
}

void Camera::UpdateTransforms(Shader* shader, float deltaTime) {
	if (!enabled) {
		return;
	}

	_Zoom(shader);
	_Tilt(shader);
	_Move(deltaTime);
}

void Camera::Disable() {
	enabled = false;
}

void Camera::Enable() {
	enabled = true;
}

void Camera::_Move(float deltaTime) {
	if (upKeyDown) {
		cameraPos += cameraFront * MOVE_SPEED * deltaTime;
	}
	if (downKeyDown) {
		cameraPos -= cameraFront * MOVE_SPEED * deltaTime;
	}
	if (leftKeyDown) {
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * MOVE_SPEED * deltaTime;
	}
	if (rightKeyDown) {
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * MOVE_SPEED * deltaTime;
	}
}

void Camera::_Tilt(Shader* shader) {
	// Change camera direction
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);

	glm::mat4 view = glm::lookAt(
		cameraPos,
		cameraPos + cameraFront,
		cameraUp
	);
	shader->setMat4("viewTransform", view);
}

void Camera::_Zoom(Shader* shader) {
	// Update FOV (zoom)
	glm::mat4 projection = glm::perspective(glm::radians(fov), DISPLAY_WIDTH / DISPLAY_HEIGHT, 0.1f, 100.0f);
	shader->setMat4("perspectiveTransform", projection);
}

void Camera::UpdateKeys(int key, int action) {
	if (key == GLFW_KEY_W) {
		if (action == GLFW_PRESS) {
			upKeyDown = true;
		} else if (action == GLFW_RELEASE) {
			upKeyDown = false;
		}
	}

	if (key == GLFW_KEY_S) {
		if (action == GLFW_PRESS) {
			downKeyDown = true;
		} else if (action == GLFW_RELEASE) {
			downKeyDown = false;
		}
	}

	if (key == GLFW_KEY_A) {
		if (action == GLFW_PRESS) {
			leftKeyDown = true;
		} else if (action == GLFW_RELEASE) {
			leftKeyDown = false;
		}
	}

	if (key == GLFW_KEY_D) {
		if (action == GLFW_PRESS) {
			rightKeyDown = true;
		} else if (action == GLFW_RELEASE) {
			rightKeyDown = false;
		}
	}
}

void Camera::UpdateMouse(float xPos, float yPos) {
	float xOffset = (xPos - lastX) * MOUSE_SENSITIVITY;
	float yOffset = (lastY - yPos) * MOUSE_SENSITIVITY;

	lastX = xPos;
	lastY = yPos;

	yaw += xOffset;
	pitch += yOffset;

	if (pitch > 89.0f) {
		pitch = 89.0f;
	} else if (pitch < -89.0f) {
		pitch = -89.0f;
	}
}

void Camera::UpdateScroll(float yOffset) {
	fov -= yOffset;

	if (fov < 1.0f) {
		fov = 1.0f;
	} else if (fov > 45.0f) {
		fov = 45.0f;
	}
}


