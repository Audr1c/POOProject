#include <vector>
#include <memory>

#include "../physics/integrators/particles.hpp" //JUST TO DEBUG : NEED TO BE CHANGED
#include "../physics/integrators/integrators.hpp"
#include "../physics/world.hpp"




using namespace std;

int main() {

    cout << "Running world_test" << endl;


    //==== INITIALIZING THE WORLD ====//
    World world;

    world.setIntegrator(std::make_unique<Verlet>());

    Borders b{.x_min=0, .x_max = 20, .y_min = 0, .y_max=10};
    world.setBorders(b);

    for (int i=0; i<1; i++){
        Particle p;
        p.pos = {1,0};
        p.previous_pos = {1,0};
        p.speed = {0,0};
        p.accel = {-1,0};
        p.id = i;
        world.addPart(p);
    }

    //===============================//




    //==== RUNNING THE SIMULATION ====//
    world.printParticleState();
    for (int i=0; i<5; i++) {
        world.updatePos(0.1);
        world.printParticleState();
    }

    return 0;
}

