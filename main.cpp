
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

#include "shader.hpp"
#include "camera.hpp"
#include "model.hpp"

const char* glsl_version = "#version 330";
const float SCREEN_HEIGHT = 600.0f;
const float SCREEN_WIDTH = 800.0f;

static bool showImGui = false;
static ImVec4 clear_color = ImVec4(0.07f, 0.11f, 0.11f, 1.0f);
static glm::vec3 globalLightColour(1.0);
//static glm::vec3 globalLightColour(0.455, 0.375, 0.824);

Camera camera;
glm::vec3 lightDirection(-0.2f, -1.0f, -0.3f);

static float deltaTime = 0.0f;
static float lastFrame = 0.0f;


static void glfw_error_callback(int error, const char* description);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void glfw_mouse_callback(GLFWwindow* window, double xpos, double ypos);
void glfw_scroll_callback(GLFWwindow* window, double xOffset, double yOffset);

GLFWwindow* InitWindow();
void InitShader(Shader* shader);
void Render(Shader& shader, Model& model);

void InitImGUI(GLFWwindow** window);
void RenderImGui(ImVec4* clear_color, glm::vec3* lightColor);


int main() {
	GLFWwindow* window;
	window = InitWindow();
	assert(window);

	glEnable(GL_DEPTH_TEST);
	Shader shader("shaders/backpack.vs", "shaders/backpack.fs");
	Model model("models/backpack/backpack.obj");

	double curX;
	double curY;
	glfwGetCursorPos(window, &curX, &curY);
	camera.Init(static_cast<float>(curX), static_cast<float>(curY));

	InitShader(&shader);
	InitImGUI(&window);

	while (!glfwWindowShouldClose(window)) {
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		camera.Update(deltaTime);

		// clear screen
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Render(shader, model);
		RenderImGui(&clear_color, &globalLightColour);

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

void InitShader(Shader* shader) {
	shader->use();

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));		// it's a bit too big for our scene, so scale it down

	shader->setMat4("modelTransform", model);
	shader->setMat4("viewTransform", camera.GetViewMatrix());
	shader->setMat4("perspectiveTransform", camera.GetProjectionMatrix());

	//shader->setInt("material.diffuse", 0);
	//shader->setInt("material.specular", 1);
	//shader->setInt("material.emission", 2);
	shader->setFloat("specular_shininess", 32.0f);

	shader->setVec3("directionalLight.direction", -0.2f, -1.0f, -0.3f);
	shader->setVec3("directionalLight.ambient", globalLightColour * 0.1f);
	shader->setVec3("directionalLight.diffuse", globalLightColour * 0.8f);
	shader->setVec3("directionalLight.specular", globalLightColour);

	shader->setVec3("viewerPosition", camera.GetPosition());

	shader->use();
}


void Render(Shader& shader, Model& model) {
	shader.use();

	// The camera may have moved
	shader.setVec3("viewerPosition", camera.GetPosition());
	shader.setMat4("viewTransform", camera.GetViewMatrix());
	shader.setMat4("perspectiveTransform", camera.GetProjectionMatrix());

	shader.setVec3("directionalLight.ambient", globalLightColour * 0.1f);
	shader.setVec3("directionalLight.diffuse", globalLightColour * 0.8f);
	shader.setVec3("directionalLight.specular", globalLightColour);

	model.Draw(shader);
}

void RenderImGui(ImVec4* clear_color, glm::vec3* directionalLightColour) {
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
			ImGui::ColorEdit3("Directional light", (float*)directionalLightColour);

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
