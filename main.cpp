#include <glad/glad.h>
#include <iostream>
#include <vector>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "shader.h"
#include "camera.h"
#include "initwindow.h"
#include "texture.h"
#include "mesh.h"
#include "model.h"
#include "myrand.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//???????
const float screenWidth = 1600;
const float screenHeight = 1200;

// ????????????????
float deltaTime = 0.0f;
// ?????????
float lastFrame = 0.0f;
//????????????
float enterTime;

//?????????
Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
float lastX = screenWidth / 2.0f;
float lastY = screenHeight / 2.0f;
bool firstMouse = true;

//??????
glm::vec3 carpos(0.0f, -0.25f, 0.0f);
glm::vec3 carforward(0.0f, 0.0f, 1.0f);
float angletorotate = 0;
double carvelocity = 0;

//?????
int screenmode = 0;

double getRotateAngle(double x1, double y1, double x2, double y2)
{
	const double epsilon = 1.0e-6;
	const double nyPI = acos(-1.0);
	double dist, dot, degree, angle;

	// normalize
	dist = sqrt(x1 * x1 + y1 * y1);
	x1 /= dist;
	y1 /= dist;
	dist = sqrt(x2 * x2 + y2 * y2);
	x2 /= dist;
	y2 /= dist;
	// dot product
	dot = x1 * x2 + y1 * y2;
	if (fabs(dot - 1.0) <= epsilon)
		angle = 0.0;
	else if (fabs(dot + 1.0) <= epsilon)
		angle = nyPI;
	else {
		double cross;

		angle = acos(dot);
		//cross product
		cross = x1 * y2 - x2 * y1;
		// vector p2 is clockwise from vector p1
		// with respect to the origin (0.0)
		if (cross < 0) {
			angle = 2 * nyPI - angle;
		}
	}
	degree = angle * 180.0 / nyPI;
	return degree;
}

