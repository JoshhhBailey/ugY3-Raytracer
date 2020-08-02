/// \file Sphere.h
/// \brief handles all plane properties
/// \author Josh Bailey

#ifndef _PLANE_H_
#define _PLANE_H_

//File includes
#include <glm.hpp>

#include "Shape.h"

class Plane : public Shape	//Inheritance from Shape
{
public:
	//Variables
	glm::vec3 m_normalOfPlane;
	
	//Functions
	Plane();
	Plane(glm::vec3 _position, glm::vec3 _normal, glm::vec3 _colour);
	bool Intersection(float *_t, glm::vec3 _originOfRay, glm::vec3 _directionOfRay);
	glm::vec3 NormalCalculation(glm::vec3 _p0, int *_shine, glm::vec3* _colourOfDiffuse, glm::vec3 *_colourOfSpecular);
};

#endif //_PLANE_H_