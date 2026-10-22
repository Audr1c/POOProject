#pragma once

#include <vector>
#include "particles.hpp" //JUST TO DEBUG : NEED TO BE CHANGED



class Integrator {

    public:
        virtual ~Integrator() = default;
        virtual void step(std::vector<Particle>& parts, float dt) = 0;
};



class Euler : public Integrator {

    public:
        void step(std::vector<Particle>& parts, float dt) override {
            for (Particle &p : parts){
                //Speed += acceleration * dt
                p.speed[0] += p.accel[0] * dt;
                p.speed[1] += p.accel[1] * dt;

                //Position += speed * dt
                p.pos[0] += p.speed[0] * dt;
                p.pos[1] += p.speed[1] * dt;
                
            }
            
        }
};





class Verlet : public Integrator {

    public:
        void step(std::vector<Particle>& parts, float dt) override {
            for (Particle &p : parts){
                
                std::vector<float> tmp_old_pos = p.pos;

                //x(n+1) = 2*x(n)     - x(n-1)            + a*dt*dt
                p.pos[0] = 2*p.pos[0] - p.previous_pos[0] + p.accel[0]*dt*dt;
                p.pos[1] = 2*p.pos[1] - p.previous_pos[1] + p.accel[1]*dt*dt;

                //Updating particle's speed attribute
                p.speed[0] = (p.pos[0] - tmp_old_pos[0]) / dt;
                p.speed[1] = (p.pos[1] - tmp_old_pos[1]) / dt;

                p.previous_pos = tmp_old_pos;
            }
            
        }
};
