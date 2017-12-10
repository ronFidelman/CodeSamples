/**
 * \name core.h
 * \author Ron Fidelman
 * \date Tuesday, September 13, 2016
 * \brief The decleration of the engine core, the driving singleton for the 
 * system.
 *
 */
#pragma once
#include <vector>
#include "system.h"
#include "Physics.h"


namespace Engine
{
  class Core
  {
  public:
    Core();
    ~Core(){}

    // Starts up the sytems in System.
    void Startup();

    // The update loop for the game. Handles timestepping.
    void MainLoop(bool capFrameRate, int FrameRate, int PhysicsRate = 60);

    // Calls updates on systems in Systems
    void Update(float dt);

    // Shuts down and destroys every system in the system vector, in reverse 
    // order of how it starts.
    void ShutDown();

    // Adds a new system, to the system vector.
    void Add(System * system);
    // Adds the physics system.
    void AddPhysics(Physics *physics);

    // Flips the running bool.
    void EndGame();

    long double GetCurrentTime() const;

  private:
    // A vector that stores pointers to the systems in the game.
    std::vector<System *> Systems;
    Physics *PhysicsSystem;

    // If true, the engine continues running.
    bool Running;
    
    // Total time the game has been running.
    long double Time = 0;
  };

  // A global pointer to core;
  extern Core *CORE;
}
