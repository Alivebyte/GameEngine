#include "glad/glad.h"

#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"
#include "imgui.h"
#include "GLFW/glfw3.h"

#include "main.h"
#include "shader.h"
#include "camera.h"
#include <iostream>
#include "stb_image.h"

#define GLM_FORCE_CXX17

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

	int nrAttrib;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttrib);
	std::cout << "Max number of vertex attributes: " << nrAttrib << std::endl;

	Shader basic("basic.vs", "basic.fs");
	Shader lighting("lighting.vs", "lighting.fs");

	float vertices[] = {
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
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	unsigned int VBO, VAO;//, EBO;
	glGenVertexArrays(1, &VAO);
	//glGenBuffers(1, &EBO);
	glGenBuffers(1, &VBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	unsigned int diffuseMap = load_texture("container2.png");
	unsigned int specularMap = load_texture("container2_specular.png");
	//unsigned int emissionMap = load_texture("emis.png");

	basic.use();
	basic.setInt("material.diffuse", 0);
	basic.setInt("material.specular", 1);
	//basic.setInt("material.emission", 2);
	

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
		glEnable(GL_DEPTH_TEST);
		glClearColor(my_color[0], my_color[1], my_color[2], my_color[3]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		basic.use();

		basic.setVec3("viewPos", cam.Position);
		basic.setFloat("material.shininess", 32.0f);

		basic.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		basic.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		basic.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		basic.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
		// point light 1
		basic.setVec3("pointLights[0].position", pointLightPositions[0]);
		basic.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
		basic.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
		basic.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		basic.setFloat("pointLights[0].constant", 1.0f);
		basic.setFloat("pointLights[0].linear", 0.09);
		basic.setFloat("pointLights[0].quadratic", 0.032);
		// point light 2
		basic.setVec3("pointLights[1].position", pointLightPositions[1]);
		basic.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
		basic.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
		basic.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		basic.setFloat("pointLights[1].constant", 1.0f);
		basic.setFloat("pointLights[1].linear", 0.09);
		basic.setFloat("pointLights[1].quadratic", 0.032);
		// point light 3
		basic.setVec3("pointLights[2].position", pointLightPositions[2]);
		basic.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
		basic.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
		basic.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
		basic.setFloat("pointLights[2].constant", 1.0f);
		basic.setFloat("pointLights[2].linear", 0.09);
		basic.setFloat("pointLights[2].quadratic", 0.032);
		// point light 4
		basic.setVec3("pointLights[3].position", pointLightPositions[3]);
		basic.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
		basic.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
		basic.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
		basic.setFloat("pointLights[3].constant", 1.0f);
		basic.setFloat("pointLights[3].linear", 0.09);
		basic.setFloat("pointLights[3].quadratic", 0.032);
		// spotLight
		basic.setVec3("spotLight.position", cam.Position);
		basic.setVec3("spotLight.direction", cam.Front);
		basic.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		basic.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		basic.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		basic.setFloat("spotLight.constant", 1.0f);
		basic.setFloat("spotLight.linear", 0.09);
		basic.setFloat("spotLight.quadratic", 0.032);
		basic.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		basic.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
		

		//basic.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
		glm::mat4 view;
		view = cam.GetViewMatrix();
		basic.setMat4("view", view);
		glm::mat4 proj = glm::perspective(glm::radians(cam.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);
		basic.setMat4("proj", proj);
		

		//glm::vec3 lightPos(1.2f, 1.0f, 2.1f);

		glm::mat4 model = glm::mat4(1.0f);
		basic.setMat4("model", model);
		

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

		/*glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, emissionMap);*/

		glBindVertexArray(VAO);


		 // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
		for (unsigned int i = 0; i < 10; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			basic.setMat4("model", model);

			
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// We don't need light cube for directional light

		
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));

		lighting.use();

		lighting.setMat4("model", model);
		lighting.setMat4("view", view);
		lighting.setMat4("proj", proj);
		glBindVertexArray(lightVAO);
		for (unsigned int i = 0; i < 4; i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
			lighting.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		
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