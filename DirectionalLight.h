#pragma once
#include "Light.h"

class DirectionalLight :
	public Light
{
public:
	DirectionalLight();
	DirectionalLight(GLfloat red, GLfloat green, GLfloat blue, 
					GLfloat aIntensity, GLfloat dIntensity,
					GLfloat xDir, GLfloat yDir, GLfloat zDir);

	void UseLight(GLfloat ambientIntensityLocation, GLfloat ambientcolorLocation,
		GLfloat diffuseIntensityLocation, GLfloat directionLocation);
	//Para el ciclo de luz direccional
	void SetDirection(glm::vec3 dir);
	void SetAmbientIntensity(GLfloat aIntensity);
	void SetDiffuseIntensity(GLfloat dIntensity);

	~DirectionalLight();

private:
	glm::vec3 direction;
};

