
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "glad/include/glad/glad.h"
#include <glfw/glfw3.h>

#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <map>
#include <vector>

#include "stb_image.h"

#include "shader.hpp"
#include "camera.hpp"
#include "model.hpp"

const char* glsl_version = "#version 330";
const float SCREEN_WIDTH = 1200.0f;
const float SCREEN_HEIGHT = 900.0f;

bool showImGui = false;
static ImVec4 clear_color = ImVec4(0.07f, 0.11f, 0.11f, 1.0f);
static glm::vec3 globalLightColour(1.0);
static glm::vec3 pointLightColour(0.584f, 0.475f, 0.173f);
static glm::vec3 flashlightColour(0.455, 0.375, 0.824);

Camera camera;
glm::vec3 lightDirection(16.3, -11.0f, 20.0f);
vector<glm::vec3> pointLightPositions = {
	glm::vec3(0.7f,  0.2f,  2.0f),
	glm::vec3(2.3f, 4.0f, 0.0f),
	glm::vec3(-4.0f,  2.0f, -7.0f),
	glm::vec3(0.0f,  1.5f, -1.75f)
};
vector<float> cubeVertices = {
	// positions          // texture coords
	// Back face
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right         
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
	// Front face
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
	// Left face
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
	// Right face
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right         
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left     
	 // Bottom face
	 -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
	  0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
	  0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
	  0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
	 -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
	 -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
	 // Top face
	 -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
	  0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
	  0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right     
	  0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
	 -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
	 -0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // bottom-left        
};

vector<float> skyboxVertices = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};
vector<string> skyboxPaths = {
	"assets/skybox/right.jpg",
	"assets/skybox/left.jpg",
	"assets/skybox/top.jpg",
	"assets/skybox/bottom.jpg",
	"assets/skybox/front.jpg",
	"assets/skybox/back.jpg"
};

static float deltaTime = 0.0f;
static float lastFrame = 0.0f;


static void glfw_error_callback(int error, const char* description);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void glfw_mouse_callback(GLFWwindow* window, double xpos, double ypos);
void glfw_scroll_callback(GLFWwindow* window, double xOffset, double yOffset);

GLFWwindow* InitWindow();
void InitVertices(GLuint& VAO, vector<float> vertices, int numAttrs);
void InitCustomFramebuffer(GLuint& framebuffer, GLuint& textureColorBuffer);

unsigned int LoadTexture(char const* path);
unsigned int LoadSkybox(vector<string> faces);

void InitFloorShader(Shader* shader);
void InitPointLightsShader(Shader* shader);
void InitGuitarShader(Shader* shader);
void InitSkyboxShader(Shader* shader);


void RenderFloor(Shader& shader, GLuint& VAO, GLuint& texture);
void RenderPointLights(Shader& shader, GLuint& VAO);
void RenderGuitar(Shader& shader, Model& model);
void RenderOutline(Shader& shader, GLuint& VAO, glm::vec3& colour);
void RenderCustomFramebuffer(Shader& shader, GLuint& VAO, GLuint& texture);
void RenderSkybox(Shader& shader, GLuint& VAO, GLuint& texture);

void InitImGUI(GLFWwindow** window);
void RenderImGui(ImVec4* clear_color);


