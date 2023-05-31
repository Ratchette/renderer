
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <vector>

#include "stb_image.h"

#include "shader.hpp"
#include "camera.hpp"
#include "model.hpp"

const char* glsl_version = "#version 330";
const float SCREEN_HEIGHT = 600.0f;
const float SCREEN_WIDTH = 800.0f;

static bool showImGui = false;
static ImVec4 clear_color = ImVec4(0.07f, 0.11f, 0.11f, 1.0f);
static glm::vec3 globalLightColour(1.0);
static glm::vec3 outlineColour(0.0);

Camera camera;
glm::vec3 lightDirection(-0.2f, -1.0f, -0.3f);

static float deltaTime = 0.0f;
static float lastFrame = 0.0f;

vector<float> crateVertices = {
	// positions          // texture Coords
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

vector<glm::vec3> cratePositions = {
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

vector<float> planeVertices = {
	// positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
	 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
	-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
	-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

	 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
	-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
	 5.0f, -0.5f, -5.0f,  2.0f, 2.0f
};

static void glfw_error_callback(int error, const char* description);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void glfw_mouse_callback(GLFWwindow* window, double xpos, double ypos);
void glfw_scroll_callback(GLFWwindow* window, double xOffset, double yOffset);

GLFWwindow* InitWindow();
void InitVertices(GLuint& VAO, vector<float> vertices);
void InitCratesShader(Shader* shader);
void InitOutlineShader(Shader* shader);
unsigned int LoadTexture(char const* path);

void RenderPlane(Shader& shader, GLuint& VAO, GLuint& texture);
void RenderCrates(Shader& shader, GLuint& VAO, GLuint& texture);
void RenderOutline(Shader& shader);

void InitImGUI(GLFWwindow** window);
void RenderImGui(ImVec4* clear_color);


int main() {
	GLFWwindow* window;
	window = InitWindow();
	assert(window);

	Shader cratesShader("shaders/crate.vs", "shaders/crate.fs");
	Shader outlineShader("shaders/crate.vs", "shaders/outline.fs");

	unsigned int crateTexture = LoadTexture("assets/crate.png");
	unsigned int metalTexture = LoadTexture("assets/metal.png");
	cratesShader.setInt("texture0", 0);

	double curX;
	double curY;
	glfwGetCursorPos(window, &curX, &curY);
	camera.Init(static_cast<float>(curX), static_cast<float>(curY));

	GLuint cratesVAO, planeVAO;
	InitVertices(cratesVAO, crateVertices);
	InitVertices(planeVAO, planeVertices);

	InitCratesShader(&cratesShader);
	InitOutlineShader(&outlineShader);
	InitImGUI(&window);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	while (!glfwWindowShouldClose(window)) {
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		camera.Update(deltaTime);

		// clear screen
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		{ // Draw the floor (plane)
			glStencilMask(0x00);
			// the cratesShader is basic enough to re-use here
			RenderPlane(cratesShader, planeVAO, metalTexture);
		}

		{ // Draw the guitar
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xFF);
			RenderCrates(cratesShader, cratesVAO, crateTexture);
		}

		{ // Draw the outline
			glDisable(GL_DEPTH_TEST);

			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			glStencilMask(0x00);

			RenderOutline(outlineShader);

			glEnable(GL_DEPTH_TEST);

			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xFF);
		}

		RenderImGui(&clear_color);

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
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
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

void InitVertices(GLuint& VAO, vector<float> vertices) {
	GLuint VBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glBindVertexArray(0);
}

void InitCratesShader(Shader* shader) {
	shader->use();

	shader->setMat4("modelTransform", glm::mat4(1.0f));
	shader->setMat4("viewTransform", camera.GetViewMatrix());
	shader->setMat4("perspectiveTransform", camera.GetProjectionMatrix());

	shader->setInt("texture0", 0);
}

void InitOutlineShader(Shader* shader) {
	shader->use();

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(1.1f, 1.1f, 1.1f));
	shader->setMat4("modelTransform", model);

	shader->setMat4("viewTransform", camera.GetViewMatrix());
	shader->setMat4("perspectiveTransform", camera.GetProjectionMatrix());

	shader->setVec3("outlineColour", outlineColour);
}

void RenderPlane(Shader& shader, GLuint& VAO, GLuint& texture) {
	shader.use();
	glBindVertexArray(VAO);
	glBindTexture(GL_TEXTURE_2D, texture);

	// The camera may have moved
	shader.setMat4("modelTranform", glm::mat4(1.0f));
	shader.setMat4("viewTransform", camera.GetViewMatrix());
	shader.setMat4("perspectiveTransform", camera.GetProjectionMatrix());
	shader.setVec3("viewerPosition", camera.GetPosition());

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void RenderCrates(Shader& shader, GLuint& VAO, GLuint& texture) {
	shader.use();
	glBindVertexArray(VAO);
	glBindTexture(GL_TEXTURE_2D, texture);

	// The camera may have moved
	shader.setVec3("viewerPosition", camera.GetPosition());
	shader.setMat4("viewTransform", camera.GetViewMatrix());
	shader.setMat4("perspectiveTransform", camera.GetProjectionMatrix());

	// Crates
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
	shader.setMat4("modelTransform", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
	shader.setMat4("modelTransform", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void RenderOutline(Shader& shader) {
	shader.use();

	shader.setVec3("outlineColour", outlineColour);

	// The camera may have moved
	shader.setVec3("viewerPosition", camera.GetPosition());
	shader.setMat4("viewTransform", camera.GetViewMatrix());
	shader.setMat4("perspectiveTransform", camera.GetProjectionMatrix());

	// Crates
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
	model = glm::scale(model, glm::vec3(1.1f));
	shader.setMat4("modelTransform", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.1f));
	shader.setMat4("modelTransform", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void RenderImGui(ImVec4* clear_color) {
	static float texture_mix = 0.2f;

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	if (showImGui) {
		ImGuiIO& io = ImGui::GetIO();

		ImGui::Begin("ImGui", &showImGui);
		{
			ImGui::LabelText("label", "Value");
			ImGui::ColorEdit3("Background", (float*)clear_color);
			ImGui::ColorEdit3("Directional light", &globalLightColour[0]);
			ImGui::ColorEdit3("Outline colour", &outlineColour[0]);

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

unsigned int LoadTexture(char const* path) {
	unsigned int textureID;
	glGenTextures(1, &textureID);

	stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	} else {
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}


/****************************************************************
 *	Callbacks
 ****************************************************************/

static void glfw_error_callback(int error, const char* description) {
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
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
