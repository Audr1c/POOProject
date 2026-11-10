#pragma once

#include "../drawable.hpp"
#include "force.hpp"
#include <array>

class Particle;

class Spring : public Force, public Drawable {
private:
	std::array<Particle *, 2> ends; //only 2 particles can be connected by a spring
    
	float L0;                     //rest_length
	float k;                      //stiffness
	float rho;                    //drag coefficient


public:
	/*Spring(Particle &first, Particle &second, float k,
				 float rho = 0.0f);*/    //constructor with default rest length (distance between the two particles at the time of creation)



	Spring(Particle &first, Particle &second, float L0, float k,
				 float rho);

	

	Vec2 compute(const Particle &particle);
    void applyForces(); 
	void draw(Renderer &renderer);
};