int main() {
	GLFWwindow* window;
	window = InitWindow();
	assert(window);

	Shader lightsShader("shaders/basic.vert", "shaders/basic_colour.frag");

	Shader guitarShader("shaders/backpack.vert", "shaders/backpack.frag");
	Model guitarModel("models/backpack/backpack.obj");

	Shader skyboxShader("shaders/skybox.vert", "shaders/skybox.frag");
	unsigned int skyboxTexture = LoadSkybox(skyboxPaths);

	double curX;
	double curY;
	glfwGetCursorPos(window, &curX, &curY);
	camera.Init(static_cast<float>(curX), static_cast<float>(curY));

	GLuint lightsVAO, skyboxVAO;
	InitVertices(lightsVAO, cubeVertices, 5);
	InitVertices(skyboxVAO, skyboxVertices, 3);

	InitSkyboxShader(&skyboxShader);
	InitPointLightsShader(&lightsShader);
	InitGuitarShader(&guitarShader);
	InitImGUI(&window);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	while (!glfwWindowShouldClose(window)) {
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		camera.Update(deltaTime);

		// Render to custom framebuffer
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		
		// Draw Scene
		RenderPointLights(lightsShader, lightsVAO);
		RenderGuitar(guitarShader, guitarModel);
		RenderSkybox(skyboxShader, skyboxVAO, skyboxTexture);

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

void InitVertices(GLuint& VAO, vector<float> vertices, int numAttrs) {
	GLuint VBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, numAttrs * sizeof(float), (void*)0);

	if (numAttrs > 3) {
		// Texture coordinates
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, numAttrs * sizeof(float), (void*)(3 * sizeof(float)));
	}

	glBindVertexArray(0);
}

void InitCustomFramebuffer(GLuint& framebuffer, GLuint& textureColorBuffer) {
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// Create a texture for colours
	glGenTextures(1, &textureColorBuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, static_cast<int>(SCREEN_WIDTH), static_cast<int>(SCREEN_HEIGHT), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);

	// Create a renderbuffer for depth & stencil
	GLuint rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, static_cast<int>(SCREEN_WIDTH), static_cast<int>(SCREEN_HEIGHT));
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR::FRAMEBUFFER:: framebuffer is incomplete." << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void InitFloorShader(Shader* shader) {
	shader->use();

	shader->setMat4("modelTransform", glm::mat4(1.0f));
	shader->setMat4("viewTransform", camera.GetViewMatrix());
	shader->setMat4("perspectiveTransform", camera.GetProjectionMatrix());

	shader->setInt("texture0", 0);
}

void InitPointLightsShader(Shader* shader) {
	shader->use();

	shader->setMat4("viewTransform", camera.GetViewMatrix());
	shader->setMat4("perspectiveTransform", camera.GetProjectionMatrix());

	shader->setVec3("colour", pointLightColour);
}

void InitGuitarShader(Shader* shader) {
	shader->use();

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 1.25f, 0.0f)); // translate it down so it's at the center of the scene

	shader->setMat4("modelTransform", model);
	shader->setMat4("viewTransform", camera.GetViewMatrix());
	shader->setMat4("perspectiveTransform", camera.GetProjectionMatrix());

	shader->setFloat("specular_shininess", 32.0f);

	shader->setVec3("directionalLight.direction", lightDirection);
	shader->setVec3("directionalLight.ambient", globalLightColour * 0.05f);
	shader->setVec3("directionalLight.diffuse", globalLightColour * 0.6f);
	shader->setVec3("directionalLight.specular", globalLightColour);

	// point light 1
	shader->setVec3("pointLight[0].position", pointLightPositions[0]);
	shader->setVec3("pointLight[0].diffuse", pointLightColour * 0.8f);
	shader->setVec3("pointLight[0].specular", pointLightColour);
	shader->setFloat("pointLight[0].constant", 1.0f);
	shader->setFloat("pointLight[0].linear", 0.09f);
	shader->setFloat("pointLight[0].quadratic", 0.032f);
	// point light 2
	shader->setVec3("pointLight[1].position", pointLightPositions[1]);
	shader->setVec3("pointLight[1].diffuse", pointLightColour * 0.8f);
	shader->setVec3("pointLight[1].specular", pointLightColour);
	shader->setFloat("pointLight[1].constant", 1.0f);
	shader->setFloat("pointLight[1].linear", 0.09f);
	shader->setFloat("pointLight[1].quadratic", 0.032f);
	// point light 3
	shader->setVec3("pointLight[2].position", pointLightPositions[2]);
	shader->setVec3("pointLight[2].diffuse", pointLightColour * 0.8f);
	shader->setVec3("pointLight[2].specular", pointLightColour);
	shader->setFloat("pointLight[2].constant", 1.0f);
	shader->setFloat("pointLight[2].linear", 0.09f);
	shader->setFloat("pointLight[2].quadratic", 0.032f);
	// point light 4
	shader->setVec3("pointLight[3].position", pointLightPositions[3]);
	shader->setVec3("pointLight[3].diffuse", pointLightColour * 0.8f);
	shader->setVec3("pointLight[3].specular", pointLightColour);
	shader->setFloat("pointLight[3].constant", 1.0f);
	shader->setFloat("pointLight[3].linear", 0.09f);
	shader->setFloat("pointLight[3].quadratic", 0.032f);

	shader->setVec3("spotLight.position", camera.GetPosition());
	shader->setVec3("spotLight.direction", camera.GetDirection());
	shader->setVec3("spotLight.ambient", glm::vec3(0.0f));
	shader->setVec3("spotLight.diffuse", flashlightColour * 0.8f);
	shader->setVec3("spotLight.specular", flashlightColour);
	shader->setFloat("spotLight.constant", 1.0f);
	shader->setFloat("spotLight.linear", 0.09f);
	shader->setFloat("spotLight.quadratic", 0.032f);
	shader->setFloat("spotLight.beamWidth", static_cast<float>(glm::cos(glm::radians(12.5))));
	shader->setFloat("spotLight.outerCutoff", static_cast<float>(glm::cos(glm::radians(17.5))));

	shader->setVec3("viewerPosition", camera.GetPosition());
}

