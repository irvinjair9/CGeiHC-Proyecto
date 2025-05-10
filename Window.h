#pragma once
#include<stdio.h>
#include<glew.h>
#include<glfw3.h>

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	int Initialise();
	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }
	GLfloat getXChange();
	GLfloat getYChange();
	GLfloat getmuevex() { return muevex; }
	bool getShouldClose() {
		return  glfwWindowShouldClose(mainWindow);
	}
	bool* getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }

	// Variables para movimiento y animaci�n de Jake
	GLfloat getPosX() { return posX; }
	GLfloat getPosZ() { return posZ; }
	GLfloat getBrazoDerAng() { return brazoDerAng; }
	GLfloat getBrazoIzqAng() { return brazoIzqAng; }
	GLfloat getPiernaDerAng() { return piernaDerAng; }
	GLfloat getPiernaIzqAng() { return piernaIzqAng; }
	GLfloat getDireccion() { return direccion; }

	GLfloat getLantern() { return Lantern; }
	GLfloat getJuego() { return Juego; }

	GLfloat getDeseo() { return Deseo; }

	GLfloat getCamaraAerea() { return CamaraAerea; }//Camara aerea

	GLfloat getCamaraPersona() { return CamaraPersona; }//Camara en tercera persona

	// Funci�n para actualizar la animaci�n de Jake
	void actualizarAnimacionJake(float yawCamara);

	~Window();
private:
	GLFWwindow* mainWindow;
	GLint width, height;



	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	GLfloat muevex;
	GLfloat Lantern;//para linterna
	GLfloat Juego;//para los juego

	GLfloat CamaraAerea;
	GLfloat CamaraPersona;

	GLfloat Deseo;//para luces al pedir deseo

	


	bool mouseFirstMoved;

// Variables para el movimiento y animaci�n de Jake
	GLfloat posX, posZ;          // Posici�n de Jake
	GLfloat direccion;           // Direcci�n a la que mira Jake
	GLfloat brazoDerAng;         // �ngulo del brazo derecho
	GLfloat brazoIzqAng;         // �ngulo del brazo izquierdo
	GLfloat piernaDerAng;        // �ngulo de la pierna derecha
	GLfloat piernaIzqAng;        // �ngulo de la pierna izquierda
	bool enMovimiento;           // Indica si Jake est� en movimiento
	GLfloat velocidadAnim;       // Velocidad de la animaci�n
	GLfloat rangoAnim;           // Rango de movimiento de las extremidades
	bool direccionAnim;          // Direcci�n de la animaci�n (oscilaci�n)

	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);

};

