#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "zidingyi.h"
#include "shader_h.h"
#include "camera.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

// create transformations
glm::mat4 transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
glm::mat4 scale = glm::mat3(1.3f);

// render mode
int GLMODE = GL_TRIANGLES;
int material_o = 1;
int texture_o = 1;
float light_n = 0.0f;

int main()
{

	printf("操作方式\n");
	printf("前后左右：A S W D              \n");
	printf("鼠标滚轮：拉近拉远              \n");
	printf("渲染模式：P 点；  L 线； T 填充  \n");
	printf("材质开关：1 开；  2 关          \n");
	printf("纹理开关：3 开；  4 关          \n");
	printf("光的强弱：5 增；  6 减          \n");
	printf("定点旋转：X 绕X轴； Y 绕Y轴； Z 绕Z轴\n");
	printf("\n\n\n请选择想要读取的模型, 输入对应的数字:\n");
	printf("1: 玫瑰(rose.txt)\n");
	printf("2: 仙女(luweiqi.txt)\n");
	printf("3: 恐怖分子(omon.txt)\n");
	// load the model_data;
	int model = 0;
	Model_data* model_data = NULL;
	while (model != 1 && model != 2 && model != 3)
	{
		scanf("%d", &model);
		if (model == 1)
		{
			model_data = ReadTxt("model_data\\rose\\rose.txt");
		}
		else if (model == 2) {
			model_data = ReadTxt("model_data\\luweiqi\\luweiqi.txt");
		}
		else if (model == 3) {
			model_data = ReadTxt("model_data\\terror\\omon.txt");
		}
		else {
			printf("目前只有三个模型哦，更多模型，敬请期待......\n请重新选择:\n");
		}
	}
	printf("读取完成  have a good time!!!\n");
	
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	//glfwSet

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	// build and compile our shader program
	// ------------------------------------

	Shader lightingShader("shader\\model_1.vs", "shader\\model_1.fs"); // you can name your shader files however you like
	Shader lightPointShader("shader\\lightpoint.vs", "shader\\lightpoint.fs");
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	

	int n;
	n = model_data[0].model_num;
	
	unsigned int VBO[76], VAO[76], diffuseMap[76];
	for (int i = 0; i < n; i++)
	{
		glGenVertexArrays(1, &VAO[i]);
		glGenBuffers(1, &VBO[i]);
		// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).

		glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
		glBufferData(GL_ARRAY_BUFFER, model_data[i].point_num * 24 * sizeof(float), model_data[i].model, GL_STATIC_DRAW);
		glBindVertexArray(VAO[i]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
		glEnableVertexAttribArray(1);

		// load textures (we now use a utility function to keep the code more organized)
	// -----------------------------------------------------------------------------
		//diffuseMap[i] = loadTexture(model_data[i].texture_name);
		if (model_data[i].texture_name[0] != NULL)
		{
			diffuseMap[i] = loadTexture(model_data[i].texture_name);
		}
	}
	// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	unsigned int lightCubeVAO;
	glGenVertexArrays(1, &lightCubeVAO);
	glBindVertexArray(lightCubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBindVertexArray(lightCubeVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// shader configuration
	// --------------------
	lightingShader.use();

	if (model == 3)
	{
		transform = glm::scale(transform, glm::vec3(1, 1, 1));
	}
	else if (model == 1) {
		transform = glm::scale(transform, glm::vec3(5.0, 5.0, 5.0));
	}
	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

		// be sure to activate shader when setting uniforms/drawing objects
		lightingShader.use();
		// 

		// get matrix's uniform location and set matrix
		unsigned int transformLoc = glGetUniformLocation(lightingShader.ID, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

		//lightingShader.setVec3("light.direction", -0.2f, -1.0f, -0.3f);
		lightingShader.setVec3("light.direction", -1.0f, -1.0f, 1.0f);
		lightingShader.setVec3("viewPos", camera.Position);

		// light properties
		//lightingShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
		//lightingShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
		//lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

		lightingShader.setVec3("light.ambient", 0.2f + light_n, 0.2f + light_n, 0.2f + light_n);
		lightingShader.setVec3("light.diffuse", 0.2f + light_n, 0.2f + light_n, 0.2f + light_n);
		lightingShader.setVec3("light.specular", 0.2f+light_n, 0.2f + light_n, 0.2f + light_n);



		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		lightingShader.setMat4("projection", projection);
		glm::mat4 view = camera.GetViewMatrix();
		lightingShader.setMat4("view", view);

		// world transformation
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		lightingShader.setMat4("model", model);
		
		for (int i = 0; i < n; i++)
		//int i = 1;
		{		
			//lightingShader.setInt("material.diffuse", 0);
			
			// bind diffuse map
			if (model_data[i].texture_name[0] != NULL && material_o == 1)
			{
				// material properties
				lightingShader.use();
				lightingShader.setVec3("material.specular", model_data[i].specular[0], model_data[i].specular[1], model_data[i].specular[2]);
				lightingShader.setFloat("material.shininess", model_data[i].shininess);			
			}
			else {
				// material properties
				lightingShader.use();
				lightingShader.setVec3("material.specular",  0.0f, 0.0f, 0.0f);
				lightingShader.setFloat("material.shininess", 0);			
			}

			if (model_data[i].texture_name[0] != NULL && texture_o == 1)
			{
				lightingShader.setInt("material.diffuse", 0);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, diffuseMap[i]);
			}
			else {
				lightingShader.setInt("material.diffuse", 1);
				glActiveTexture(GL_TEXTURE1);
			}
			// render the triangle_1
			glBindVertexArray(VAO[i]);
			// calculate the model matrix for each object and pass it to shader before drawing
			glDrawArrays(GLMODE, 0, model_data[i].point_num*18);
			//glDrawArrays(GL_LINES, i, model_data[i].point_num * 9);
			//glDrawElements(GL_TRIANGLES, TRIANGLE_NA, GL_UNSIGNED_INT, 0);
		}
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// optional: de-allocate all resources once they've outlived their purpose:
// ------------------------------------------------------------------------
	glDeleteVertexArrays(4, VAO);
	glDeleteBuffers(4, VBO);
	//glDeleteVertexArrays(1, &VAO_2);
	//glDeleteBuffers(1, &VBO_2);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
	{
		GLMODE = GL_TRIANGLES;
	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
	{
		GLMODE = GL_LINES;
	}
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
	{
		GLMODE = GL_POINTS;
	}
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		material_o = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		material_o = 0;
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
	{
		texture_o = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
	{
		texture_o = 0;
	}
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
	{
		light_n += 0.001;
	}
	if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
	{
		light_n -= 0.001;
	}
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
	{
		//transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.000000001f, 0.0f, 0.0f));
		transform = glm::rotate(transform, glm::radians(0.5f), glm::vec3(1.0f, 0.0f, 0.0f));
	}
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
	{
		transform = glm::rotate(transform, glm::radians(0.5f), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
	{
		transform = glm::rotate(transform, glm::radians(0.5f), glm::vec3(0.0f, 0.0f, 1.0f));
	}

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
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
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}