void InitSkyboxShader(Shader* shader) {
	shader->use();
	
	shader->setMat4("viewTransform", glm::mat4(glm::mat3(camera.GetViewMatrix())));
	shader->setMat4("perspectiveTransform", camera.GetProjectionMatrix());
}

void RenderFloor(Shader& shader, GLuint& VAO, GLuint& texture) {
	shader.use();
	glBindVertexArray(VAO);
	glBindTexture(GL_TEXTURE_2D, texture);

	// The camera may have moved
	shader.setMat4("modelTransform", glm::mat4(1.0f));
	shader.setMat4("viewTransform", camera.GetViewMatrix());
	shader.setMat4("perspectiveTransform", camera.GetProjectionMatrix());

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void RenderGuitar(Shader& shader, Model& model) {
	shader.use();

	shader.setVec3("viewerPosition", camera.GetPosition());
	shader.setMat4("viewTransform", camera.GetViewMatrix());
	shader.setMat4("perspectiveTransform", camera.GetProjectionMatrix());

	shader.setVec3("directionalLight.ambient", globalLightColour * 0.1f);
	shader.setVec3("directionalLight.diffuse", globalLightColour * 0.8f);
	shader.setVec3("directionalLight.specular", globalLightColour);

	shader.setVec3("spotLight.position", camera.GetPosition());
	shader.setVec3("spotLight.direction", camera.GetDirection());
	shader.setVec3("spotLight.diffuse", flashlightColour * 0.8f);
	shader.setVec3("spotLight.specular", flashlightColour);

	shader.setVec3("pointLight[0].diffuse", pointLightColour * 0.8f);
	shader.setVec3("pointLight[0].specular", pointLightColour);
	shader.setVec3("pointLight[1].diffuse", pointLightColour * 0.8f);
	shader.setVec3("pointLight[1].specular", pointLightColour);
	shader.setVec3("pointLight[2].diffuse", pointLightColour * 0.8f);
	shader.setVec3("pointLight[2].specular", pointLightColour);
	shader.setVec3("pointLight[3].diffuse", pointLightColour * 0.8f);
	shader.setVec3("pointLight[3].specular", pointLightColour);

	model.Draw(shader);
}

void RenderPointLights(Shader& shader, GLuint& VAO) {
	shader.use();
	glBindVertexArray(VAO);

	shader.setVec3("colour", pointLightColour);

	shader.setMat4("viewTransform", camera.GetViewMatrix());
	shader.setMat4("perspectiveTransform", camera.GetProjectionMatrix());

	float radius = 4.0f;
	float angle = static_cast<float>(glfwGetTime() * 0.5);

	for (int i = 0; i < pointLightPositions.size(); i++) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, pointLightPositions[i]);
		model = glm::scale(model, glm::vec3(0.2f));
		shader.setMat4("modelTransform", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	//// Rotate the light around the screen
	//glm::mat4 model = glm::mat4(1.0f);
	//lightPos.x = glm::cos(angle) * radius;
	//lightPos.z = glm::sin(angle) * radius;

	//model = glm::translate(model, lightPos);
	//model = glm::scale(model, glm::vec3(0.2f));
	//shader->setMat4("modelTransform", model);

	/*shader->setVec3("light.position", lightPos);
	shader->setVec3("lightColour", lightColor);*/

}

void RenderOutline(Shader& shader, GLuint& VAO, glm::vec3& colour) {
	// Don't apply face culling to default framebuffer
	//		At the moment the default framebuffer is a quad, so culling faces doesn't make sense in this context
	bool isCullingEnabled = glIsEnabled(GL_CULL_FACE);
	if (isCullingEnabled) {
		glDisable(GL_CULL_FACE);
	}

	shader.use();
	shader.setVec3("colour", colour);

	glBindVertexArray(VAO);

	glDisable(GL_DEPTH_TEST);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	if (isCullingEnabled) {
		glEnable(GL_CULL_FACE);
	}
}

void RenderCustomFramebuffer(Shader& shader, GLuint& VAO, GLuint& texture) {
	// Don't apply face culling to default framebuffer
	//		At the moment the default framebuffer is a quad, so culling faces doesn't make sense in this context
	bool isCullingEnabled = glIsEnabled(GL_CULL_FACE);
	if (isCullingEnabled) {
		glDisable(GL_CULL_FACE);
	}

	shader.use();
	glBindVertexArray(VAO);

	glDisable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, texture);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	if (isCullingEnabled) {
		glEnable(GL_CULL_FACE);
	}
}

