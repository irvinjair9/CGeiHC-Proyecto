#include "DirectionalLight.h"

DirectionalLight::DirectionalLight() : Light()
{
	direction = glm::vec3(0.0f, -1.0f, 0.0f);
}

DirectionalLight::DirectionalLight(GLfloat red, GLfloat green, GLfloat blue,
									GLfloat aIntensity, GLfloat dIntensity,
									GLfloat xDir, GLfloat yDir, GLfloat zDir) : Light(red, green, blue, aIntensity, dIntensity)
{
	direction = glm::vec3(xDir, yDir, zDir);
}

void DirectionalLight::UseLight(GLfloat ambientIntensityLocation, GLfloat ambientcolorLocation,
	GLfloat diffuseIntensityLocation, GLfloat directionLocation)
{
	glUniform3f(ambientcolorLocation, color.x, color.y, color.z);
	glUniform1f(ambientIntensityLocation, ambientIntensity);

	glUniform3f(directionLocation, direction.x, direction.y, direction.z);
	glUniform1f(diffuseIntensityLocation, diffuseIntensity);
}

void DirectionalLight::SetDirection(glm::vec3 dir)
{
	direction = glm::normalize(dir);
}
// Cambia la direcci�n de la luz direccional
void DirectionalLight::SetAmbientIntensity(GLfloat aIntensity) {
	ambientIntensity = aIntensity;
}
void DirectionalLight::SetDiffuseIntensity(GLfloat dIntensity) {
	diffuseIntensity = dIntensity;
}


DirectionalLight::~DirectionalLight()
{
}