void processInput(GLFWwindow* window)
{
	//?��?????��???????????��????glfwGetKey??????GLFW_RELEASE
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (camera.mode2 == 39)
		return;

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		carvelocity = SPEED * deltaTime;
		carpos.x += camera.Front.x * carvelocity;
		carpos.z += camera.Front.z * carvelocity;
		glm::vec3 tmppos = carpos;
		tmppos += camera.Front + camera.Front;
		tmppos.y += 1.2f;
		if (camera.mode == 1)
			camera.Position = tmppos;
		camera.mode = 1;
		//glm::vec3 axis = glm::normalize(glm::cross(carforward, camera.Front));
		//angletorotate = glm::orientedAngle(camera.Front, carforward, axis);
		
		angletorotate = glm::radians(getRotateAngle(carforward.z, carforward.x, camera.Front.z, camera.Front.x));

		camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.mode = 0;
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{ 
		camera.mode = 0;
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.mode = 0;
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{ 
		camera.mode = 0;
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}


//????
const int particlenum = 60;
struct Particle
{
	glm::vec3 startposition;
	glm::vec3 curposition;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	glm::vec3 color;
	glm::vec3 size;
	float alpha;
	float age;
	float life;
} p[particlenum];


unsigned int loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
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

//?????
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
glm::vec3 lightPos(-10000.0f, 10000.0f, -15000.0f);

//???????
glm::vec3 rocketpos(0.0f, -0.25f, 0.0f);
const float rocketspeed = 0.000001f;
const float rocketacceleration = 1.0f;
float rockettimepass = 0.0f;
glm::vec3 origrocketpos;


//?????????
Rand myrandengine;

int main()
{
	GLFWwindow* window = Window().returnwindow();

	//????-----------------------------------------------------------------------------------------------
	Shader skyboxShader("vert\\skyboxshader.vert", "frag\\skyboxshader.frag");
	std::vector<std::string> faces
	{
			"material\\skybox\\right.jpg",
			"material\\skybox\\left.jpg",
			"material\\skybox\\top.jpg",
			"material\\skybox\\bottom.jpg",
			"material\\skybox\\front.jpg",
			"material\\skybox\\back.jpg"
		};
	unsigned int cubemapTexture = loadCubemap(faces);

	float skyboxVertices[] = {
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

	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);
	//????-----------------------------------------------------------------------------------------------
	
	//????-------------------------------------------------------------------------------------------------
	Shader partShader("vert\\partshader.vert", "frag\\partshader.frag");
	const int Y_SEGMENTS = 8;
	const int X_SEGMENTS = 8;
	const GLfloat PI = 3.14159265358979323846f;
	std::vector<float> sphereVertices;
	std::vector<int> sphereIndices;

	for (int y = 0; y <= Y_SEGMENTS; y++)
	{
		for (int x = 0; x <= X_SEGMENTS; x++)
		{
			float xSegment = (float)x / (float)X_SEGMENTS;
			float ySegment = (float)y / (float)Y_SEGMENTS;
			float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
			float yPos = std::cos(ySegment * PI);
			float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
			sphereVertices.push_back(xPos);
			sphereVertices.push_back(yPos);
			sphereVertices.push_back(zPos);
		}
	}

	//???????Indices
	for (int i = 0; i < Y_SEGMENTS; i++)
	{
		for (int j = 0; j < X_SEGMENTS; j++)
		{
			sphereIndices.push_back(i * (X_SEGMENTS + 1) + j);
			sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j);
			sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j + 1);
			sphereIndices.push_back(i * (X_SEGMENTS + 1) + j);
			sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j + 1);
			sphereIndices.push_back(i * (X_SEGMENTS + 1) + j + 1);
		}
	}

	unsigned int partVBO, partVAO;
	glGenVertexArrays(1, &partVAO);
	glGenBuffers(1, &partVBO);
	//????????????VAO??VBO
	glBindVertexArray(partVAO);
	glBindBuffer(GL_ARRAY_BUFFER, partVBO);
	//?????????????????????????
	glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), &sphereVertices[0], GL_STATIC_DRAW);

	GLuint partEBO;
	glGenBuffers(1, &partEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, partEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(int), &sphereIndices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	for (int i = 0; i < particlenum; i++)
	{
		//??????
		p[i].velocity = { myrandengine.myRandGetReal<float>(-5.0, 5.0) ,
						  myrandengine.myRandGetReal<float>(-5.0, -4.0) ,
						  myrandengine.myRandGetReal<float>(-5.0, 5.0) };
		//???
		glm::vec3 fire[] = { {0.1,0,0}, {0.5,0,0}, {1.0,0,0}, {1.0,1.0,0}, {1.0,1.0,1.0} };
		//???
		p[i].color = fire[myrandengine.myRandGetInteger<int>(0, 4)];

		//?????
		p[i].alpha = myrandengine.myRandGetReal<float>(0.9, 1.0);

		//?????
		p[i].acceleration = { myrandengine.myRandGetReal<float>(-5.0, 5.0) ,
						  myrandengine.myRandGetReal<float>(-5.0, -3.0) ,
						  myrandengine.myRandGetReal<float>(-5.0, 5.0) };
		//????????
		p[i].life = myrandengine.myRandGetReal<float>(0.6, 0.9);

		//??��
		float se = myrandengine.myRandGetReal<float>(0.1, 0.2);

		p[i].size = { se, se, se };


		p[i].startposition = { myrandengine.myRandGetReal <float>(0,0.5),
							   myrandengine.myRandGetReal <float>(-0.5,0) ,
							   myrandengine.myRandGetReal<float>(0,0.5) };
		p[i].curposition = p[i].startposition;
		p[i].age = 0;
	}
	
	//????-------------------------------------------------------------------------------------------------



	//??-------------------------------------------------------------------------------------------------
	Shader planShader("vert\\planshader.vert", "frag\\planshader.frag");
	float planeVertices[] = {
		// positions            // normals         // texcoords
		 1001.0f, -0.5f,  1001.0f,  0.0f, 1.0f, 0.0f,  1001.0f,  0.0f,
		-1001.0f, -0.5f,  1001.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
		-1001.0f, -0.5f, -1001.0f,  0.0f, 1.0f, 0.0f,   0.0f, 1001.0f,

		 1001.0f, -0.5f,  1001.0f,  0.0f, 1.0f, 0.0f,  1001.0f,  0.0f,
		-1001.0f, -0.5f, -1001.0f,  0.0f, 1.0f, 0.0f,   0.0f, 1001.0f,
		 1001.0f, -0.5f, -1001.0f,  0.0f, 1.0f, 0.0f,  1001.0f, 1001.0f
	};

	unsigned int planeVAO, planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	unsigned int floorTexture = Texture::loadTexture("material\\others\\grassland.jpg");

	planShader.use();
	planShader.setInt("floorTexture", 0);
	//??-------------------------------------------------------------------------------------------------

	//???-------------------------------------------------------------------------------------------------
	Shader grassShader("vert\\grassshader.vert", "frag\\grassshader.frag");

	float cubeVertices[] = {
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
	float transparentVertices[] = {
		// positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
		1.0f,  0.5f,  0.0f,  1.0f,  0.0f
	};
	unsigned int transparentVAO, transparentVBO;
	glGenVertexArrays(1, &transparentVAO);
	glGenBuffers(1, &transparentVBO);
	glBindVertexArray(transparentVAO);
	glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindVertexArray(0);

	unsigned int transparentTexture = Texture::loadTexture("material\\others\\grass.png");

	grassShader.use();
	grassShader.setInt("texture1", 0);

	unsigned int amount = 500000;
	glm::mat4* vegetation = new glm::mat4[amount];

	for (unsigned int i = 0; i < amount; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		float x = myrandengine.myRandGetInteger<int>(-1000, 1000);
		float y = 0.0f;
		float z = myrandengine.myRandGetInteger<int>(-1000, 1000);
		model = glm::translate(model, glm::vec3(x, y, z));

		vegetation[i] = model;
	}

	// ?????????
	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &vegetation[0], GL_STATIC_DRAW);

	glBindVertexArray(transparentVAO);
	// ????????
	GLsizei vec4Size = sizeof(glm::vec4);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(vec4Size));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);

	glBindVertexArray(0);
	//???-------------------------------------------------------------------------------------------------

	//????-------------------------------------------------------------------------------------------------
	Model carmodel("material\\car\\uploads_files_2979046_Renault12TL.obj");
	//Model carmodel("C:\\Users\\asuru\\Desktop\\uploads_files_1914765_Rocket.obj");
	Shader carShader("vert\\carshader.vert", "frag\\carshader.frag");
	//????-------------------------------------------------------------------------------------------------

	//???-------------------------------------------------------------------------------------------------
	Model rocketmodel("material\\rocket\\uploads_files_1914765_Rocket.obj");
	//Model carmodel("C:\\Users\\asuru\\Desktop\\uploads_files_1914765_Rocket.obj");
	Shader rocketShader("vert\\rocketshader.vert", "frag\\rocketshader.frag");
	
	rocketShader.setInt("skybox", 0);
	rocketShader.setInt("show",1);
	rocketShader.use();
	rocketpos.x = myrandengine.myRandGetInteger<int>(-950, 950);
	rocketpos.z = myrandengine.myRandGetInteger<int>(-950, 950);
	//???-------------------------------------------------------------------------------------------------

	//??????-------------------------------------------------------------------------------------------------
	Shader doorShader("vert\\doorshader.vert", "frag\\doorshader.frag");

	unsigned int doorTexture1 = Texture::loadTexture("material\\others\\container2.jpg");
	unsigned int doorTexture2 = Texture::loadTexture("material\\others\\container2_specular.jpg");
	unsigned int doorTexture3 = Texture::loadTexture("material\\others\\matrix.jpg");

	float doorvertices[] = {
		// positions          // normals           // texture coords
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

	const int dooramount = 10;
	//????
	std::vector<int> doorhaveenter(dooramount, 0);
	
	glm::vec3 doorPositions[dooramount] = {
		glm::vec3(rocketpos)
	};

	doorPositions[0].x += 15;
	doorPositions[0].z += 5;

	for (unsigned int i = 1; i < dooramount; i++)
	{
		float tmpx = myrandengine.myRandGetInteger<int>(-950, 950);
		float tmpz = myrandengine.myRandGetInteger<int>(-950, 950);
		while (1)
		{
			bool ok = true;
			for(unsigned int j = 1; j < i; j ++)
			{ 
				if (sqrt(fabs(tmpx - doorPositions[j].x) * fabs(tmpx - doorPositions[j].x) + fabs(tmpz - doorPositions[j].z) * fabs(tmpz - doorPositions[j].z)) <= 30.0f)
				{
					tmpx = myrandengine.myRandGetInteger<int>(-950, 950);
					tmpz = myrandengine.myRandGetInteger<int>(-950, 950);
				}
			}
			if (ok)
				break;
		}
		doorPositions[i] = { tmpx, 0.0f, tmpz };
	}

	unsigned int doorVBO, doorVAO;
	glGenVertexArrays(1, &doorVAO);
	glGenBuffers(1, &doorVBO);

	glBindBuffer(GL_ARRAY_BUFFER, doorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(doorvertices), doorvertices, GL_STATIC_DRAW);

	glBindVertexArray(doorVAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	//??????-------------------------------------------------------------------------------------------------

	//?????-------------------------------------------------------------------------------------------------
	Shader frameShader("vert\\frameshader.vert", "frag\\frameshader.frag");

	float framequadVertices[] = {
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	unsigned int quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(framequadVertices), &framequadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	frameShader.use();
	frameShader.setInt("screenTexture", 2);

	//????????????
	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	//????????
	unsigned int frametexture;
	glGenTextures(1, &frametexture);
	glBindTexture(GL_TEXTURE_2D, frametexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frametexture, 0);

	//??????????
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	//??????????????
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//?????-------------------------------------------------------------------------------------------------


	//???????
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	while (!glfwWindowShouldClose(window))
	{
		//?????-----------------------------------------------------------------------------------------------
		//????????
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//???????
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		//??????????
		glEnable(GL_DEPTH_TEST);
		//GAMMA????
		glEnable(GL_FRAMEBUFFER_SRGB);

		//???????
		processInput(window);

		//??????????????????????????????????
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		//????????????????????
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//vp?��
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)screenWidth / (float)screenHeight, 0.1f, 1500.0f);

		//?????-----------------------------------------------------------------------------------------------

		//????-----------------------------------------------------------------------------------------------
		glDepthFunc(GL_LEQUAL);
		skyboxShader.use();
		glm::mat4 skyboxview = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		skyboxShader.setMat4("view", skyboxview);
		skyboxShader.setMat4("projection", projection);
		glBindVertexArray(skyboxVAO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthFunc(GL_LESS);
		//????-----------------------------------------------------------------------------------------------

		//??-------------------------------------------------------------------------------------------------
		planShader.use();
		planShader.setMat4("projection", projection);
		planShader.setMat4("view", view);
		planShader.setVec3("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);
		planShader.setVec3("lightPos", lightPos.x, lightPos.y, lightPos.z);
		glBindVertexArray(planeVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, floorTexture);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		//??-------------------------------------------------------------------------------------------------

		//???-------------------------------------------------------------------------------------------------
		grassShader.use();
		grassShader.setMat4("projection", projection);
		grassShader.setMat4("view", view);
		glBindVertexArray(transparentVAO);
		glBindTexture(GL_TEXTURE_2D, transparentTexture);
		glBindVertexArray(transparentVAO);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, amount);
		//???-------------------------------------------------------------------------------------------------

		//????-------------------------------------------------------------------------------------------------
		//????????
		lightColor.x = 0.2226f;
		lightColor.y = 0.769f;
		lightColor.z = 0.7304f;

		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); // ???????
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // ???????


		carShader.use();
		carShader.setVec3("objectColor", 1.0f, 1.0f, 1.0f);
		carShader.setVec3("lightColor", lightColor.x, lightColor.y, lightColor.z);
		carShader.setVec3("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);

		//???????
		carShader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
		carShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
		carShader.setFloat("material.shininess", 64.0f);

		//??��?
		carShader.setVec3("dirlight.direction", 1.0f, 0.3f, 0.5f);
		carShader.setVec3("dirlight.ambient", 0.2f, 0.2f, 0.2f);
		carShader.setVec3("dirlight.diffuse", 0.5f, 0.5f, 0.5f);
		carShader.setVec3("dirlight.specular", 1.0f, 1.0f, 1.0f);

		//????
		for (unsigned int i = 0; i < 1; i++)
		{
			std::string pla = "pointlights[" + std::to_string(i) + "].position";
			carShader.setVec3(pla.c_str(), lightPos.x, lightPos.y, lightPos.z);
			pla = "pointlights[" + std::to_string(i) + "].ambient";
			carShader.setVec3(pla.c_str(), 0.2f, 0.2f, 0.2f);
			pla = "pointlights[" + std::to_string(i) + "].diffuse";
			carShader.setVec3(pla.c_str(), 0.5f, 0.5f, 0.5f);
			pla = "pointlights[" + std::to_string(i) + "].specular";
			carShader.setVec3(pla.c_str(), 1.0f, 1.0f, 1.0f);
			pla = "pointlights[" + std::to_string(i) + "].constant";
			carShader.setFloat(pla.c_str(), 1.0f);
			pla = "pointlights[" + std::to_string(i) + "].linear";
			carShader.setFloat(pla.c_str(), 0.09f);
			pla = "pointlights[" + std::to_string(i) + "].quadratic";
			carShader.setFloat(pla.c_str(), 0.032f);
		}

		carShader.setMat4("projection", projection);
		carShader.setMat4("view", view);

		//????m?��
		glm::mat4 modell = glm::mat4(1.0f);
		//modell = glm::scale(modell, glm::vec3(0.5f)); // a smaller cube
		modell = glm::translate(modell, carpos);
		modell = glm::rotate(modell, angletorotate, glm::vec3(0.0f, 1.0f, 0.0f));
		//std::cout << angletorotate << std::endl;
		carShader.setMat4("model", modell);
		carmodel.Draw(carShader);
		//????------------------------------------------------------------------------------------------------


		//???-------------------------------------------------------------------------------------------------
		// rocketShader.use();
		rocketShader.setInt("show", 1);
		rocketShader.setInt("skybox", 0);
		rocketShader.setVec3("objectColor", 1.0f, 1.0f, 1.0f);
		rocketShader.setVec3("lightColor", lightColor.x, lightColor.y, lightColor.z);
		rocketShader.setVec3("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);

		//???????
		rocketShader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
		rocketShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
		rocketShader.setFloat("material.shininess", 64.0f);

		//??��?
		rocketShader.setVec3("dirlight.direction", 1.0f, 0.3f, 0.5f);
		rocketShader.setVec3("dirlight.ambient", 0.2f, 0.2f, 0.2f);
		rocketShader.setVec3("dirlight.diffuse", 0.5f, 0.5f, 0.5f);
		rocketShader.setVec3("dirlight.specular", 1.0f, 1.0f, 1.0f);

		//????
		for (unsigned int i = 0; i < 1; i++)
		{
			std::string pla = "pointlights[" + std::to_string(i) + "].position";
			rocketShader.setVec3(pla.c_str(), lightPos.x, lightPos.y, lightPos.z);
			pla = "pointlights[" + std::to_string(i) + "].ambient";
			rocketShader.setVec3(pla.c_str(), 0.2f, 0.2f, 0.2f);
			pla = "pointlights[" + std::to_string(i) + "].diffuse";
			rocketShader.setVec3(pla.c_str(), 0.5f, 0.5f, 0.5f);
			pla = "pointlights[" + std::to_string(i) + "].specular";
			rocketShader.setVec3(pla.c_str(), 1.0f, 1.0f, 1.0f);
			pla = "pointlights[" + std::to_string(i) + "].constant";
			rocketShader.setFloat(pla.c_str(), 1.0f);
			pla = "pointlights[" + std::to_string(i) + "].linear";
			rocketShader.setFloat(pla.c_str(), 0.09f);
			pla = "pointlights[" + std::to_string(i) + "].quadratic";
			rocketShader.setFloat(pla.c_str(), 0.032f);
		}

		rocketShader.setMat4("projection", projection);
		rocketShader.setMat4("view", view);

		//????m?��
		modell = glm::mat4(1.0f);
		modell = glm::translate(modell, rocketpos);
		modell = glm::scale(modell, glm::vec3(0.03f));
		
		rocketShader.setMat4("model", modell);
		rocketmodel.Draw(rocketShader);
		//???------------------------------------------------------------------------------------------------

		//??????------------------------------------------------------------------------------------------------
		if (camera.mode2==39 || sqrt(fabs(carpos.x - rocketpos.x) * fabs(carpos.x - rocketpos.x) + fabs(carpos.z - rocketpos.z) * fabs(carpos.z - rocketpos.z)) <= 5.0f)
		{
			if (camera.mode2 != 39)
			{ 
				camera.Position = rocketpos;
				camera.Position.x -= 15.0f;
				camera.Position.z += 15.0f;
				//camera.MouseSensitivity += 0.2f;
				origrocketpos = rocketpos;
				rocketShader.use();
				rocketShader.setInt("show", 0);
			}
			camera.mode2 = 39;
			camera.mode = 1;
			
			partShader.use();
			partShader.setMat4("projection", projection);
			partShader.setMat4("view", view);

			for (int i = 0; i < particlenum; i++)
			{
				p[i].age += deltaTime;

				if (p[i].age >= p[i].life)
				{
					//??????
					p[i].velocity = { myrandengine.myRandGetReal<float>(-5.0, 5.0) ,
							  myrandengine.myRandGetReal<float>(-5.0, -4.0) ,
							  myrandengine.myRandGetReal<float>(-5.0, 5.0) };

					glm::vec3 fire[] = { {0.1,0,0}, {0.5,0,0}, {1.0,0,0}, {1.0,1.0,0}, {1.0,1.0,1.0} };
					//???
					p[i].color = fire[myrandengine.myRandGetInteger<int>(0, 4)];

					//?????
					p[i].alpha = myrandengine.myRandGetReal<float>(0.6, 1.0);

					//?????
					p[i].acceleration = { myrandengine.myRandGetReal<float>(-5.0, 5.0) ,
							  myrandengine.myRandGetReal<float>(-5.0, -3.0) ,
							  myrandengine.myRandGetReal<float>(-5.0, 5.0) };

					//????????
					p[i].life = myrandengine.myRandGetReal<float>(0.6, 0.9);

					//??��
					float se = myrandengine.myRandGetReal<float>(0.1, 0.2);

					p[i].size = { se, se, se };


					p[i].startposition = rocketpos;
					p[i].curposition = p[i].startposition;
					p[i].age = 0;
				}

				partShader.setVec3("color", p[i].color.x, p[i].color.y, p[i].color.z);
				partShader.setFloat("alpha", p[i].alpha);

				const float half = 0.5f;
				p[i].curposition = p[i].startposition + p[i].velocity * p[i].age + p[i].acceleration * p[i].age * p[i].age * half;

				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, p[i].curposition);
				model = glm::scale(model, p[i].size);
				partShader.setMat4("model", model);
				//??????
				glBindVertexArray(partVAO);
				//????????????
				//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glDrawElements(GL_TRIANGLES, X_SEGMENTS * Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);
			}
			
			rockettimepass += deltaTime;
			rocketpos.y = origrocketpos.y + rocketspeed * rockettimepass + 0.5f * rocketacceleration * rockettimepass * rockettimepass;
			camera.lasteye = rocketpos - camera.Position;
			
		}
		//??????------------------------------------------------------------------------------------------------

		//??????-------------------------------------------------------------------------------------------------
		doorShader.use();
		doorShader.setVec3("objectColor", 1.0f, 1.0f, 1.0f);
		doorShader.setVec3("lightColor", lightColor.x, lightColor.y, lightColor.z);
		doorShader.setVec3("lightPos", lightPos.x, lightPos.y, lightPos.z);
		doorShader.setVec3("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);

		//???????
		doorShader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
		doorShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
		doorShader.setFloat("material.shininess", 64.0f);
		//??????????????��??
		doorShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
		// ????????????��???????
		doorShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
		doorShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		//???????
		doorShader.setInt("material.diffuse", 0);
		doorShader.setInt("material.specular", 1);
		doorShader.setInt("matrixLight", 2);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, doorTexture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, doorTexture2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, doorTexture3);

		doorShader.setMat4("projection", projection);
		doorShader.setMat4("view", view);

		for (unsigned int i = 0; i < dooramount; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, doorPositions[i]);
			model = glm::scale(model, glm::vec3(15.0f));
			//model = glm::rotate(model, glm::radians(myrandengine.myRandGetReal<float>(0, 360.0f)), 
			//	                  glm::vec3(myrandengine.myRandGetReal<float>(0, 1.0f), myrandengine.myRandGetReal<float>(0, 1.0f), myrandengine.myRandGetReal<float>(0, 1.0f)));
			
			doorShader.setMat4("model", model);
			doorShader.setInt("haveenter",doorhaveenter[i]);
			glBindVertexArray(doorVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		//??????-------------------------------------------------------------------------------------------------

		//?????????-------------------------------------------------------------------------------------------------
		for (unsigned int i = 0; i < dooramount; i++)
		{ 
			glm::vec3 curdoorpos = doorPositions[i];
			if (sqrt(fabs(carpos.x - curdoorpos.x) * fabs(carpos.x - curdoorpos.x) + fabs(carpos.z - curdoorpos.z) * fabs(carpos.z - curdoorpos.z)) <= 15.0f)
			{
				carpos = doorPositions[i ^ 0x1];
				carpos.x += 20;
				glm::vec3 tmppos = carpos;
				tmppos += camera.Front + camera.Front;
				tmppos.y += 1.2f;
				camera.Position = tmppos;
				enterTime = glfwGetTime();
				screenmode = 1;
				doorhaveenter[i] = doorhaveenter[i^0x1] = 1;
			}
		}
		//?????????-------------------------------------------------------------------------------------------------

		//?????-------------------------------------------------------------------------------------------------
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		frameShader.use();
		frameShader.setInt("screenmode", screenmode);
		glBindVertexArray(quadVAO);
		glBindTexture(GL_TEXTURE_2D, frametexture);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		if (glfwGetTime() - enterTime > 1.0f)
			screenmode = 0;
		//?????-------------------------------------------------------------------------------------------------

		//????????---------------------------------------------------------------------------------------------
		//???????
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);

		glfwSwapBuffers(window);
		//???????��???????????????????????????????????????????
		glfwPollEvents();
		//????????---------------------------------------------------------------------------------------------
	}

	//??????????
	glDeleteFramebuffers(1, &fbo);
	glfwTerminate();
	return 0;
}