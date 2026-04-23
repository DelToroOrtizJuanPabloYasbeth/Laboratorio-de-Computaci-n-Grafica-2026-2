/*
Práctica 9 Animación:
Sesión 1: Simple o básica:Por banderas y condicionales (más de 1 transformación geométrica se ve modificada)
Sesión 2: Compleja: Por medio de funciones y algoritmos. Textura Animada
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

//variables para animación proxima clase
float movCoche;
float movOffset;
float rotllanta;
float rotllantaOffset;
bool avanza = 1;
float toffsetflechau = 0.0f;
float toffsetflechav = 0.0f;
float toffsetnumerou = 0.0f;
float toffsetnumerov = 0.0f;
float toffsetnumerocambiau = 0.0;
float angulovaria = 0.0f;


// ---  VARIABLES AUTO  ---
glm::vec3 posAuto = glm::vec3(0.0f, -1.8f, 3.5f); // Posición inicial
glm::vec3 dirAuto = glm::vec3(-1.0f, 0.0f, 0.0f); // Dirección inicial (Hacia -X)
float rotAutoY = 0.0f;
float rotAutoX = 0.0f; 
float rotAutoZ = 0.0f; 
float distanciaRecorrida = 0.0f;

// --- VARIABLES NAVE  ---
glm::vec3 posNave = glm::vec3(-182.0f, 30.0f, -22.0f); // Posición inicial
glm::vec3 dirNave = glm::vec3(1.0f, 0.0f, 0.0f); // Dirección inicial (Hacia +X)
float rotNaveY = 0.0f;
float rotNaveX = 0.0f;
float rotNaveZ = 0.0f;
float distanciaNave = 0.0f;
bool naveIniciada = false;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture FlechaTexture;




// --- AUTO PRÁCTICA 9 ---
Model Cuerpo_M;
Model Cofre_M;
Model LlantaD1_M;
Model LlantaD2_M;
Model LlantaT1_M;
Model LlantaT2_M;


Model Pista_M;

// ---  NAVE MODIFICADA ---
Model Nave_M;        
Model AlaIzq_M;      
Model AlaDer_M;
Model HeliceIzq_M;
Model HeliceDer_M;

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



//cálculo del promedio de las normales para sombreado de Phong
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
	   4,5,6,
	   4,6,7
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


	unsigned int flechaIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat flechaVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int scoreIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat scoreVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int numeroIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat numeroVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		0.25f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		0.25f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);


	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(flechaVertices, flechaIndices, 32, 6);
	meshList.push_back(obj5);

	Mesh* obj6 = new Mesh();
	obj6->CreateMesh(scoreVertices, scoreIndices, 32, 6);
	meshList.push_back(obj6); // todos los números

	Mesh* obj7 = new Mesh();
	obj7->CreateMesh(numeroVertices, numeroIndices, 32, 6);
	meshList.push_back(obj7); // solo un número

}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);

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
	FlechaTexture = Texture("Textures/flechas.tga");
	FlechaTexture.LoadTextureA();
	

	// --- CARGA COCHE HELLDIVERS --- 
	Cuerpo_M.LoadModel("Models/cuerpo_auto_texturizado.obj");
	Cofre_M.LoadModel("Models/cofre_auto_fi_logo.obj");
	LlantaD1_M.LoadModel("Models/llantad1_texturizada.obj");
	LlantaD2_M.LoadModel("Models/llantad2_texturizada.obj");
	LlantaT1_M.LoadModel("Models/llantat1_texturizada.obj");
	LlantaT2_M.LoadModel("Models/llantat2_texturizada.obj");

	Pista_M = Model();
	Pista_M.LoadModel("Models/pista.obj");

	
	// --- CARGA DE NAVE Y SUS COMPONENTES ---
	Nave_M = Model();
	Nave_M.LoadModel("Models/nave_modificada.obj"); // El cuerpo base
	AlaIzq_M = Model();
	AlaIzq_M.LoadModel("Models/nave_ala_I.obj");
	AlaDer_M = Model();
	AlaDer_M.LoadModel("Models/nave_ala_D.obj");
	HeliceIzq_M = Model();
	HeliceIzq_M.LoadModel("Models/nave_helice_I.obj");
	HeliceDer_M = Model();
	HeliceDer_M.LoadModel("Models/nave_helice_D.obj");

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


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.5f, 0.3f,
		0.0f, -1.0f, 0.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 2.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	//luz fija
	spotLights[1] = SpotLight(0.0f, 0.0f, 1.0f,
		0.0f, 0.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;

	// Luz frontal del auto (Azul) 
	spotLights[2] = SpotLight(0.0f, 0.0f, 1.0f,  // Color 
		2.0f, 2.0f,                              // Intensidad ambiente y difusa 
		0.0f, 0.0f, 0.0f,                        // Posición 
		0.0f, 0.0f, -1.0f,                       // Dirección
		0.7f, 0.025f, 0.025f,                    // Coeficientes de atenuación 
		20.0f);                                  // Ángulo de apertura (aprox 20°) 
	spotLightCount++;

	// Luz de la Nave 
	spotLights[3] = SpotLight(1.0f, 0.0f, 0.0f,  // Color 
		2.0f, 2.0f,                              // Intensidad ambiente y difusa 
		0.0f, 0.0f, 0.0f,                        // Posición 
		0.0f, 0.0f, -1.0f,                       // Dirección
		0.7f, 0.025f, 0.025f,                    // Coeficientes de atenuación 
		20.0f);                                  // Ángulo de apertura (aprox 20°) 
	spotLightCount++;

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	movCoche = 0.0f;
	movOffset = 0.25f;
	rotllanta = 0.0f;
	rotllantaOffset = 10.0f;

	glm::vec3 lowerLight(0.0f, 0.0f, 0.0f);

	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec2 toffset = glm::vec2(0.0f, 0.0f);

	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		angulovaria += 0.05f * deltaTime;
		
		// --- LÓGICA DE REINICIO DE AUTO (Tecla R) ---
		if (mainWindow.getReiniciarAnimacion()) {
			// Reinicio Auto
			posAuto = glm::vec3(0.0f, -1.8f, 3.5f);
			rotAutoY = 0.0f;
			rotAutoX = 0.0f;
			rotAutoZ = 0.0f;
			distanciaRecorrida = 0.0f;
			rotllanta = 0.0f;

			// Apagamos la bandera de R
			mainWindow.apagarReinicio();
		}

		// --- LOGICA MOVIMIENTO DEL AUTO ---
		float velocidad = movOffset * deltaTime;

		// Nueva  máxima total: 195 unidades 
		if (distanciaRecorrida < 195.0f) {
			distanciaRecorrida += velocidad;
			rotllanta += rotllantaOffset * deltaTime;

			// 1. Definición de Secciones de Animación
			if (distanciaRecorrida <= 10.0f) { /* Recto inicial */ }
			else if (distanciaRecorrida > 10.0f && distanciaRecorrida <= 35.0f) { /* Sec 1 */
				float t_giro1 = (distanciaRecorrida - 10.0f) / 25.0f;
				rotAutoY = t_giro1 * -18.0f;
			}
			else if (distanciaRecorrida > 35.0f && distanciaRecorrida <= 40.0f) { /* Sec 2 */ }
			else if (distanciaRecorrida > 40.0f && distanciaRecorrida <= 50.0f) { /* Sec 3 */
				float t_subida = (distanciaRecorrida - 40.0f) / 10.0f;
				rotAutoY = -18.0f;
				rotAutoZ = t_subida * -5.0f;
			}
			else if (distanciaRecorrida > 50.0f && distanciaRecorrida <= 65.0f) { /* Sec 4 */
				float t_giro2 = (distanciaRecorrida - 50.0f) / 15.0f;
				rotAutoY = -18.0f + (t_giro2 * 73.0f);
				rotAutoZ = -5.0f + (t_giro2 * -5.0f);
				rotAutoX = t_giro2 * -15.0f;
			}
			else if (distanciaRecorrida > 65.0f && distanciaRecorrida <= 75.0f) { /* Sec 5 */
				rotAutoY = 55.0f;
				rotAutoZ = -10.0f;
				rotAutoX = -15.0f;
			}
			else if (distanciaRecorrida > 75.0f && distanciaRecorrida <= 90.0f) { /* Sec 6 */
				float t_giro3 = (distanciaRecorrida - 75.0f) / 15.0f;
				rotAutoY = 55.0f - (t_giro3 * 61.0f);
				rotAutoZ = -10.0f + (t_giro3 * -8.0f);
				rotAutoX = -15.0f + (t_giro3 * 21.0f);
			}
			else if (distanciaRecorrida > 90.0f && distanciaRecorrida <= 105.0f) { /* Sec 7 */
				float t_sec6 = (distanciaRecorrida - 90.0f) / 15.0f;
				rotAutoY = -6.0f + (t_sec6 * 1.0f);
				rotAutoZ = -18.0f + (t_sec6 * -13.0f);
				rotAutoX = 6.0f - (t_sec6 * 2.0f);
			}
			else if (distanciaRecorrida > 105.0f && distanciaRecorrida <= 115.0f) { /* Sec 8 */
				float t_sec7 = (distanciaRecorrida - 105.0f) / 10.0f;
				rotAutoY = -5.0f + (t_sec7 * 1.0f);
				rotAutoZ = -31.0f;
				rotAutoX = 4.0f;
			}
			else if (distanciaRecorrida > 115.0f && distanciaRecorrida <= 118.0f) { /* Sec 9 */
				float t_sec8 = (distanciaRecorrida - 115.0f) / 3.0f;
				rotAutoY = -4.0f + (t_sec8 * 1.0f);
				rotAutoZ = -31.0f + (t_sec8 * 11.0f);
				rotAutoX = 4.0f - (t_sec8 * 4.0f);
			}
			else if (distanciaRecorrida > 118.0f && distanciaRecorrida <= 123.0f) { /* Sec 10 */
				float t_sec9 = (distanciaRecorrida - 118.0f) / 5.0f;
				rotAutoY = -3.0f;
				rotAutoZ = -20.0f - (t_sec9 * 3.0f);
				rotAutoX = 0.0f;
			}
			else if (distanciaRecorrida > 123.0f && distanciaRecorrida <= 128.0f) { /* Sec 11 */
				rotAutoY = -3.0f;
				rotAutoZ = -22.0f;
				rotAutoX = 0.0f;
			}
			else if (distanciaRecorrida > 128.0f && distanciaRecorrida <= 135.0f) { /* Sec 12 */
				float t_sec12 = (distanciaRecorrida - 128.0f) / 7.0f;
				rotAutoY = -3.0f + (t_sec12 * 3.0f);
				rotAutoZ = -22.0f + (t_sec12 * 7.0f);
				rotAutoX = 0.0f;
			}
			else if (distanciaRecorrida > 135.0f && distanciaRecorrida <= 145.0f) { /* Sec 13 */
				float t_sec13 = (distanciaRecorrida - 135.0f) / 10.0f;
				rotAutoY = 0.0f + (t_sec13 * 12.0f);
				rotAutoZ = -15.0f + (t_sec13 * 13.0f);
				rotAutoX = 0.0f;
			}
			else if (distanciaRecorrida > 145.0f && distanciaRecorrida <= 150.0f) { /* Sec 14 */
				float t_sec14 = (distanciaRecorrida - 145.0f) / 5.0f;
				rotAutoY = 12.0f + (t_sec14 * 5.0f);
				rotAutoZ = -3.5f + (t_sec14 * 4.0f);
				rotAutoX = 0.0f;
			}
			else if (distanciaRecorrida > 150.0f && distanciaRecorrida <= 155.0f) { /* Sec 15 */
				float t_sec15 = (distanciaRecorrida - 150.0f) / 5.0f;
				rotAutoY = 12.0f + (t_sec15 * 5.0f);
				rotAutoZ = 2.0f + (t_sec15 * 1.4f);
				rotAutoX = 0.0f;
			}

			else if (distanciaRecorrida > 160.0f && distanciaRecorrida <= 165.0f) { /* Sec 16  */
				rotAutoY = 17.0f;
				rotAutoZ = 0.0f;
				rotAutoX = 0.0f;
			}
			// --- SECCIÓN 17: DESCENSO FINAL Y GIRO AJUSTADO (165 a 195) ---
			else if (distanciaRecorrida > 165.0f && distanciaRecorrida <= 195.0f) {
				rotAutoY = 19.0f;
				rotAutoZ = 5.0f;
				rotAutoX = 0.0f;
			}

			// 2. Cálculo de dirección (180 + 18 = 198 grados)
			float anguloDireccion = (180.0f + rotAutoY) * toRadians;
			dirAuto.x = cos(anguloDireccion);
			dirAuto.z = sin(anguloDireccion);

			// 3. Desplazar posición física
			if (distanciaRecorrida > 40.0f) {
				if (distanciaRecorrida > 165.0f) {
					// Descenso con 5 grados de inclinación
					posAuto.y -= (float)(sin(abs(rotAutoZ) * toRadians) * velocidad);
				}
				else {
					// Subida original
					posAuto.y += (float)(sin(abs(rotAutoZ) * toRadians) * velocidad);
				}
			}
			posAuto.x += dirAuto.x * velocidad;
			posAuto.z += dirAuto.z * velocidad;

		}
		else {
			// --- ESTADO FINAL ESTÁTICO (Unidad 195) ---
			rotAutoY = 19.0f;
			rotAutoZ = 5.0f;
			rotAutoX = 0.0f;
		}
		
		// --- LÓGICA DE INICIO ÚNICO DE NAVE (Tecla T) ---
		// Añadimos "!naveIniciada" para que solo entre si la nave NO ha comenzado
		if (mainWindow.getAccionT() && !naveIniciada) {
			// Posición inicial (aseguramos que esté en el origen de la animación)
			posNave = glm::vec3(-182.0f, 30.0f, -22.0f);
			rotNaveY = 0.0f;
			rotNaveX = 0.0f;
			rotNaveZ = 0.0f;
			distanciaNave = 0.0f;

			// Activamos el movimiento
			naveIniciada = true;

			// Apagamos la bandera de la ventana
			mainWindow.apagarAccionT();
		}
		else if (mainWindow.getAccionT() && naveIniciada) {
			// Opcional: Si se presiona T y ya está iniciada, solo se apaga la bandera 
			// de la ventana para que no se quede acumulada, pero no hace nada.
			mainWindow.apagarAccionT();
		}

		// Variables para la rotación de hélices y alas
		float rotHeliceContinua = glfwGetTime() * 800.0f;
		float anguloAleteo = 20.0f * sin(glfwGetTime() * 10.0f);
		
		// --- LOGICA MOVIMIENTO DE LA NAVE (Sección 10: Nivelación rápida en 10 unidades) ---
		float velNave = movOffset * deltaTime;
		// Límite total: 190.0
		if (naveIniciada && distanciaNave < 190.0f) {
			distanciaNave += velNave;
			if (distanciaNave <= 55.0f) {/* Sec 1 */
				rotNaveY = 19.0f;
				rotNaveZ = 0.0f;
			}
			else if (distanciaNave > 55.0f && distanciaNave <= 85.0f) {/* Sec 2 */
				float t_sec2 = (distanciaNave - 55.0f) / 30.0f;
				rotNaveY = 19.0f + (t_sec2 * -35.0f);
				rotNaveZ = t_sec2 * -30.0f;
			}
			else if (distanciaNave > 85.0f && distanciaNave <= 95.0f) {/* Sec 3 */
				rotNaveY = -16.0f;
				rotNaveZ = -30.0f;
			}
			else if (distanciaNave > 95.0f && distanciaNave <= 110.0f) {/* Sec 4 */
				float t_sec4 = (distanciaNave - 95.0f) / 15.0f;
				rotNaveY = -16.0f + (t_sec4 * 35.0f);
				rotNaveZ = -30.0f;
			}
			else if (distanciaNave > 110.0f && distanciaNave <= 122.0f) {/* Sec 5 */
				float t_sec5 = (distanciaNave - 110.0f) / 12.0f;
				rotNaveY = 19.0f + (t_sec5 * 27.0f);
				rotNaveZ = -30.0f + (t_sec5 * 5.0f);
			}
			else if (distanciaNave > 122.0f && distanciaNave <= 130.0f) {/* Sec 6 */
				float t_sec6 = (distanciaNave - 122.0f) / 8.0f;
				rotNaveY = 46.0f;
				rotNaveZ = -25.0f + (t_sec6 * 25.0f);
			}
			else if (distanciaNave > 130.0f && distanciaNave <= 143.0f) {/* Sec 7 */
				float t_sec7 = (distanciaNave - 130.0f) / 13.0f;
				rotNaveY = 46.0f + (t_sec7 * -61.0f);
				rotNaveZ = 0.0f;
			}
			else if (distanciaNave > 143.0f && distanciaNave <= 160.0f) {/* Sec 8 */
				float t_sec8 = (distanciaNave - 143.0f) / 17.0f;
				rotNaveY = -15.0f + (t_sec8 * 15.5f);
				rotNaveZ = t_sec8 * -5.0f;
			}
			else if (distanciaNave > 160.0f && distanciaNave <= 180.0f) {/* Sec 9 */
				float t_sec9 = (distanciaNave - 160.0f) / 20.0f;
				rotNaveY = 0.5f + (t_sec9 * -5.5f);
				rotNaveZ = -5.0f;
			}
			// SECCIÓN 10: (180 a 190) - NIVELACIÓN Z (10 unidades)
			else {
				float t_sec10 = (distanciaNave - 180.0f) / 10.0f;
				rotNaveY = -5.0f;
				// Z: De -5.0 a 0.0 en un tramo más corto
				rotNaveZ = -5.0f + (t_sec10 * 5.0f);
			}
			rotNaveX = 0.0f;
			// --- FÍSICA ---
			float anguloDireccionNave = rotNaveY * toRadians;
			dirNave.x = cos(anguloDireccionNave);
			dirNave.z = sin(anguloDireccionNave);
			posNave.x += dirNave.x * velNave;
			posNave.z += dirNave.z * velNave;
			if (rotNaveZ != 0.0f) {
				posNave.y += (float)(sin(rotNaveZ * toRadians) * velNave);
			}
		}
		else if (naveIniciada && distanciaNave >= 190.0f) {
			// ESTADO FINAL FIJO
			rotNaveY = -5.0f;
			rotNaveZ = 0.0f;
		}

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformTextureOffset = shaderList[0].getOffsetLocation(); // para la textura con movimiento

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);


		//Reinicializando variables cada ciclo de reloj
		model = glm::mat4(1.0);
		modelaux = glm::mat4(1.0);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Pista
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.1f, 2.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pista_M.RenderModel();

		
		// --- INSTANCIA DEL COCHE (HELLDIVERS) ---
		model = glm::mat4(1.0);
		model = glm::translate(model, posAuto);
		model = glm::rotate(model, -rotAutoY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f)); // Agregamos el signo '-'
		model = glm::rotate(model, rotAutoX * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, rotAutoZ * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));

		// Rotación de corrección base (mantenla como está si ya estaba alineado)
		model = glm::rotate(model, -90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;

		// --- ACTUALIZACIÓN DE LUZ FRONTAL (SPOTLIGHT 2) ---
		glm::vec4 offsetFrente = glm::vec4(0.0f, 0.5f, 2.0f, 1.0f);
		glm::vec3 posLuzAuto = glm::vec3(modelaux * offsetFrente);
		glm::vec3 dirLuzReal = glm::vec3(modelaux * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));
		spotLights[2].SetFlash(posLuzAuto, glm::normalize(dirLuzReal));
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		// --- RENDERIZADO DEL CUERPO ---
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Cuerpo_M.RenderModel();

		// --- COFRE ---
		model = glm::translate(modelaux, glm::vec3(0.0f, 0.72f, 1.1f));
		model = glm::rotate(model, 0 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cofre_M.RenderModel();

		// --- LLANTAS CON JERARQUÍA ---
		float wheelOffsets[4][3] = {
			{-0.5f, 0.3f, 1.4f},  // Delantera Izq
			{0.5f, 0.3f, 1.4f},   // Delantera Der
			{-0.5f, 0.3f, -1.5f}, // Trasera Izq
			{0.5f, 0.3f, -1.5f}   // Trasera Der
		};

		for (int i = 0; i < 4; i++) {
			model = glm::translate(modelaux, glm::vec3(wheelOffsets[i][0], wheelOffsets[i][1], wheelOffsets[i][2]));
			// Las llantas rotan sobre su propio eje X local
			model = glm::rotate(model, rotllanta * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

			if (i == 0) LlantaD1_M.RenderModel();
			else if (i == 1) LlantaD2_M.RenderModel();
			else if (i == 2) LlantaT1_M.RenderModel();
			else LlantaT2_M.RenderModel();
		}
		

		// --- RENDERIZADO CUERPO NAVE ---
		model = glm::mat4(1.0);
		model = glm::translate(model, posNave);
		model = glm::rotate(model, -rotNaveY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotNaveX * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, rotNaveZ * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));

		// Corrección base para alinear el modelo original (el 69.0f que ya usabas)
		model = glm::rotate(model, 90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.5f, 3.5f, 3.5f));
		modelaux = model; // Matriz base para las alas y hélices jerárquicas

		//Hay un error extraño que hace que al poner la luz esta cargue mal y openGL estalle en colores
		/*
		// ========== LUZ DE LA NAVE ==========
		glm::vec4 offsetLuzNave = glm::vec4(0.0f, -1.2f, 0.0f, 1.0f);  
		glm::vec3 posLuzNave = glm::vec3(modelaux * offsetLuzNave);
		glm::vec3 dirLuzNave = glm::vec3(0.0f, -1.0f, 0.0f);

		spotLights[3].SetFlash(posLuzNave, dirLuzNave);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);
		*/
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Nave_M.RenderModel();

		// --- ALA DERECHA (JERARQUÍA RELATIVA CON ALETEO) ---
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.2f, 0.0f, 0.3f));
		model = glm::rotate(model, anguloAleteo * toRadians, glm::vec3(0.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AlaDer_M.RenderModel();

		// --- ALA IZQUIERDA (JERARQUÍA RELATIVA CON ALETEO) ---
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.2f, 0.0f, 0.3f));
		model = glm::rotate(model, -anguloAleteo * toRadians, glm::vec3(0.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AlaIzq_M.RenderModel();

		// --- HÉLICE DERECHA ---
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.35f, -0.3f, 0.37f));
		model = glm::rotate(model, rotHeliceContinua * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		HeliceDer_M.RenderModel();

		// --- HÉLICE IZQUIERDA ---
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.35f, -0.3f, 0.37f));
		model = glm::rotate(model, -rotHeliceContinua * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		HeliceIzq_M.RenderModel();

		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}
