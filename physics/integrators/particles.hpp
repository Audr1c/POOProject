/* DEBUG ONLY FOR TESTING WORLD AND INTEGRATORS*/

#pragma once

#include <iostream>
#include <vector>




class Particle{


    public:

        std::vector<float> pos;
        std::vector<float> previous_pos; //Used for Verlet integrator
        std::vector<float> speed;
        std::vector<float> accel;
        int id;

        Particle(std::vector<float> pos, std::vector<float> speed, std::vector<float> accel, int id): pos(pos), speed(speed), accel(accel), id(id) {
            previous_pos = pos;
        }
        Particle() {
            pos = {0,0};
            previous_pos = {0,0};
            speed = {0,0};
            accel = {0,0};
            id = 0;
        }

        void printState(){
            std::cout << "\n==== State of Particle " << id << " ====" << std::endl;
            std::cout << "Position: (" << pos[0] << ", " << pos[1] << ")" << std::endl;
            std::cout << "Previous Position: (" << previous_pos[0] << ", " << previous_pos[1] << ")" << std::endl;
            std::cout << "Speed: (" << speed[0] << ", " << speed[1] << ")" << std::endl;
            std::cout << "Accel: (" << accel[0] << ", " << accel[1] << ")" << std::endl;
        }

        void apply_force(){
            
        }

  
};
