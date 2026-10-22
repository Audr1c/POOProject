#pragma once

#include <vector>
#include <memory>

#include "integrators/particles.hpp" //JUST TO DEBUG : NEED TO BE CHANGED
//#include "integrators/force.hpp" //JUST TO DEBUG : NEED TO BE CHANGED
#include "integrators/integrators.hpp"

struct Borders {
    float x_min = 0.f, x_max = 10.f, y_min = 0.f, y_max = 10.f;
};

class World {

    private:
        std::vector<Particle>        particles;      //Vector containing all the particles of the world
        std::unique_ptr<Integrator>  integrator;     //Integrator algorithm which is used in the world
        Borders                      world_borders;  //Saved as {x_min, x_max, y_min, y_max}

    public:

        /*
         * Add one particle to the particles vector
         */
        void addPart(const Particle& new_part){
            particles.push_back(new_part);
        }

        /*
         * Define which integrator the world uses
         */
        void setIntegrator(std::unique_ptr<Integrator> new_integrator){
            integrator = std::move(new_integrator);
        }

        /*
         * Define the borders of the world
         */
        void setBorders(Borders b){
            world_borders = b;
        }




        /*
         * Update all the forces on each particle
         */
        void updateForces(float dt) {
            for (Particle &p : particles){
                p.apply_force();
            }
        }

        /*
         * Use the defined integrator to compute the new position
         */
        void updatePos(float dt) {
            if (integrator){
                integrator->step(particles, dt);
            }

            //check that every particle is inside the world borders, and if that's not the case move them inside
            for (Particle &p : particles){
                if (p.pos[0] < world_borders.x_min) {p.pos[0] = world_borders.x_min; p.speed[0] = 0;}
                if (p.pos[0] > world_borders.x_max) {p.pos[0] = world_borders.x_max; p.speed[0] = 0;}
                if (p.pos[1] < world_borders.y_min) {p.pos[1] = world_borders.y_min; p.speed[1] = 0;}
                if (p.pos[1] > world_borders.y_max) {p.pos[1] = world_borders.y_max; p.speed[1] = 0;}
            }
        }

        /*
         * Print the stat of each particle of the world
         */
        void printParticleState(){
            for (Particle &p : particles){
                p.printState();
            }
        }

        /*
         * Draw the new picture on the screen by calling all drawable objects
         */
        /*void draw(Renderer& renderer){

            //Draw forces
            for (Forces &f: forces) {
                if (auto* d = dynamic_cast<Drawable*>(f.get())) { //Check if it is a drawable force
                    d->draw(renderer);
                }
            }

            //Draw particles
            for (Particle &p : particles){
                p.draw(renderer); 
            }

        }*/


};
