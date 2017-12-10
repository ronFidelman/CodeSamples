/**
 * \name core.cpp
 * \author Ron Fidelman
 * \date Monday, September 12, 2016
 * \brief The core engine that drives the game.
 *
 */
#include "core.h"
#include "Sandbox.h"
#include "Levels.h"
#include "Gameplay.h"
#include <chrono>

namespace Engine
{
  Core *CORE;
  
  /**
   * \brief Initializes the core.
   */
  Core::Core()
  {
    CORE = this;
    Running = true;
    PhysicsSystem = nullptr;
    Time = 0;
  }

 /**
  * \brief Start up every system in the system vector.
  */
  void Core::Startup()
  {
    for (unsigned i = 0; i < Systems.size(); ++i)
    {
      Systems[i]->Startup();
    }
    if (PhysicsSystem)
      PhysicsSystem->Startup();

    if(Sandbox::ACTIVE)
    {
      // Start up the sandbox.
      Sandbox::SessionStartUp();
    }
    else
    {
      // Start up the levels
      Levels::SessionStartUp();
    }

  }

  /**
   * \brief The main loop of the game engine.
   * \param capFrameRate A bool, true if framerate is capped.
   * \param FrameRate The framerate in frames.
   */
  void Core::MainLoop(bool capFrameRate, int FrameRate, int PhysicsRate)
  {
    // If the framerate is not capped, ensure it isn't zero.
    if (!capFrameRate) FrameRate = 1;
    const double MIN_FRAME_TIME = 1.0 / FrameRate;
    const std::chrono::duration<float> PHYSICS_UPDATE_TIME(1.0f / PhysicsRate);
    std::chrono::time_point<std::chrono::high_resolution_clock> frameStart,
                                                                framePrev;
    std::chrono::duration<float> dt = std::chrono::duration<float>::zero();
    std::chrono::duration<float> physicsTime = std::chrono::duration<float>::zero();
    
    // Inits the first previous frame container.
    framePrev = std::chrono::high_resolution_clock::now();

    while(Running)
    {
      // Get the start of the frame.
      frameStart = std::chrono::high_resolution_clock::now();

      // Calculate dt;
      dt = frameStart - framePrev;

      // If a set framerate is capped is wanted, we can busyloop here.
      while (capFrameRate && dt.count() < MIN_FRAME_TIME)
      {
        // Reset the frame start.
        frameStart = std::chrono::high_resolution_clock::now();

        // Calculate dt;
        dt = frameStart - framePrev;
      }
      physicsTime += dt;
      Time += dt.count();

      // Update the game
      Update(dt.count());

      // If physics's update time is an too big, cap it.
      if (physicsTime > PHYSICS_UPDATE_TIME * 5)
      {
        physicsTime = PHYSICS_UPDATE_TIME;
      }

        // Update physics
        while (PhysicsSystem && physicsTime > PHYSICS_UPDATE_TIME)
        {
          PhysicsSystem->Update(PHYSICS_UPDATE_TIME.count());
          physicsTime -= PHYSICS_UPDATE_TIME;
        }

      if(Sandbox::ACTIVE)
      {
        // Update the sandbox
        Sandbox::SessionUpdate(dt.count());
      }
      else
      {
        // Update the current level
        Levels::SessionUpdate(dt.count());
      }

      // Update framePrev
      framePrev = frameStart;
    }
  }

  /**
   * \brief Updates the systems vector
   * \param dt The current frametime.
   */
  void Core::Update(float dt)
  {
    for (unsigned i = 0; i < Systems.size(); ++i)
    {
      //if(!Systems[i]->GetPaused())
      Systems[i]->Update(dt);
    }
  }

  /**
   * \brief Deletes all the systems in reverse order from when they started
   */
  void Core::ShutDown()
  {
    // Shutdown the current level
    if(Sandbox::ACTIVE == false)
    {
      Levels::SessionShutDown();
    }

    // Shutdown the systems
    delete PhysicsSystem;
    for (unsigned i = 0; i < Systems.size(); ++i)
    {
      delete Systems[Systems.size() - i - 1];
    }
  }

  /**
   * \brief Adds a system to the system vector.
   * \param system The system being added to the back of Systems 
   */
  void Core::Add(System* system)
  {
    Systems.push_back(system);
  }

  /**
   * \brief Sets the physics system for the game.
   * \param physics The physics system to add to the game.
   */
  void Core::AddPhysics(Physics* physics)
  {
    PhysicsSystem = physics;
  }

  /**
   * \brief Flips the running bool.
   */
  void Core::EndGame()
  {
    Running = false;
  }

  long double Core::GetCurrentTime() const
  {
    return Time;
  }
}
