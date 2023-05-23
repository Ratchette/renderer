
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <vector>

#include "shader.hpp"
#include "camera.hpp"

const char* glsl_version = "#version 330";
const float SCREEN_HEIGHT = 600.0f;
const float SCREEN_WIDTH = 800.0f;

const char* BOX_TEXTURE_FILE = "assets/container.jpg";
const char* SIMILEY_TEXTURE_FILE = "assets/awesomeface.png";

static bool showImGui = false;
static ImVec4 clear_color = ImVec4(0.2f, 0.3f, 0.3f, 1.0f);
static glm::vec3 lightColor(1);

std::vector<float> vertices = {
	// positions          // normals           // texture coords
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};

std::vector<glm::vec3> cubePositions = {
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(2.0f,  5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f,  2.0f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
};

Camera camera;
glm::vec3 lightPos(1.5f, 2.0f, 1.5f);
glm::vec3 lightDirection(-0.2f, -1.0f, -0.3f);

static float deltaTime = 0.0f;
static float lastFrame = 0.0f;


static void glfw_error_callback(int error, const char* description);
void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void glfw_mouse_callback(GLFWwindow* window, double xpos, double ypos);
void glfw_scroll_callback(GLFWwindow* window, double xOffset, double yOffset);

GLFWwindow* InitWindow();
void InitVertexConfig(GLuint* VAO, std::vector<float> vertices);
GLuint InitTexture(const char* filepath, GLenum texture_unit);
void InitCubeShader(Shader* shader);
void InitLightShader(Shader* shader);

void RenderCubes(Shader* shader);
void RenderLight(Shader* shader);
void Draw();

void InitImGUI(GLFWwindow** window);
void RenderImGui(ImVec4* clear_color, glm::vec3* lightColor);


int main() {
	GLFWwindow* window;
	window = InitWindow();
	assert(window);

	GLuint VAO;
	InitVertexConfig(&VAO, vertices);

	Shader cubeShader("shaders/lighting.vs", "shaders/lighting_cube.fs");
	Shader lightShader("shaders/lighting.vs", "shaders/lighting_light.fs");

	InitTexture("assets/crate.png", GL_TEXTURE0);
	InitTexture("assets/crate_specular.png", GL_TEXTURE1);
	InitTexture("assets/matrix.jpg", GL_TEXTURE2);

	glEnable(GL_DEPTH_TEST);

	double curX;
	double curY;
	glfwGetCursorPos(window, &curX, &curY);
	camera.Init(static_cast<float>(curX), static_cast<float>(curY));

	InitCubeShader(&cubeShader);
	InitLightShader(&lightShader);

	InitImGUI(&window);

	while (!glfwWindowShouldClose(window)) {
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		camera.Update(deltaTime);

		// clear screen
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RenderCubes(&cubeShader);
		RenderLight(&lightShader);
		RenderImGui(&clear_color, &lightColor);

		// check and call events and swap the buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

GLFWwindow* InitWindow() {
	glfwSetErrorCallback(glfw_error_callback);
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(static_cast<int>(SCREEN_WIDTH), static_cast<int>(SCREEN_HEIGHT), "LearnOpenGL w/ ImGUI", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to creat the GLFW window" << std::endl;
		glfwTerminate();
		return NULL;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return NULL;
	}

	glViewport(0, 0, static_cast<int>(SCREEN_WIDTH), static_cast<int>(SCREEN_HEIGHT));

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window, glfw_key_callback);
	glfwSetCursorPosCallback(window, glfw_mouse_callback);
	glfwSetScrollCallback(window, glfw_scroll_callback);

	return window;
}

void InitImGUI(GLFWwindow** window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(*window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
}

void InitVertexConfig(GLuint* VAO, std::vector<float> vertices) {
	glGenVertexArrays(1, VAO);
	glBindVertexArray(*VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(int), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
}

GLuint InitTexture(const char* filepath, GLenum texture_unit) {
	GLuint textureID;

	glActiveTexture(texture_unit);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(filepath, &width, &height, &nrChannels, 0);

	if (data) {
		GLenum format = GL_RED;

		if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);

	} else {
		std::cout << "Failed to load texture " << filepath << std::endl;
		exit(1);
	}

	return textureID;
}

void InitCubeShader(Shader* shader) {
	shader->use();

	shader->setMat4("modelTransform", glm::mat4(1.0f));
	shader->setMat4("viewTransform", camera.GetViewMatrix());
	shader->setMat4("perspectiveTransform", camera.GetProjectionMatrix());

	shader->setInt("material.diffuse", 0);
	shader->setInt("material.specular", 1);
	shader->setInt("material.emission", 2);
	shader->setFloat("material.shininess", 32.0f);

	shader->setVec3("light.position", lightPos);
	shader->setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
	shader->setVec3("light.diffuse", 0.5f, 0.5f, 0.5f); // darken diffuse light a bit
	shader->setVec3("light.specular", 1.0f, 1.0f, 1.0f);
	shader->setFloat("light.constant", 1.0f);
	shader->setFloat("light.linear", 0.09f);
	shader->setFloat("light.quadratic", 0.032f);

	shader->setVec3("viewerPosition", camera.GetCameraPosition());
}

void InitLightShader(Shader* shader) {
	shader->use();

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, lightPos);
	model = glm::scale(model, glm::vec3(0.2f));
	shader->setMat4("modelTransform", model);

	shader->setMat4("viewTransform", camera.GetViewMatrix());
	shader->setMat4("perspectiveTransform", camera.GetProjectionMatrix());
}


void RenderCubes(Shader* shader) {
	shader->use();

	// The camera may have moved
	shader->setVec3("viewerPosition", camera.GetCameraPosition());
	shader->setMat4("viewTransform", camera.GetViewMatrix());
	shader->setMat4("perspectiveTransform", camera.GetProjectionMatrix());

	// The light source may have moved or changed colour
	shader->setVec3("light.position", lightPos);

	glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
	glm::vec3 ambientColor = diffuseColor * glm::vec3(0.5f);
	shader->setVec3("light.ambient", ambientColor);
	shader->setVec3("light.diffuse", diffuseColor);


	for (int i = 0; i < cubePositions.size(); i++) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, cubePositions[i]);

		float angle = 20.0f * i;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

		shader->setMat4("modelTransform", model);

		Draw();
	}
}

