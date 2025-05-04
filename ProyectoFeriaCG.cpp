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

//La musica
#include "inc/fmod.hpp"
#include <inc/fmod_errors.h>

FMOD::System* fmodSystem;
FMOD::Sound* music;
FMOD::Channel* channel = nullptr;


//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;






float sunAngle = 0.0f;
//float sunSpeed = 0.05f;
float sunSpeed = 0.1f; //Velocidad de rotación del sol

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture pisoTexture;
Texture pisoETexture; //Piso del kiosko
Texture pisoH; //Piso Hora de aventura
Texture pisoB; //Piso Bob Esponja
Texture pisoL; //Piso Los padrinos magicos
Texture camino; //Piso camino de conexión

//Modelos hora de aventura
Model BMO;
Model CasaDelArbol;
Model JakeCuerpo;
Model JakeBrazoDer;
Model JakeBrazoIzq;
Model JakePiernaDer;
Model JakePiernaIzq;
Model Prismo;
Model MarcelinesGuitar;




//Modelos Bob esponja
Model Banca;
Model BancaCom;
Model LamparaBob;
Model CasaBob;
Model BobBrazoDer;
Model BobBrazoIzq;
Model BobCuerpo;
Model BobPiernaDer;
Model BobPiernaIzq;
Model Cangre;
Model CasaCalam;
Model CasaPatricio;
Model CrustaceoCas;
Model FredN;
Model HaroldN;
Model Jaula;
Model Pizzas;
Model Topo;
Model Bat;
Model Martillo;
Model Medusa1;
Model Pelota;



//Modelos Los padrinos magicos
Model varita;


Skybox skyboxDay;
Skybox skyboxNight;

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

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

//Función para la musica con Fmod
void iniciarFMOD() {
	FMOD::System_Create(&fmodSystem);
	fmodSystem->init(512, FMOD_INIT_NORMAL, 0);
	fmodSystem->createSound("sounds/Fondo_Feria.wav", FMOD_LOOP_NORMAL, 0, &music);
	fmodSystem->playSound(music, 0, false, &channel);
}

void actualizarFMOD() {
	fmodSystem->update();
}

void liberarFMOD() {
	music->release();
	fmodSystem->close();
	fmodSystem->release();
}

//Creación de bancas para decorar
void RenderBanca(glm::vec3 posicion, float rotY, GLuint uniformModel, glm::vec3 escala = glm::vec3(1.0f)) {
	glm::mat4 base = glm::mat4(1.0f);
	base = glm::translate(base, posicion);
	base = glm::rotate(base, rotY, glm::vec3(0.0f, 1.0f, 0.0f));
	base = glm::scale(base, escala);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(base));
	Banca.RenderModel();
}

//Creación de bancas comedor para decorar
void RenderBancaCom(glm::vec3 posicion, float rotY, GLuint uniformModel, glm::vec3 escala = glm::vec3(0.7f)) {
	glm::mat4 base = glm::mat4(1.0f);
	base = glm::translate(base, posicion);
	base = glm::rotate(base, rotY, glm::vec3(0.0f, 1.0f, 0.0f));
	base = glm::scale(base, escala);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(base));
	BancaCom.RenderModel();
}

//Creación lamparas 
void RenderLamparaBob(glm::vec3 posicion, float rotY, GLuint uniformModel, glm::vec3 escala = glm::vec3(0.7f)) {
	glm::mat4 base = glm::mat4(1.0f);
	base = glm::translate(base, posicion);
	base = glm::rotate(base, rotY, glm::vec3(0.0f, 1.0f, 0.0f));
	base = glm::scale(base, escala);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(base));
	LamparaBob.RenderModel();
}

void RenderVarita(glm::vec3 posicion, float rotY, GLuint uniformModel, glm::vec3 escala = glm::vec3(1000.0f)) {
	glm::mat4 base = glm::mat4(1.0f);
	base = glm::translate(base, posicion);
	base = glm::rotate(base, rotY, glm::vec3(0.0f, 1.0f, 0.0f));
	base = glm::scale(base, escala);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(base));
	varita.RenderModel();
}


