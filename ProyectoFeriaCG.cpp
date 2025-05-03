#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>




#include <stdio.h>


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
float sunSpeed = 0.1f;

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



//Modelos Bob esponja
Model Banca;
Model BancaCom;
Model LamparaBob;



//Modelos Los padrinos magicos


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






	//Modelos Bob esponja
	Banca = Model();
	Banca.LoadModel("Models/Banca.obj");

	BancaCom = Model();
	BancaCom.LoadModel("Models/BancaCom.obj");

	LamparaBob = Model();
	LamparaBob.LoadModel("Models/LamparaBob.obj");








	//Modelos Los padrinos magicos








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

		if (direction.y <= 0.0f) {
			skyboxNight.DrawSkybox(camera.calculateViewMatrix(), projection); //Dibujado ambiente
		}
		else {
			skyboxDay.DrawSkybox(camera.calculateViewMatrix(), projection);
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




		mainLight.SetDirection(direction); //Actualizar dirección de la luz direccional

		if (direction.y <= -0.05f) {
			mainLight.SetAmbientIntensity(0.2f); //Bajar intensidad
			mainLight.SetDiffuseIntensity(0.2f);
		}
		else if (direction.y >= 0.05f) {
			mainLight.SetAmbientIntensity(0.5f); //Subir intensidad
			mainLight.SetDiffuseIntensity(0.5f);
		}

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
		model = glm::translate(model, glm::vec3(-400.0f, -0.5f, 400.0f));
		model = glm::scale(model, glm::vec3(15.0f, 1.0f, 15.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		pisoH.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Isla padrinos magicos		
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(400.0f, -0.5f, 400.0f));
		model = glm::scale(model, glm::vec3(15.0f, 1.0f, 15.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pisoL.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();


		//Isla Bob Esponja
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.5f, -400.0f));
		model = glm::scale(model, glm::vec3(15.0f, 1.0f, 15.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pisoB.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Kiosko 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 1.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pisoETexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();


		//Caminos de conexion
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.5f, -150.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		camino.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Caminos de conexion
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.5f, 230.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 18.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		camino.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Caminos de conexion
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.5f, 400.0f));
		model = glm::scale(model, glm::vec3(25.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		camino.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Caminos de conexion
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(400.0f, -0.5f, -80.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 33.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		camino.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Caminos de conexion
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-400.0f, -0.5f, -80.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 33.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		camino.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Caminos de conexion
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(270.0f, -0.5f, -400.0f));
		model = glm::scale(model, glm::vec3(12.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		camino.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Caminos de conexion
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-270.0f, -0.5f, -400.0f));
		model = glm::scale(model, glm::vec3(12.0f, 1.0f, 1.0f));
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


		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-400.0f, -0.2f, 400.0));
		model = glm::rotate(model, 3.14f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(7.0f, 7.0f, 7.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CasaDelArbol.RenderModel();

		
















		//Colocar modelos Bob esponja
		
		RenderBanca(glm::vec3(-48.0f, -1.0f, 25.0f), -1.6f, uniformModel);
		RenderBanca(glm::vec3(48.0f, -1.0f, 25.0f), 1.6f, uniformModel);
		RenderBanca(glm::vec3(-48.0f, -1.0f, -25.0f), -1.6f, uniformModel);
		RenderBanca(glm::vec3(48.0f, -1.0f, -25.0f), 1.6f, uniformModel);



		//Mesa de comer 1
		RenderBancaCom(glm::vec3(30.0f, -1.0f, -40.0f), 0.0f, uniformModel);
		RenderBancaCom(glm::vec3(-30.0f, -1.0f, -40.0f), 0.0f, uniformModel);
		RenderBancaCom(glm::vec3(30.0f, -1.0f, 40.0f), 0.0f, uniformModel);
		RenderBancaCom(glm::vec3(-30.0f, -1.0f, 40.0f), 0.0f, uniformModel);
























		//Colocar modelos Los padrinos magicos



























		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
