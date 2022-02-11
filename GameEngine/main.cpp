#include "glad/glad.h"

#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"
#include "imgui.h"
#include "GLFW/glfw3.h"

#include "main.h"
#include "shader.h"
#include "camera.h"
#include "model.h"
#include <iostream>
#include "stb_image.h"
#include <map>
#include <iterator>

#define GLM_FORCE_CXX17

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/trigonometric.hpp>

float mixValue = 0.2f;
double deltaTime = 0.0f;	// Time between current frame and last frame
double lastFrame = 0.0f; // Time of last frame
double lastX = 400, lastY = 300;
bool firstMouse = true;
glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
float yaw;
float pitch;

Camera cam;

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	float my_color[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	static bool closeWindow = true;
	
	GLFWwindow* window = glfwCreateWindow(800, 600, "GameEngine", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//glViewport(0, 0, 400, 400);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to init GLAD" << std::endl;
		return EXIT_FAILURE;
	};

	stbi_set_flip_vertically_on_load(true);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// all fragments should pass the stencil test
	


	Shader basic_lighting("basic_lighting.vs", "basic_lighting.fs");
	Shader light_cube("light_cube.vs", "light_cube.fs");
	Shader model_shader("model.vs", "model.fs");
	Shader stencil_test("singlecolor.vs", "singlecolor.fs");

	glm::vec3 cubePositions[] = {
	
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

	// positions of the point lights
	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  1.2f,  2.0f),
		glm::vec3(-0.7f, 1.2f, 2.0f),
		glm::vec3(-0.7f, -1.6f, 2.0f),
		glm::vec3(0.7f,  -1.6f, 2.0f)
	};

	glm::vec3 pointLightColors[] = {
		glm::vec3(1.0f, 0.6f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 1.0, 0.0),
		glm::vec3(0.2f, 0.2f, 1.0f)
	};

	std::vector<glm::vec3> backpackPosition =
	{
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(-2.0f, 0.0f, -4.0f),
		glm::vec3(-4.0f, 0.0f, -8.0f),
		glm::vec3(-6.0f, 0.0f, -12.0f)
	};


	Model backpack("resources/models/backpack.obj");
	Model test_cube("resources/models/cube.obj");

	//unsigned int diffuseMap = load_texture("container2.png");
	//unsigned int specularMap = load_texture("container2_specular.png");
	//unsigned int emissionMap = load_texture("emis.png");

	//basic_lighting.use();
	//basic_lighting.setInt("material.diffuse", 0);
	//basic_lighting.setInt("material.specular", 1);
	//basic_lighting.setInt("material.emission", 2);
	

	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();


	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		//render order
		
		glClearColor(0.75f, 0.52f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		
		glm::mat4 view;
		view = cam.GetViewMatrix();
		glm::mat4 proj = glm::perspective(glm::radians(cam.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);
		stencil_test.setMat4("proj", proj);
		stencil_test.setMat4("view", view);

		

		basic_lighting.use();

		basic_lighting.setMat4("view", view);
		
		basic_lighting.setMat4("proj", proj);
		

		
		basic_lighting.setVec3("viewPos", cam.Position);
		basic_lighting.setFloat("material.shininess", 32.0f);

		basic_lighting.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		basic_lighting.setVec4("dirLight.ambient", 0.3f, 0.24f, 0.14f, 1.0f);
		basic_lighting.setVec4("dirLight.diffuse", 0.7f, 0.42f, 0.26f, 1.0f);
		basic_lighting.setVec4("dirLight.specular", 0.5f, 0.5f, 0.5f, 1.0f);
		// point light 1
		basic_lighting.setVec3("pointLights[0].position", pointLightPositions[0]);
		basic_lighting.setVec3("pointLights[0].ambient", pointLightColors[0].x * 0.1, pointLightColors[0].y * 0.1, pointLightColors[0].z * 0.1);
		basic_lighting.setVec3("pointLights[0].diffuse", pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
		basic_lighting.setVec3("pointLights[0].specular", pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
		basic_lighting.setFloat("pointLights[0].constant", 1.0f);
		basic_lighting.setFloat("pointLights[0].linear", 0.09);
		basic_lighting.setFloat("pointLights[0].quadratic", 0.032);
		// point light 2
		basic_lighting.setVec3("pointLights[1].position", pointLightPositions[1]);
		basic_lighting.setVec3("pointLights[1].ambient", pointLightColors[1].x * 0.1, pointLightColors[1].y * 0.1, pointLightColors[1].z * 0.1);
		basic_lighting.setVec3("pointLights[1].diffuse", pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z);
		basic_lighting.setVec3("pointLights[1].specular", pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z);
		basic_lighting.setFloat("pointLights[1].constant", 1.0f);
		basic_lighting.setFloat("pointLights[1].linear", 0.09);
		basic_lighting.setFloat("pointLights[1].quadratic", 0.032);
		// point light 3
		basic_lighting.setVec3("pointLights[2].position", pointLightPositions[2]);
		basic_lighting.setVec3("pointLights[2].ambient", pointLightColors[2].x * 0.1, pointLightColors[2].y * 0.1, pointLightColors[2].z * 0.1);
		basic_lighting.setVec3("pointLights[2].diffuse", pointLightColors[2].x, pointLightColors[2].y, pointLightColors[2].z);
		basic_lighting.setVec3("pointLights[2].specular", pointLightColors[2].x, pointLightColors[2].y, pointLightColors[2].z);
		basic_lighting.setFloat("pointLights[2].constant", 1.0f);
		basic_lighting.setFloat("pointLights[2].linear", 0.09);
		basic_lighting.setFloat("pointLights[2].quadratic", 0.032);
		// point light 4
		basic_lighting.setVec3("pointLights[3].position", pointLightPositions[3]);
		basic_lighting.setVec3("pointLights[3].ambient", pointLightColors[3].x * 0.1, pointLightColors[3].y * 0.1, pointLightColors[3].z * 0.1);
		basic_lighting.setVec3("pointLights[3].diffuse", pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z);
		basic_lighting.setVec3("pointLights[3].specular", pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z);
		basic_lighting.setFloat("pointLights[3].constant", 1.0f);
		basic_lighting.setFloat("pointLights[3].linear", 0.09);
		basic_lighting.setFloat("pointLights[3].quadratic", 0.032);
		// spotLight
		basic_lighting.setVec3("spotLight.position", cam.Position);
		basic_lighting.setVec3("spotLight.direction", cam.Front);
		basic_lighting.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		basic_lighting.setVec3("spotLight.diffuse", 0.8f, 0.8f, 0.0f);
		basic_lighting.setVec3("spotLight.specular", 0.8f, 0.8f, 0.0f);
		basic_lighting.setFloat("spotLight.constant", 1.0f);
		basic_lighting.setFloat("spotLight.linear", 0.09);
		basic_lighting.setFloat("spotLight.quadratic", 0.032);
		basic_lighting.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		basic_lighting.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
		

		basic_lighting.setFloat("alpha", 0.4f);
		
		

		//glm::vec3 lightpos(1.2f, 1.0f, 2.1f);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);

		std::map<float, glm::vec3> sorted;
		for (int i = 0; i < backpackPosition.size(); i++)
		{
			float distance = glm::distance(cam.Position,backpackPosition[i]);
			sorted[distance] = backpackPosition[i];
		}

		for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
		{
			glm::mat4 model = glm::mat4(1.0f);

			model = glm::translate(model, it->second);
			//model = glm::rotate(model, 360.0f, glm::vec3(sin(glfwGetTime()) * 10.0f, 0.0f, cos(glfwGetTime()) * 10.0f));
			basic_lighting.setMat4("model", model);

			backpack.Draw(basic_lighting);
		}

		

		//glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		//glStencilMask(0x00);
		//glDisable(GL_DEPTH_TEST);
		//stencil_test.use();



		//model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(0.0, 0.0f, 1.0f));
		//model = glm::scale(model, glm::vec3(1.05f, 1.05f, 1.05f));
		////model = glm::rotate(model, 360.0f, glm::vec3(sin(glfwGetTime()) * 10.0f, 0.0f, cos(glfwGetTime()) * 10.0f));
		//stencil_test.setMat4("model", model);
		////
		//

		//backpack.Draw(stencil_test);

		//glStencilMask(0xFF);
		//glStencilFunc(GL_ALWAYS, 1, 0xFF);
		//glEnable(GL_DEPTH_TEST);
		 // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

		// We don't need light cube for directional light

		
		//model = glm::mat4(1.0f);
		//model = glm::translate(model, lightPos);
		//model = glm::scale(model, glm::vec3(0.2f));

		//light_cube.use();

		//light_cube.setMat4("model", model);
		//light_cube.setMat4("view", view);
		//light_cube.setMat4("proj", proj);
		////glBindVertexArray(lightVAO);
		//for (unsigned int i = 0; i < 4; i++)
		//{
		//	model = glm::mat4(1.0f);
		//	model = glm::translate(model, pointLightPositions[i]);
		//	model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		//	light_cube.setMat4("model", model);
		//	test_cube.Draw(light_cube);
		//	
		//}

		
		// Create a window called "My First Tool", with a menu bar.
		ImGui::Begin("My First Tool", &closeWindow, ImGuiWindowFlags_MenuBar);
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
				if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */ }
				//if (ImGui::MenuItem("Close", "Ctrl+W")) { my_tool_active = false; }
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		
		// Edit a color (stored as ~4 floats)
		ImGui::ColorEdit4("Color", my_color);

		// Plot some values
		const float my_values[] = { 0.2f, 0.1f, 1.0f, 0.5f, 0.9f, 2.2f };
		ImGui::PlotLines("Frame Times", my_values, IM_ARRAYSIZE(my_values));

		// Display contents in a scrolling region
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "Important Stuff");
		ImGui::BeginChild("Scrolling");
		for (int n = 0; n < 50; n++)
			ImGui::Text("%04d: Some text", n);
		ImGui::EndChild();
		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		glfwPollEvents();
	
	}
	
	glfwTerminate();
	return EXIT_SUCCESS;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	double currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	double cameraSpeed = 2.5f * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		mixValue += 0.001f;
		if (mixValue > 1.0f)
			mixValue = 1.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		mixValue -= 0.001f;
		if (mixValue < 0.0f)
			mixValue = 0.0f;
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cam.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cam.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cam.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cam.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
	
	if (glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT) == GLFW_PRESS) {

		int imode = glfwGetInputMode(window, GLFW_CURSOR);

		if (imode == GLFW_CURSOR_NORMAL)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		} 
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}


}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse) // initially set to true
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	
	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
	lastX = xpos;
	lastY = ypos;
	cam.ProcessMouseMovement(xoffset, yoffset);
	
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	cam.ProcessMouseScroll(yoffset);
}

unsigned int load_texture(const char* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	
	
	// load and generate the texture
	int width, height, nrChannels;
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
	if (data)
	{

		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// set the texture wrapping/filtering options (on the currently bound texture object)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}
	

	return textureID;
}