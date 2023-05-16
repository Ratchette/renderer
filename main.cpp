
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "shaders/shader.h"

#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <vector>


const char* glsl_version = "#version 330";
const char* vertex_shader_file = "shaders/shader_vertex.glsl";
const char* fragment_shader_file = "shaders/shader_fragment.glsl";

std::vector<float> vertices = {
	// positions          // colors           // texture coords
	 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
	 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
	-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
	-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
};

std::vector<int> indices = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
};

static void glfw_error_callback(int error, const char* description);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

GLFWwindow* InitWindow();
void InitImGUI(GLFWwindow** window);

void InitVertexConfig(GLuint* VAO, std::vector<float> vertices, std::vector<int> indices);
void InitTexture(GLuint* texture);

void ProcessInput(GLFWwindow* window);
void RenderTriangle();
void RenderImGui(bool* showImGui, ImVec4* clear_color);

int main() {
	GLFWwindow* window;

	GLuint VAO; 
	GLuint texture;

	bool showImGui;
	ImVec4 clear_color = ImVec4(0.2f, 0.3f, 0.3f, 1.0f);

	assert(window = InitWindow());
	InitImGUI(&window);

	InitVertexConfig(&VAO, vertices, indices);
	InitTexture(&texture);
	Shader shader(vertex_shader_file, fragment_shader_file);
	shader.use();

	while (!glfwWindowShouldClose(window)) {
		// input
		glfwPollEvents();
		ProcessInput(window);

		// clear screen
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);

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

void InitVertexConfig(GLuint* VAO, std::vector<float> vertices, std::vector<int> indices) {

	glGenVertexArrays(1, VAO);
	glBindVertexArray(*VAO);

	GLuint EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(float), indices.data(), GL_STATIC_DRAW);

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

void InitTexture(GLuint* texture) {
	const char* TEXTURE_FILEPATH = "assets/container.jpg";

	glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_2D, *texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(TEXTURE_FILEPATH, &width, &height, &nrChannels, 0);
	if (!data) {
		std::cout << "Failed to load texture " << TEXTURE_FILEPATH << std::endl;
		exit(1);
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);


}

void ProcessInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

void RenderTriangle() {
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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