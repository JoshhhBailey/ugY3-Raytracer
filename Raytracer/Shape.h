/// \file Shape.h
/// \brief base class that all shapes inherit from
/// \author Josh Bailey

#ifndef _SHAPE_H_
#define _SHAPE_H_

//File includes
#include <glm.hpp>

class Shape
{
public:
	//Variables
	glm::vec3 m_position;
	glm::vec3 m_colour;
	glm::vec3 m_normal;

	//Functions
	Shape();
	Shape(glm::vec3 _position, glm::vec3 _colour, glm::vec3 _normal);
	//"Virtual" in order for method to be inherited
	virtual bool Intersection(float *_t, glm::vec3 _originOfRay, glm::vec3 _directionOfRay);
	virtual glm::vec3 NormalCalculation(glm::vec3 _p0, int *_shine, glm::vec3* _colourOfDiffuse, glm::vec3 *_colourOfSpecular);
};

#endif // _SHAPE_H_