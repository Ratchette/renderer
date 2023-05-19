
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "shaders/shader.h"

#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <vector>


const char* glsl_version = "#version 330";
const char* VERTEX_SHADER_FILE = "shaders/shader_vertex.glsl";
const char* FRAGMENT_SHADER_FILE = "shaders/shader_fragment.glsl";

const char* BOX_TEXTURE_FILE = "assets/container.jpg";
const char* SIMILEY_TEXTURE_FILE = "assets/awesomeface.png";

static bool showImGui = true;

std::vector<float> vertices = {
	// positions          // texture coords
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

static void glfw_error_callback(int error, const char* description);
void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

GLFWwindow* InitWindow();
void InitImGUI(GLFWwindow** window);

void InitVertexConfig(GLuint* VAO, std::vector<float> vertices);
void InitTexture(GLuint* texture, const char* filepath, GLenum format, GLenum texture_unit);
void InitTransforms(Shader* shader);

void UpdateTransforms(Shader* shader, glm::vec3 position, int index, bool rotate = false);
void ProcessInput(GLFWwindow* window);
void RenderTriangle();
void RenderImGui(ImVec4* clear_color, float* texture_mix);

int main() {
	GLFWwindow* window;

	GLuint VAO; 
	GLuint texture0;
	GLuint texture1;

	ImVec4 clear_color = ImVec4(0.2f, 0.3f, 0.3f, 1.0f);

	assert(window = InitWindow());
	InitImGUI(&window);
	glfwSetKeyCallback(window, glfw_key_callback);

	InitVertexConfig(&VAO, vertices);

	InitTexture(&texture0, BOX_TEXTURE_FILE, GL_RGB, GL_TEXTURE0);
	InitTexture(&texture1, SIMILEY_TEXTURE_FILE, GL_RGBA, GL_TEXTURE1);


	Shader shader(VERTEX_SHADER_FILE, FRAGMENT_SHADER_FILE);
	shader.use();
	shader.setInt("texture0", 0);
	shader.setInt("texture1", 1);

	float texture_mix = 0.2f;
	shader.setFloat("texture_mix", texture_mix);

	InitTransforms(&shader);

	while (!glfwWindowShouldClose(window)) {
		// input
		glfwPollEvents();
		ProcessInput(window);

		// clear screen
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// rendering
		for (int i = 0; i < cubePositions.size(); i++) {
			UpdateTransforms(&shader, cubePositions[i], i, (i%3 == 0));
			RenderTriangle();
		}
		RenderImGui(&clear_color, &texture_mix);
		shader.setFloat("texture_mix", texture_mix);

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

void InitVertexConfig(GLuint* VAO, std::vector<float> vertices) {
	glGenVertexArrays(1, VAO);
	glBindVertexArray(*VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(int), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
}

void InitTexture(GLuint* texture, const char* filepath, GLenum format, GLenum texture_unit) {
	glActiveTexture(texture_unit);
	glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_2D, *texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(filepath, &width, &height, &nrChannels, 0);
	if (!data) {
		std::cout << "Failed to load texture " << filepath << std::endl;
		exit(1);
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);
}

void InitTransforms(Shader* shader) {
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
	projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
	shader->setMat4("perspectiveTransform", projection);
}

void UpdateTransforms(Shader* shader, glm::vec3 position, int index, bool rotate) {
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);

	float angle = 20.0f * index + (rotate ? (float)glfwGetTime() : 0);
	model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
	shader->setMat4("modelTransform", model);
}


void ProcessInput(GLFWwindow* window) {}

void RenderTriangle() {
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void RenderImGui(ImVec4* clear_color, float* texture_mix) {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	if (showImGui) {
		ImGuiIO& io = ImGui::GetIO();

		ImGui::Begin("ImGui", &showImGui);
		{
			ImGui::LabelText("label", "Value");
			ImGui::ColorEdit3("Background Colour", (float*)clear_color);
			ImGui::SliderFloat("Texture Mixture", texture_mix, 0.0f, 1.0f, "ratio = %.3f");

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
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, 800, 600);
}