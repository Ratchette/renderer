
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <vector>


const char* glsl_version = "#version 330";

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char* fragmentShaderSource_orange = "#version 330 core\n"
"out vec4 FragColour;\n"
"void main()\n"
"{\n"
"   FragColour = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\0";

const char* fragmentShaderSource_yellow = "#version 330 core\n"
"out vec4 FragColour;\n"
"void main()\n"
"{\n"
"   FragColour = vec4(1.0f, 1.0f, 0.0f, 1.0f);\n"
"}\0";

static void glfw_error_callback(int error, const char* description);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

GLFWwindow* InitWindow();
void InitImGUI(GLFWwindow** window);

void InitVertexConfig(GLuint* VAO, std::vector<float> vertices);
void InitShaders(GLuint* shaderProgram, const char* vertexShaderSource, const char* fragmentShaderSource);

void ProcessInput(GLFWwindow* window);
void RenderTriangle(GLuint* VAO, GLuint* shaderProgram);
void RenderImGui(bool* showImGui, ImVec4* clear_color);

int main() {
	GLFWwindow* window;

	assert(window = InitWindow());
	InitImGUI(&window);

	GLuint VAO1;
	GLuint VAO2;
	std::vector<float> vertices1{
		-0.8f,  -0.5f, 0.0f,
		-0.4f,   0.5f, 0.0f,
		 0.0f,  -0.5f, 0.0f
	};
	std::vector<float> vertices2{
		0.0f,  -0.5f, 0.0f,
		0.4f,   0.5f, 0.0f,
		0.8f,  -0.5f, 0.0f
	};

	GLuint shaderProgram_orange;
	GLuint shaderProgram_yellow;

	InitVertexConfig(&VAO1, vertices1);
	InitVertexConfig(&VAO2, vertices2);
	InitShaders(&shaderProgram_orange, vertexShaderSource, fragmentShaderSource_orange);
	InitShaders(&shaderProgram_yellow, vertexShaderSource, fragmentShaderSource_yellow);

	bool showImGui;
	ImVec4 clear_color = ImVec4(0.2f, 0.3f, 0.3f, 1.0f);

	while (!glfwWindowShouldClose(window)) {
		// input
		glfwPollEvents();
		ProcessInput(window);

		// clear screen
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);

		// rendering
		RenderTriangle(&VAO1, &shaderProgram_orange);
		RenderTriangle(&VAO2, &shaderProgram_yellow);
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}


void InitShaders(GLuint* shaderProgram, const char* vertexShaderSource, const char* fragmentShaderSource) {
	GLuint vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		exit(1);
	}

	GLuint fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		exit(1);
	}

	*shaderProgram = glCreateProgram();
	glAttachShader(*shaderProgram, vertexShader);
	glAttachShader(*shaderProgram, fragmentShader);
	glLinkProgram(*shaderProgram);

	glGetProgramiv(*shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(*shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		exit(1);
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void RenderTriangle(GLuint* VAO, GLuint* shaderProgram) {
	glUseProgram(*shaderProgram);
	glBindVertexArray(*VAO);
	glUseProgram(*shaderProgram);
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