void RenderSkybox(Shader& shader, GLuint& VAO, GLuint& texture) {
	glDepthFunc(GL_LEQUAL);
	shader.use();
	glBindVertexArray(VAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	shader.setMat4("viewTransform", glm::mat4(glm::mat3(camera.GetViewMatrix())));
	shader.setMat4("perspectiveTransform", camera.GetProjectionMatrix());

	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
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
			if (ImGui::CollapsingHeader("Lighting")) {
				ImGui::LabelText("label", "Value");
				ImGui::ColorEdit3("Background", (float*)clear_color);
				ImGui::ColorEdit3("Directional light", &globalLightColour[0]);
				ImGui::ColorEdit3("Point lights", &pointLightColour[0]);
				ImGui::ColorEdit3("Flashlight", &flashlightColour[0]);
			}

			if (ImGui::CollapsingHeader("Culling")) {
				if (ImGui::Button("No Culling")) {
					glDisable(GL_CULL_FACE);
				}
				ImGui::SameLine();
				if (ImGui::Button("Cull Back Faces")) {
					glEnable(GL_CULL_FACE);
					glCullFace(GL_BACK);
				}
				ImGui::SameLine();
				if (ImGui::Button("Cull Front Faces")) {
					glEnable(GL_CULL_FACE);
					glCullFace(GL_FRONT);
				}
			}

			ImGui::Text("Unsorted options");
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

	stbi_set_flip_vertically_on_load(false);
	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format = GL_RED;
		if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else {
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

unsigned int LoadSkybox(vector<string> faces) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	stbi_set_flip_vertically_on_load(false);
	int width, height, nrComponents;

	for (int i = 0; i < faces.size(); ++i) {
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		} else {
			std::cout << "Cubemap failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

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
	if (key == GLFW_KEY_DELETE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
		showImGui = !(showImGui);

		if (showImGui) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			camera.Disable();
		}
		else {
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