int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();





	mainLight.SetAmbientIntensity(0.05f); //intensidad de la luz
	mainLight.SetDiffuseIntensity(0.05f); //intensidad de la luz difusa


	CreateObjects();
	CreateShaders();
	iniciarFMOD(); //iniciar la musica


	camera = Camera(glm::vec3(-10.0f, 0.0f, 10.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);
	
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();

	pisoH = Texture("Textures/pisoH.tga");
	pisoH.LoadTextureA();
	pisoB = Texture("Textures/pisoB.tga");
	pisoB.LoadTextureA();
	pisoL = Texture("Textures/pisoL.tga");
	pisoL.LoadTextureA();
	pisoETexture = Texture("Textures/pisoE.tga");
	pisoETexture.LoadTextureA();

	camino = Texture("Textures/camino.tga");
	camino.LoadTextureA();

	//Modelos hora de aventura
	BMO = Model();
	BMO.LoadModel("Models/BMO.obj");
	CasaDelArbol = Model();
	CasaDelArbol.LoadModel("Models/CasaDelArbol.obj");
	JakeCuerpo = Model();
	JakeCuerpo.LoadModel("Models/CuerpoJake.obj");
	JakeBrazoDer = Model();
	JakeBrazoDer.LoadModel("Models/BrazoDerechoJake.obj");
	JakeBrazoIzq = Model();
	JakeBrazoIzq.LoadModel("Models/BrazoIzquierdoJake.obj");
	JakePiernaDer = Model();
	JakePiernaDer.LoadModel("Models/PiernaDerechaJake.obj");
	JakePiernaIzq = Model();
	JakePiernaIzq.LoadModel("Models/PiernaIzquierdaJake.obj");
	Prismo = Model();
	Prismo.LoadModel("Models/Prismo.obj");
	MarcelinesGuitar = Model();
	MarcelinesGuitar.LoadModel("Models/MarcelinesGuitar.obj");





	//Modelos Bob esponja
	Banca = Model();
	Banca.LoadModel("Models/Banca.obj");

	BancaCom = Model();
	BancaCom.LoadModel("Models/BancaCom.obj");

	LamparaBob = Model();
	LamparaBob.LoadModel("Models/LamparaBob.obj");

	CasaBob = Model();
	CasaBob.LoadModel("Models/CasaBob.obj");

	BobBrazoDer = Model();
	BobBrazoDer.LoadModel("Models/BobBrazoDer1.obj");

	BobBrazoIzq = Model();
	BobBrazoIzq.LoadModel("Models/BobBrazoIzq1.obj");

	BobCuerpo = Model();
	BobCuerpo.LoadModel("Models/BobCuerpo1.obj");

	BobPiernaDer = Model();
	BobPiernaDer.LoadModel("Models/BobPiernaDer1.obj");

	BobPiernaIzq = Model();
	BobPiernaIzq.LoadModel("Models/BobPiernaIzq1.obj");

	Cangre = Model();
	Cangre.LoadModel("Models/Cangre.obj");

	CasaCalam = Model();
	CasaCalam.LoadModel("Models/CasaCalam.obj");

	CasaPatricio = Model();
	CasaPatricio.LoadModel("Models/CasaPatricio.obj");

	CrustaceoCas = Model();
	CrustaceoCas.LoadModel("Models/CrustaceoCas.obj");

	FredN = Model();
	FredN.LoadModel("Models/FredN.obj");

	HaroldN = Model();
	HaroldN.LoadModel("Models/HaroldN.obj");

	Jaula = Model();
	Jaula.LoadModel("Models/Jaula.obj");

	Pizzas = Model();
	Pizzas.LoadModel("Models/Pizzas.obj");

	Topo = Model();
	Topo.LoadModel("Models/Topo.obj");

	Bat = Model();
	Bat.LoadModel("Models/Bat.obj");

	Martillo = Model();
	Martillo.LoadModel("Models/Martillo.obj");

	Medusa1 = Model();
	Medusa1.LoadModel("Models/Medusa1.obj");

	Pelota = Model();
	Pelota.LoadModel("Models/Pelota.obj");








	//Modelos Los padrinos magicos

	varita = Model();
	varita.LoadModel("Models/varita.obj");







	//Skybox
	std::vector<std::string> skyboxFacesDay;
	skyboxFacesDay.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFacesDay.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFacesDay.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFacesDay.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFacesDay.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFacesDay.push_back("Textures/Skybox/cupertin-lake_ft.tga");
	skyboxDay = Skybox(skyboxFacesDay);

	std::vector<std::string> skyboxFacesNight;
	skyboxFacesNight.push_back("Textures/Skybox/cupertin-lake-night_rt.tga");
	skyboxFacesNight.push_back("Textures/Skybox/cupertin-lake-night_lf.tga");
	skyboxFacesNight.push_back("Textures/Skybox/cupertin-lake-night_dn.tga");
	skyboxFacesNight.push_back("Textures/Skybox/cupertin-lake-night_up.tga");
	skyboxFacesNight.push_back("Textures/Skybox/cupertin-lake-night_bk.tga");
	skyboxFacesNight.push_back("Textures/Skybox/cupertin-lake-night_ft.tga");
	skyboxNight = Skybox(skyboxFacesNight);


	//



	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 1.0f, 0.0f); //Iluminación del sol //Dirección inicial
	
	
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	unsigned int spotLightCount = 0;



	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		

		if (mainWindow.getLantern() == 1)
		{
			//linterna
			spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
				0.0f, 2.0f,
				0.0f, 0.0f, 0.0f,
				0.0f, -1.0f, 0.0f,
				1.0f, 0.0f, 0.0f,
				5.0f);
			spotLightCount++;//Encender lampara
		}
		else if (mainWindow.getLantern() == 0)
		{
			//linterna
			spotLights[0] = SpotLight(0.0f, 0.0f, 0.0f,
				0.0f, 2.0f,
				0.0f, 0.0f, 0.0f,
				0.0f, -1.0f, 0.0f,
				1.0f, 0.0f, 0.0f,
				5.0f);
			spotLightCount--; //Apagar lampara
		}



		// luz ligada a la cámara de tipo flash
		//sirve para que en tiempo de ejecución (dentro del while) se cambien propiedades de la luz
		
		
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection()); //Luz se mueva respecto a la camara

		//información al shader de fuentes de iluminación
		





		//Actualizar ángulo del sol
		sunAngle += sunSpeed * deltaTime;
		if (sunAngle > 360.0f)
			sunAngle -= 360.0f;	
		float radians = glm::radians(sunAngle); //Convertir a radianes
		glm::vec3 direction = glm::vec3(0.0f, sin(radians), -cos(radians)); //Calcular dirección	
		pointLightCount = 0; //Reiniciar contador de luces puntuales

		//Cambio para que la transición sea más suave
		//Función dinamica de actualización encendido de luces


		if (direction.y > 0.2f) {
			skyboxDay.DrawSkybox(camera.calculateViewMatrix(), projection);
		}
		else if (direction.y <= 0.2f) {
			skyboxNight.DrawSkybox(camera.calculateViewMatrix(), projection);
		
			//Camara
			glm::vec3 camPos = camera.getCameraPosition();			
			//Luces
			// Luz poste
			if (glm::distance(camPos, glm::vec3(-48.0f, 0.0f, -48.0f)) < 100.0f) {
				spotLights[1] = SpotLight(1.0f, 1.0f, 1.0f,
					1.0f, 1.5f,
					-48.0f, 8.0f, -48.0f,
					0.5f, -1.0f, 0.5f,
					1.0f, 0.0f, 0.0f,
					50.0f);
				spotLightCount++;
			}
			else {
				spotLights[1] = SpotLight(0.0f, 0.0f, 0.0f,
					1.0f, 1.5f,
					-48.0f, 8.0f, -48.0f,
					0.5f, -1.0f, 0.5f,
					1.0f, 0.0f, 0.0f,
					50.0f);
				spotLightCount--;
			}

			//Luz poste
			if (glm::distance(camPos, glm::vec3(48.0f, 0.0f, -48.0f)) < 100.0f) {
				spotLights[2] = SpotLight(1.0f, 1.0f, 1.0f,
					1.0f, 1.5f,
					48.0f, 8.0f, -48.0f,
					-0.5f, -1.0f, 0.5f,
					1.0f, 0.0f, 0.0f,
					50.0f);
				spotLightCount++;
			}
			else {
				spotLights[2] = SpotLight(0.0f, 0.0f, 0.0f,
					1.0f, 1.5f,
					48.0f, 8.0f, -48.0f,
					-0.5f, -1.0f, 0.5f,
					1.0f, 0.0f, 0.0f,
					50.0f);
				spotLightCount--;
			}

			//Luz poste
			if (glm::distance(camPos, glm::vec3(-48.0f, 0.0f, 48.0f)) < 100.0f) {
				spotLights[3] = SpotLight(1.0f, 1.0f, 1.0f,
					1.0f, 1.5f,
					-48.0f, 8.0f, 48.0f,
					0.5f, -1.0f, -0.5f,
					1.0f, 0.0f, 0.0f,
					50.0f);
				spotLightCount++;
			}
			else {
				spotLights[3] = SpotLight(0.0f, 0.0f, 0.0f,
					1.0f, 1.5f,
					-48.0f, 8.0f, 48.0f,
					0.5f, -1.0f, -0.5f,
					1.0f, 0.0f, 0.0f,
					50.0f);
				spotLightCount--;
			}

			//Luz poste
			if (glm::distance(camPos, glm::vec3(48.0f, 0.0f, 48.0f)) < 100.0f) {
				spotLights[4] = SpotLight(1.0f, 1.0f, 1.0f,
					1.0f, 1.5f,
					48.0f, 8.0f, 48.0f,
					-0.5f, -1.0f, -0.5f,
					1.0f, 0.0f, 0.0f,
					50.0f);
				spotLightCount++;
			}
			else {
				spotLights[4] = SpotLight(0.0f, 0.0f, 0.0f,
					1.0f, 1.5f,
					48.0f, 8.0f, 48.0f,
					-0.5f, -1.0f, -0.5f,
					1.0f, 0.0f, 0.0f,
					50.0f);
				spotLightCount--;
			}

			
			//Luces de las varitas
			struct VaritaLight {
				glm::vec3 position;
			};

			std::vector<VaritaLight> varitas = {
				{{320.0f, 55.f, 0.0f}},
				{{300.0f, 55.f, 499.0f}},
				{{-320.0f, 55.f, 0.0f}},
				{{-300.0f, 55.f, 499.0f}},
				{{0.0f,   55.f, 320.0f}},
				{{300.0f, 55.f, -320.0f}},
				{{-300.0f,55.f, -320.0f}},
				{{0.0f,   55.f, -499.0f}}
			};

			for (int i = 0; i < varitas.size() && pointLightCount < MAX_POINT_LIGHTS; ++i) {
				if (glm::distance(camPos, varitas[i].position) < 100.0f) {
					pointLights[pointLightCount] = PointLight(1.0f, 1.0f, 1.0f,
						2.0f, 3.0f,
						varitas[i].position.x, varitas[i].position.y, varitas[i].position.z,
						0.3f, 0.05f, 0.01f);
					pointLightCount++;
				}
			}





		}
	






		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);


		//Nueva implemantación cambio de luz de día a noche con interpolación

		mainLight.SetDirection(direction); //Actualizar dirección de la luz direccional

		float intensidadMin = 0.5f;
		float intensidadMax = 1.0f;

		// Clamp la dirección Y para que siempre esté entre 0 y 1
		float t = glm::clamp(direction.y, 0.0f, 1.0f);

		// Interpolación de luz según la posición del sol
		float ambient = glm::mix(intensidadMin, intensidadMax, t);
		float diffuse = glm::mix(intensidadMin, intensidadMax, t);

		mainLight.SetAmbientIntensity(ambient);
		mainLight.SetDiffuseIntensity(diffuse);

		//Esto ya estaba
		shaderList[0].SetDirectionalLight(&mainLight); //Mainlight para el sol
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);


		//Piso principal escenario
		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(60.0f, 1.0f, 60.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Piso de cada isla
		
		//Isla Hora de aventura
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-300.0f, -0.8f, 300.0f));
		model = glm::scale(model, glm::vec3(20.0f, 1.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		pisoH.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Isla padrinos magicos		
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(300.0f, -0.8f, 300.0f));
		model = glm::scale(model, glm::vec3(20.0f, 1.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pisoL.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();


		//Isla Bob Esponja
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.8f, -300.0f));
		model = glm::scale(model, glm::vec3(20.0f, 1.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pisoB.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Kiosko 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.8f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 1.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pisoETexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();


		//Caminos de conexion
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.8f, -75.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 2.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		camino.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Caminos de conexion
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.8f, 170.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 12.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		camino.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Caminos de conexion
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.8f, 300.0f));
		model = glm::scale(model, glm::vec3(10.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		camino.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Caminos de conexion
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(300.0f, -0.8f, -95.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 19.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		camino.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Caminos de conexion
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-300.0f, -0.8f, -95.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 19.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		camino.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Caminos de conexion
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(255.0f, -0.8f, -300.0f));
		model = glm::scale(model, glm::vec3(5.5f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		camino.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Caminos de conexion
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-255.0f, -0.8f, -300.0f));
		model = glm::scale(model, glm::vec3(5.5f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		camino.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();





		//Colocar modelos hora de aventura
		//BMO
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-300.0f, -0.2f, 400.0));
		model = glm::scale(model, glm::vec3(30.0f, 30.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BMO.RenderModel();

		//Casa del arbol
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-400.0f, -0.2f, 400.0));
		model = glm::rotate(model, 3.14f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(7.0f, 7.0f, 7.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CasaDelArbol.RenderModel();
		
		//Jake el perro
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f));  
		model = glm::scale(model, glm::vec3(1.0f));                  
		glm::mat4 modelJake = model;                                
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		JakeCuerpo.RenderModel();

		model = modelJake;
		model = glm::translate(model, glm::vec3(-1.0f, 2.0f, 0.0f)); 
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		JakeBrazoDer.RenderModel();

		model = modelJake;
		model = glm::translate(model, glm::vec3(0.95f, 2.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		JakeBrazoIzq.RenderModel();

		model = modelJake;
		model = glm::translate(model, glm::vec3(-0.38f, 0.2f, -0.03f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		JakePiernaDer.RenderModel();

		model = modelJake;
		model = glm::translate(model, glm::vec3(0.51f, 0.2f, -0.03f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		JakePiernaIzq.RenderModel();

		//Prismo
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-200.0f, 0.5f, 470.0));
		model = glm::scale(model, glm::vec3(23.0f, 23.0f, 23.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Prismo.RenderModel();

		//Guitarra de Marceline
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-200.0f, 2.5f, 450.0));
		//model = glm::rotate(model, 3.14f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(100.5f, 100.5f, 100.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		MarcelinesGuitar.RenderModel();


















		//Colocar modelos Bob esponja
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 8.8f, -250.0));
		model = glm::rotate(model, 3.14f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BobCuerpo.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(1.9f, -1.0f, 0.0));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BobBrazoDer.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.9f, -1.0f, 0.0));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BobBrazoIzq.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.8f, -2.5f, 0.0));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BobPiernaDer.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.8f, -2.5f, 0.0));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BobPiernaIzq.RenderModel();

		//CASAS
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(140.0f, 0.0f, -420.0));
		model = glm::rotate(model, 3.14f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CasaBob.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-130.0f, 0.0f, -450.0));
		model = glm::rotate(model, 3.14f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.8f, 2.8f, 2.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CasaCalam.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-130.0f, -3.0f, -140.0));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CasaPatricio.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(160.0f, -4.0f, -160.0));
		model = glm::rotate(model, 1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.3f, 2.3f, 2.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CrustaceoCas.RenderModel();

		//TOPO
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-130.0f, 0.0f, -210.0));
		model = glm::rotate(model, -1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.3f, 2.3f, 2.3f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Topo.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(1.3f, 4.0f, 0.4));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Medusa1.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.1f, 4.0f, 0.4));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Medusa1.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.4f, 4.0f, 0.4));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Medusa1.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.0f, 3.4f, -1.1));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Medusa1.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.8f, 3.4f, -1.1));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Medusa1.RenderModel();

		//JAULA
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 10.0f, -420.0));
		model = glm::scale(model, glm::vec3(2.3f, 2.3f, 2.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Jaula.RenderModel();

		//PIZZAS
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-170.0f, 13.0f, -340.0));
		model = glm::rotate(model, 1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.32f, 0.32f, 0.32f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pizzas.RenderModel();

		//CANGREBURGUERS
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(50.0f, 9.4f, -120.0));
		model = glm::rotate(model, 3.14f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.3f, 1.3f, 1.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cangre.RenderModel();


		//Bancas
		RenderBanca(glm::vec3(-48.0f, -1.0f, 25.0f), -1.6f, uniformModel);
		RenderBanca(glm::vec3(48.0f, -1.0f, 25.0f), 1.6f, uniformModel);
		RenderBanca(glm::vec3(-48.0f, -1.0f, -25.0f), -1.6f, uniformModel);
		RenderBanca(glm::vec3(48.0f, -1.0f, -25.0f), 1.6f, uniformModel);



		//Mesas de comer
		RenderBancaCom(glm::vec3(30.0f, -1.0f, -40.0f), 0.0f, uniformModel);
		RenderBancaCom(glm::vec3(-30.0f, -1.0f, -40.0f), 0.0f, uniformModel);
		RenderBancaCom(glm::vec3(30.0f, -1.0f, 40.0f), 0.0f, uniformModel);
		RenderBancaCom(glm::vec3(-30.0f, -1.0f, 40.0f), 0.0f, uniformModel);



		//Lamparas
		//En kiosko
		RenderLamparaBob(glm::vec3(-49.0f, -0.8f, -49.0f), -0.5f, uniformModel);
		RenderLamparaBob(glm::vec3(49.0f, -0.8f, -49.0f), 3.5f, uniformModel);
		RenderLamparaBob(glm::vec3(-49.0f, -0.8f, 49.0f), 0.5f, uniformModel);
		RenderLamparaBob(glm::vec3(49.0f, -0.8f, 49.0f), 2.5f, uniformModel);
		















		//Colocar modelos Los padrinos magicos

		//Varitas
		RenderVarita(glm::vec3(320.0f, -0.8f, 0.0f), 0.0f, uniformModel);
		RenderVarita(glm::vec3(300.0f, -0.8f, 499.0f), 1.5f, uniformModel);


		RenderVarita(glm::vec3(-320.0f, -0.8f, 0.0f), 0.0f, uniformModel);
		RenderVarita(glm::vec3(-300.0f, -0.8f, 499.0f), 1.5f, uniformModel);

		RenderVarita(glm::vec3(0.0f, -0.8f, 320.0f), 1.5f, uniformModel);


		RenderVarita(glm::vec3(300.0f, -0.8f, -320.0f), 1.5f, uniformModel);
		RenderVarita(glm::vec3(-300.0f, -0.8f, -320.0f), 1.5f, uniformModel);
		RenderVarita(glm::vec3(0.0f, -0.8f, -499.0f), 1.5f, uniformModel);








		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
