/// @file Shape.cpp
/// @brief Base class all shapes inherit from

#include <glm.hpp>

#include "Shape.h"

Shape::Shape()
{
	//Shape defaults
	m_position = glm::vec3(0, 0, 0);
	m_colour = glm::vec3(0, 0, 0);
	m_normal = glm::vec3(0, 0, 0);
}

Shape::Shape(glm::vec3 _position, glm::vec3 _colour, glm::vec3 _normal)
{
	//Create shape with specific parameters
	m_position = _position;
	m_colour = _colour;
	m_normal = _normal;
}

bool Shape::Intersection(float *_t, glm::vec3 _originOfRay, glm::vec3 _directionOfRay)
{
	return false;
}

glm::vec3 Shape::NormalCalculation(glm::vec3 _p0, int *_shine, glm::vec3* _colourOfDiffuse, glm::vec3 *_colourOfSpecular)
{
	return m_normal;
}