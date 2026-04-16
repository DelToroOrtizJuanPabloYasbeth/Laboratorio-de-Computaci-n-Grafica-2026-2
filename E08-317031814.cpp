/*
Práctica 8: Iluminación 2 
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;

//--- OCTAEDRO TEXTURIZADO ---
Texture octaedroTexture; 
// --- AUTO TEXTURIZADO ---
Model Cuerpo_M;
Model Cofre_M;
Model LlantaD1_M;
Model LlantaD2_M;
Model LlantaT1_M;
Model LlantaT2_M;
//--- FAROL TEXTURIZADO ---
Model Farola_M;

Model Kitt_M;
Model Llanta_M;
Model Blackhawk_M;


Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//función de calculo de normales por promedio de vértices 
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4, 5, 6,
	   4, 6, 7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};
	
	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


void CrearDado()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,

		// back
		8, 9, 10,
		10, 11, 8,

		// left
		12, 13, 14,
		14, 15, 12,
		// bottom
		16, 17, 18,
		18, 19, 16,
		// top
		20, 21, 22,
		22, 23, 20,

		// right
		4, 5, 6,
		6, 7, 4,

	};	GLfloat cubo_vertices[] = {
		// front
		//x		y		z		S		T			NX		NY		NZ
		-0.5f, -0.5f,  0.5f,	0.26f,  0.34f,		0.0f,	0.0f,	-1.0f,	//0
		0.5f, -0.5f,  0.5f,		0.49f,	0.34f,		0.0f,	0.0f,	-1.0f,	//1
		0.5f,  0.5f,  0.5f,		0.49f,	0.66f,		0.0f,	0.0f,	-1.0f,	//2
		-0.5f,  0.5f,  0.5f,	0.26f,	0.66f,		0.0f,	0.0f,	-1.0f,	//3
		// right
		//x		y		z		S		T
		0.5f, -0.5f,  0.5f,	    0.0f,  0.0f,		-1.0f,	0.0f,	0.0f,
		0.5f, -0.5f,  -0.5f,	1.0f,	0.0f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  -0.5f,	1.0f,	1.0f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  0.5f,	    0.0f,	1.0f,		-1.0f,	0.0f,	0.0f,
		// back
		-0.5f, -0.5f, -0.5f,	0.0f,  0.0f,		0.0f,	0.0f,	1.0f,
		0.5f, -0.5f, -0.5f,		1.0f,	0.0f,		0.0f,	0.0f,	1.0f,
		0.5f,  0.5f, -0.5f,		1.0f,	1.0f,		0.0f,	0.0f,	1.0f,
		-0.5f,  0.5f, -0.5f,	0.0f,	1.0f,		0.0f,	0.0f,	1.0f,

		// left
		//x		y		z		S		T
		-0.5f, -0.5f,  -0.5f,	0.0f,  0.0f,		1.0f,	0.0f,	0.0f,
		-0.5f, -0.5f,  0.5f,	1.0f,	0.0f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  0.5f,	1.0f,	1.0f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  -0.5f,	0.0f,	1.0f,		1.0f,	0.0f,	0.0f,

		// bottom
		//x		y		z		S		T
		-0.5f, -0.5f,  0.5f,	0.0f,  0.0f,		0.0f,	1.0f,	0.0f,
		0.5f,  -0.5f,  0.5f,	1.0f,	0.0f,		0.0f,	1.0f,	0.0f,
		 0.5f,  -0.5f,  -0.5f,	1.0f,	1.0f,		0.0f,	1.0f,	0.0f,
		-0.5f, -0.5f,  -0.5f,	0.0f,	1.0f,		0.0f,	1.0f,	0.0f,

		//UP
		 //x		y		z		S		T
		 -0.5f, 0.5f,  0.5f,	0.0f,  0.0f,		0.0f,	-1.0f,	0.0f,
		 0.5f,  0.5f,  0.5f,	1.0f,	0.0f,		0.0f,	-1.0f,	0.0f,
		  0.5f, 0.5f,  -0.5f,	1.0f,	1.0f,		0.0f,	-1.0f,	0.0f,
		 -0.5f, 0.5f,  -0.5f,	0.0f,	1.0f,		0.0f,	-1.0f,	0.0f,

	};

	Mesh* dado = new Mesh();
	dado->CreateMesh(cubo_vertices, cubo_indices, 192, 36);
	meshList.push_back(dado);

}

void CrearOctaedro()
{
	unsigned int octa_indices[] = {
		0, 1, 2,    // Cara Superior Frontal Derecha
		3, 4, 5,    // Cara Superior Trasera Derecha
		6, 7, 8,    // Cara Superior Trasera Izquierda
		9, 10, 11,  // Cara Superior Frontal Izquierda
		12, 13, 14, // Cara Inferior Frontal Derecha
		15, 16, 17, // Cara Inferior Trasera Derecha
		18, 19, 20, // Cara Inferior Trasera Izquierda
		21, 22, 23  // Cara Inferior Frontal Izquierda
	};

	GLfloat octa_vertices[] = {
		// --- PIRÁMIDE SUPERIOR ---
		// Cara 1: Superior Frontal Derecha
		// x      y      z          S        T         NX      NY      NZ
		 0.0f,  1.0f,  0.0f,      0.34f,  0.20f,      -1.0f,  -1.0f,  0.0f,
		 0.5f,  0.0f,  0.5f,      0.01f,  0.40f,      -1.0f,  -1.0f,  0.0f,
		 0.5f,  0.0f, -0.5f,      0.01f,  0.01f,      -1.0f,  -1.0f,  0.0f,

		 // Cara 2: Superior Trasera Derecha
		 0.0f,  1.0f,  0.0f,      0.34f,  0.20f,      0.0f,  -1.0f,  1.0f,
		 0.5f,  0.0f, -0.5f,      0.34f,  0.60f,      0.0f,  -1.0f,  1.0f,
		-0.5f,  0.0f, -0.5f,      0.01f,  0.40f,      0.0f,  -1.0f,  1.0f,

		// Cara 3: Superior Trasera Izquierda
		 0.0f,  1.0f,  0.0f,      0.34f,  0.20f,       1.0f,  -1.0f,  0.0f,
		-0.5f,  0.0f, -0.5f,      0.67f,  0.40f,       1.0f,  -1.0f,  0.0f,
		-0.5f,  0.0f,  0.5f,      0.34f,  0.59f,       1.0f,  -1.0f,  0.0f,

		// Cara 4: Superior Frontal Izquierda
		 0.0f,  1.0f,  0.0f,      0.34f,  0.20f,       0.0f,  -1.0f,  -1.0f,
		-0.5f,  0.0f,  0.5f,      0.67f,  0.00f,       0.0f,  -1.0f,  -1.0f,
		 0.5f,  0.0f,  0.5f,      0.67f,  0.39f,       0.0f,  -1.0f,  -1.0f,

		 // --- PIRÁMIDE INFERIOR ---
		 // Cara 5: Inferior Frontal Derecha
		 0.0f, -1.0f,  0.0f,      0.66f,  0.79f,      -1.0f,  1.0f,  0.0f,
		 0.5f,  0.0f,  0.5f,      0.34f,  0.60f,      -1.0f,  1.0f,  0.0f,
		 0.5f,  0.0f, -0.5f,      0.34f,  1.00f,      -1.0f,  1.0f,  0.0f,

		 // Cara 6: Inferior Trasera Derecha
		 0.0f, -1.0f,  0.0f,      0.67f,  0.80f,      0.0f,  1.0f,  1.0f,
		 0.5f,  0.0f, -0.5f,      0.67f,  0.40f,      0.0f,  1.0f,  1.0f,
		-0.5f,  0.0f, -0.5f,      0.34f,  0.60f,      0.0f,  1.0f,  1.0f,

		// Cara 7: Inferior Trasera Izquierda
		 0.0f, -1.0f,  0.0f,      0.67f,  0.79f,       1.0f,  1.0f,  0.0f,
		-0.5f,  0.0f, -0.5f,      1.00f,  0.60f,       1.0f,  1.0f,  0.0f,
		-0.5f,  0.0f,  0.5f,      0.67f,  0.40f,       1.0f,  1.0f,  0.0f,

		// Cara 8: Inferior Frontal Izquierda
		 0.0f, -1.0f,  0.0f,      0.67f,  0.79f,       0.0f,  1.0f,  -1.0f,
		-0.5f,  0.0f,  0.5f,      1.00f,  1.00f,       0.0f,  1.0f,  -1.0f,
		 0.5f,  0.0f,  0.5f,      1.00f,  0.60f,       0.0f,  1.0f,  -1.0f,
	};

	Mesh* octa = new Mesh();
	octa->CreateMesh(octa_vertices, octa_indices, 192, 24);
	meshList.push_back(octa);
}
int main()
{
	mainWindow = Window(1366, 768);
	mainWindow.Initialise();
	CreateObjects();
	CrearDado();
	CrearOctaedro(); // Crea el octaedro
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();
	octaedroTexture = Texture("Textures/octaedro_imagen.tga");
	octaedroTexture.LoadTextureA();

	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta_optimizada.obj");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");

	// --- CARGA COCHE HELLDIVERS ---
	Cuerpo_M.LoadModel("Models/cuerpo_auto_texturizado.obj");
	Cofre_M.LoadModel("Models/cofre_auto_fi_logo.obj");
	LlantaD1_M.LoadModel("Models/llantad1_texturizada.obj");
	LlantaD2_M.LoadModel("Models/llantad2_texturizada.obj");
	LlantaT1_M.LoadModel("Models/llantat1_texturizada.obj");
	LlantaT2_M.LoadModel("Models/llantat2_texturizada.obj");

	// -- CARGA FAROL TEXTURIZADO ---
	Farola_M = Model();
	Farola_M.LoadModel("Models/LAP.obj");

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	// --- LUZ DEL SOL (Direccional) ---
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f, 0.4f, 0.4f, 0.0f, -1.0f, 0.0f);

	// --- CAMBIO PARA APAGADO ---
	unsigned int pointLightCountPrendido = 1;
	unsigned int pointLightCountApagado = 0;

	//-- LUZ DE LA FAROLA (En el índice 0) --
	pointLights[0] = PointLight(1.0f, 1.0f, 1.0f, 0.5f, 1.0f, 0.0f, 1.5f, 5.0f, 0.3f, 0.2f, 0.1f);

	unsigned int spotLightCount = 0;
	// --- LUZ DE LA CÁMARA (linterna) ---
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 5.0f);
	spotLightCount++;

	// --- LUZ DEL AUTO (Inicia en Rojo, Índice 1) ---
	spotLights[1] = SpotLight(1.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.25f, 0.0f, 0.025f, 25.0f);
	spotLightCount++;

	// --- ARREGLO DE COLORES PARA EL CICLO (Para evitar recrear datos fijos dentro del switch) ---
	glm::vec3 coloresCoche[] = {
		glm::vec3(1.0f, 0.0f, 0.0f), // Rojo
		glm::vec3(1.0f, 1.0f, 0.0f), // Amarillo
		glm::vec3(0.0f, 1.0f, 0.0f), // Verde
		glm::vec3(0.0f, 1.0f, 1.0f), // Cian
		glm::vec3(0.0f, 0.0f, 1.0f), // Azul
		glm::vec3(1.0f, 0.0f, 1.0f)  // Magenta
	};

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	// Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		// Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// Luz ligada a la cámara de tipo flash
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		shaderList[0].SetDirectionalLight(&mainLight);

		// --- LÓGICA DE ENCENDIDO/APAGADO DEL FARO ---
		if (mainWindow.getLuzPrendida())
			shaderList[0].SetPointLights(pointLights, pointLightCountPrendido);
		else
			shaderList[0].SetPointLights(pointLights, pointLightCountApagado);

		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color(1.0f, 1.0f, 1.0f);

		// --- PISO ---
		for (float offsetZ = 0.0f; offsetZ >= -0.1f; offsetZ -= 0.1f) {
			model = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, offsetZ - 0.9f, 0.0f));
			model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			pisoTexture.UseTexture();
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[2]->RenderMesh();
		}

		// --- OCTAEDRO ---
		model = glm::translate(glm::mat4(1.0), glm::vec3(2.0f, 1.5f, 2.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, mainWindow.getrotacion1() * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, mainWindow.getrotacion2() * toRadians, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::rotate(model, mainWindow.getrotacion3() * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		octaedroTexture.UseTexture();
		meshList[5]->RenderMesh();
		glDisable(GL_BLEND);

		// --- INSTANCIA DEL COCHE ---
		model = glm::translate(glm::mat4(1.0), glm::vec3(mainWindow.getmuevex(), 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
		Cuerpo_M.RenderModel();

		// --- CORRECCIÓN DE COLORES Y POSICIÓN DEL FARO ---
		// Extraemos el color del arreglo usando el getter de la ventana
		glm::vec3 colorCicloActual = coloresCoche[mainWindow.getColorCiclo() % 6];

		// Calculamos la posición jerárquica
		glm::vec4 offsetLocal = glm::vec4(0.0f, 0.5f, 1.0f, 1.0f);
		glm::vec3 posFinalFaro = glm::vec3(modelaux * offsetLocal);
		glm::vec3 dirFaro = glm::vec3(1.0f, 0.0f, 0.0f);

		// Reconstruimos la luz con el nuevo color y luego actualizamos posición
		spotLights[1] = SpotLight(colorCicloActual.x, colorCicloActual.y, colorCicloActual.z,
			0.5f, 0.5f,
			posFinalFaro.x, posFinalFaro.y, posFinalFaro.z,
			dirFaro.x, dirFaro.y, dirFaro.z,
			0.25f, 0.0f, 0.025f,
			25.0f);

		// --- PIEZAS DEL AUTO ---
		model = glm::translate(modelaux, glm::vec3(0.0f, 0.72f, 1.1f));
		model = glm::rotate(model, mainWindow.getcofre() * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cofre_M.RenderModel();

		float wheelOffsets[4][3] = { {-0.5f, 0.3f, 1.4f}, {0.5f, 0.3f, 1.4f}, {-0.5f, 0.3f, -1.5f}, {0.5f, 0.3f, -1.5f} };
		for (int i = 0; i < 4; i++) {
			model = glm::translate(modelaux, glm::vec3(wheelOffsets[i][0], wheelOffsets[i][1], wheelOffsets[i][2]));
			model = glm::rotate(model, mainWindow.getrotallanta() * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			if (i == 0) LlantaD1_M.RenderModel();
			else if (i == 1) LlantaD2_M.RenderModel();
			else if (i == 2) LlantaT1_M.RenderModel();
			else LlantaT2_M.RenderModel();
		}

		// --- FAROLA ---
		model = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 1.5f, 5.0f));
		pointLights[0] = PointLight(1.0f, 1.0f, 1.0f, 0.5f, 1.0f, model[3].x, model[3].y, model[3].z, 0.3f, 0.2f, 0.1f);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Farola_M.RenderModel();

		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}


/*
		//HELICOPTERO
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 5.0f, 6.0));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Blackhawk_M.RenderModel();


		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-5.0f, 5.0f, 6.0));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		plainTexture.UseTexture();
		meshList[4]->RenderMesh();

		//Agave ¿qué sucede si lo renderizan antes del coche y el helicóptero?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -4.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		glDisable(GL_BLEND);
*/