void RenderLight(Shader* shader) {
	float radius = 4.0f;
	float angle = static_cast<float>(glfwGetTime() * 0.5);

	shader->use();

	// The camera may have moved
	shader->setVec3("viewerPosition", camera.GetCameraPosition());
	shader->setMat4("viewTransform", camera.GetViewMatrix());

	//// Rotate the light around the screen
	//glm::mat4 model = glm::mat4(1.0f);
	//lightPos.x = glm::cos(angle) * radius;
	//lightPos.z = glm::sin(angle) * radius;

	//model = glm::translate(model, lightPos);
	//model = glm::scale(model, glm::vec3(0.2f));
	//shader->setMat4("modelTransform", model);


	shader->setVec3("light.position", lightPos);
	shader->setVec3("lightColour", lightColor);

	Draw();
}

void Draw() {
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void RenderImGui(ImVec4* clear_color, glm::vec3* lightColour) {
	static float texture_mix = 0.2f;

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	if (showImGui) {
		ImGuiIO& io = ImGui::GetIO();

		ImGui::Begin("ImGui", &showImGui);
		{
			ImGui::LabelText("label", "Value");
			ImGui::ColorEdit3("Background Colour", (float*)clear_color);
			ImGui::ColorEdit3("Light Colour", (float*)lightColour);

			if (ImGui::Button("Fill")) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
			ImGui::SameLine();
			if (ImGui::Button("Lines")) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
		}
		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


/****************************************************************
 *	Callbacks
 ****************************************************************/

static void glfw_error_callback(int error, const char* description) {
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
		showImGui = !(showImGui);

		if (showImGui) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			camera.Disable();
		} else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			camera.Enable();
		}
	}

	camera.OnKeyPress(key, action);
}

void glfw_mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	camera.OnMouseMove(static_cast<float>(xpos), static_cast<float>(ypos));
}

void glfw_scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {
	camera.OnScroll(static_cast<float>(yOffset));
}