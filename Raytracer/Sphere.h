/// \file Sphere.h
/// \brief handles all sphere properties
/// \author Josh Bailey

#ifndef _SPHERE_H_
#define _SPHERE_H_

//File includes
#include <iostream>
#include <glm.hpp>

#include "Shape.h"

class Sphere : public Shape		//Inheritance from Shape
{
public:
	//Variables
	float m_radius;

	//Functions
	Sphere();
	Sphere(glm::vec3 _position, float _radius, glm::vec3 _colour);
	bool Intersection(float *_t, glm::vec3 _OriginOfRay, glm::vec3 _directionOfRay);
	glm::vec3 NormalCalculation(glm::vec3 _p0, int *_shine, glm::vec3* _colourOfDiffuse, glm::vec3 *_colourOfSpecular);
};

#endif // _SPHERE_H_