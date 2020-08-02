/// @file Sphere.cpp
/// @brief Handles sphere parameters, calculates sphere intersection method and normal of hit position

#include <iostream>
#include <glm.hpp>

#include "Sphere.h"

Sphere::Sphere()
{
	//Sphere defaults
	m_position = glm::vec3(0, 0, 0);
	m_colour = glm::vec3(0, 0, 0);
	m_radius = 0.0f;
}

Sphere::Sphere(glm::vec3 _position, float _radius, glm::vec3 _colour)
{
	//Create sphere with specific parameters
	m_position = _position;
	m_colour = _colour;
	m_radius = _radius;
}

bool Sphere::Intersection(float *_t, glm::vec3 _OriginOfRay, glm::vec3 _directionOfRay)
{
	//Sphere intersection method - https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection
	glm::vec3 L = m_position - _OriginOfRay;
	float tca = glm::dot(L, _directionOfRay);
	if (tca < 0)
	{
		return false;
	}
	else
	{
		float s2 = (glm::dot(L, L)) - (tca * tca);
		float s = glm::sqrt(s2);
		if (s > m_radius)
		{
			return false;
		}
		else
		{
			float thc = glm::sqrt((m_radius * m_radius) - s2);
			float t0 = tca - thc;
			*_t = t0;	//Pointer allows return of final result float value when using a bool method
			return true;
		}
	}
}

glm::vec3 Sphere::NormalCalculation(glm::vec3 _p0, int *_shine, glm::vec3* _colourOfDiffuse, glm::vec3 *_colourOfSpecular)
{
	*_shine = 128;											//Glow effect
	*_colourOfDiffuse = m_colour;							//Colour of sphere, shaded effect
	*_colourOfSpecular = glm::vec3(0.65f, 0.65f, 0.76f);	//Light grey
	//Normal calculation, hit position subtracted by position of the sphere
	return(_p0 - m_position);
}