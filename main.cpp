
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "shaders/shader.h"

#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <vector>


const char* glsl_version = "#version 330";
const char* vertex_shader_file = "shaders/shader.vs";
const char* fragment_shader_file = "shaders/shader.fs";

std::vector<float> vertices = {
	// positions         // colors
	 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
	-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
	 0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
};

static void glfw_error_callback(int error, const char* description);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

GLFWwindow* InitWindow();
void InitImGUI(GLFWwindow** window);

void InitVertexConfig(GLuint* VAO, std::vector<float> vertices);

void ProcessInput(GLFWwindow* window);
void RenderTriangle();
void RenderImGui(bool* showImGui, ImVec4* clear_color);

int main() {
	GLFWwindow* window;

	GLuint VAO; 
	float greenValue;

	bool showImGui;
	ImVec4 clear_color = ImVec4(0.2f, 0.3f, 0.3f, 1.0f);

	assert(window = InitWindow());
	InitImGUI(&window);

	InitVertexConfig(&VAO, vertices);
	Shader shader(vertex_shader_file, fragment_shader_file);
	shader.use();

	while (!glfwWindowShouldClose(window)) {
		// input
		glfwPollEvents();
		ProcessInput(window);

		// clear screen
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);

		greenValue = (sin(glfwGetTime()) / 2.0f) + 0.5f;
		shader.setFloat4("greenColour", 0.0f, greenValue, 0.0f, 1.0f);

		// rendering
		RenderTriangle();
		RenderImGui(&showImGui, &clear_color);

		// check and call events and swap the buffers
		glfwSwapBuffers(window);
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

	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL w/ ImGUI", NULL, NULL);
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

	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

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


void ProcessInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

void InitVertexConfig(GLuint* VAO, std::vector<float> vertices) {

	glGenVertexArrays(1, VAO);
	glBindVertexArray(*VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
}

void RenderTriangle() {
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void RenderImGui(bool* showImGui, ImVec4* clear_color) {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// ImGui window
	if (*showImGui) {
		ImGuiIO& io = ImGui::GetIO();
		ImGui::Begin("ImGui", showImGui);
		ImGui::ColorEdit3("clear color", (float*)clear_color);
		ImGui::NewLine();
		if (ImGui::Button("Lines")) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		ImGui::SameLine();
		if (ImGui::Button("Fill")) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
	glViewport(0, 0, 800, 600);
}