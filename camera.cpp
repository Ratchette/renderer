#include "camera.hpp"

void Camera::Init(float curX, float curY) {
	lastX = curX;
	lastY = curY;
	
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
	projection = glm::perspective(glm::radians(fov), DISPLAY_WIDTH / DISPLAY_HEIGHT, 0.1f, 100.0f);
}

void Camera::Update(float deltaTime) {
	if (!enabled) {
		return;
	}

	_Zoom();
	_Tilt();
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

void Camera::_Tilt() {
	// Change camera direction
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);

	view = glm::lookAt(
		cameraPos,
		cameraPos + cameraFront,
		cameraUp
	);
}

void Camera::_Zoom() {
	// Update FOV (zoom)
	projection = glm::perspective(glm::radians(fov), DISPLAY_WIDTH / DISPLAY_HEIGHT, 0.1f, 100.0f);
}

void Camera::OnKeyPress(int key, int action) {
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

void Camera::OnMouseMove(float xPos, float yPos) {
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

void Camera::OnScroll(float yOffset) {
	fov -= yOffset;

	if (fov < 1.0f) {
		fov = 1.0f;
	} else if (fov > 45.0f) {
		fov = 45.0f;
	}
}


glm::mat4 Camera::GetViewMatrix() {
	return glm::mat4(view);
}
glm::mat4 Camera::GetProjectionMatrix() {
	return glm::mat4(projection);
}

glm::vec3 Camera::GetCameraPosition() {
	return glm::vec3(cameraPos);
}