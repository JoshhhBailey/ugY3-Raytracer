/// @file Plane.cpp
/// @brief Handles plane parameters, calculates plane intersection method and normal of hit position

#include <glm.hpp>

#include "Plane.h"

Plane::Plane()
{
	//Plane defaults
	m_normalOfPlane = glm::vec3(0, 0, 0);
}

Plane::Plane(glm::vec3 _position, glm::vec3 _normal, glm::vec3 _colour)
{
	//Create plane with specific parameters
	m_position = _position;
	m_normalOfPlane = _normal;
	m_colour = _colour;
}

bool Plane::Intersection(float *_t, glm::vec3 _originOfRay, glm::vec3 _directionOfRay)
{
	//Plane intersection method - https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-plane-and-ray-disk-intersection
	float denom = glm::dot(_directionOfRay, m_normalOfPlane);

	if (abs(denom) < 1e-6)
	{
		return false;
	}
	else
	{
		float result = glm::dot((m_position - _originOfRay), m_normalOfPlane) / denom;
		*_t = result;	//Pointer allows return of final result float value when using a bool method
		return *_t >= 0;
	}
}

glm::vec3 Plane::NormalCalculation(glm::vec3 _p0, int *_shine, glm::vec3* _colourOfDiffuse, glm::vec3 *_colourOfSpecular)
{
	//Matte like floor surface
	*_shine = 0;										//No glow
	*_colourOfDiffuse = glm::vec3(0.35f, 0.35f, 0.35f);	//Dull
	*_colourOfSpecular = m_colour;						//Colour of plane
	return m_normalOfPlane;
}