#include "GameUtil.h"
#include "GameWorld.h"
#include "Bullet.h"
#include "Spaceship.h"
#include "BoundingSphere.h"
#include "CPU.h"

CPU::CPU()
    : GameObject("CPU")
{
}

CPU::~CPU(void)
{
}

void CPU::Play(GameWorld* world, shared_ptr<Spaceship> spaceship) {
    

    // Randomly shoot bullets
    int shootProbability = 5; // Adjust probability as needed
    if (rand() % 100 < shootProbability) {
        if (spaceship) {
            spaceship->Shoot();
        }
    }

    // Randomly rotate spaceship
    int rotateProbability = 10; // Adjust probability as needed
    if (rand() % 100 < rotateProbability) {
        int rotationDirection = (rand() % 2 == 0) ? 90 : -90; // Randomly choose rotation direction
        if (spaceship) {
            spaceship->Rotate(rotationDirection);
        }
    }

    // Randomly apply thrust
    int thrustProbability = 20; // Adjust probability as needed
    if (rand() % 100 < thrustProbability) {
        int thrustPower = rand() % 50; // Randomly choose thrust power
        if (spaceship) {
            spaceship->Thrust(thrustPower);
        }
    }

    void OnWorldUpdated(GameWorld * world);
    void OnObjectAdded(GameWorld * world, shared_ptr<GameObject> object);
    void OnObjectRemoved(GameWorld * world, shared_ptr<GameObject> object);

    // Add more decision-making logic as needed

    // Schedule next action
    //int actionInterval = 1000; // Time interval between actions in milliseconds (adjust as needed)
    //world->SetTimer(actionInterval, DEMO_MODE_ACTION);
}