#include "box2d/b2_body.h"
#include "box2d/b2_math.h"
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#ifndef OBJECT_MANAGER_H
#define OBJECT_MANAGER .H

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include <vector>

#include "object.h"
#include "obstacle.h"
#include "rocket.h"

/*
 * Just thinking about how I should strucutre this.
 * I think Object Manager could manager the population and
 * be responsible for culling and stuff like that
 */
class ObjectManager {

private:
  std::vector<Object *> obstacles;
  std::vector<Rocket *> rockets;
  sf::RenderWindow &window;

  b2Vec2 *gravity;
  b2World *world;

  const int SCALE = 30;
  const int GROUND_THICKNESS = 30;
  const int ROCKET_LIFESPAN = 100;
  const int NUM_ROCKETS = 20;
  int lifecount = 0;
  enum entityCategory {
    BOUNDARY = 0x0001,
    ROCKET = 0x0002,
  };

public:
  ObjectManager(sf::RenderWindow &win)
      : window(win), gravity(new b2Vec2(0.f, 9.8f)) {

    world = new b2World(*gravity);

    // Makes the boundaries

    obstacles.push_back(new Obstacle(GROUND_THICKNESS, window.getSize().y / 3.f,
                                     window.getSize().x / 3,
                                     window.getSize().y * (5.f / 6.f), world));
    obstacles.push_back(new Obstacle(window.getSize().x, GROUND_THICKNESS,
                                     window.getSize().x / 2, window.getSize().y,
                                     world));
    obstacles.push_back(new Obstacle(GROUND_THICKNESS, window.getSize().y, 0,
                                     window.getSize().y / 2, world));
    obstacles.push_back(new Obstacle(GROUND_THICKNESS, window.getSize().y,
                                     window.getSize().x, window.getSize().y / 2,
                                     world));
    obstacles.push_back(new Obstacle(window.getSize().x, GROUND_THICKNESS,
                                     window.getSize().x / 2, 0, world));
    addRocket();
  }

  // Delete all new'd objects in this class
  ~ObjectManager() {

    for (Object *ob : obstacles) {
      delete ob;
    }

    for (Rocket *ob : rockets) {
      delete ob;
    }

    delete gravity;
    delete world;
  }

  void draw() {
    // Calls all objects draw function
    for (Object *ob : obstacles) {
      ob->drawObject(window);
    }

    for (Object *ob : rockets) {
      ob->drawObject(window);
    }
  }

  // Performs an iteration in the physics engine
  void update() {

    // Do simulation while rockets are alive
    if (lifecount < ROCKET_LIFESPAN) {

      for (int i = 0; i < rockets.size(); ++i) {
        rockets[i]->update();
      }

      world->Step(1 / 60.f, 8, 3);
      ++lifecount;
    }

    // Calc fitness, generate new population and reset simulation
    else {

      lifecount = 0;

      double highest = 0;
      for (int i = 0; i < rockets.size(); ++i) {

        int targetX = 0;
        int targetY = 0;

        double fitness =
            sqrt(pow(targetX - (rockets[i]->getWorldCenter().x * SCALE), 2) +
                 pow(targetY - (rockets[i]->getWorldCenter().y * SCALE), 2));

        fitness = floor(1 / (fitness / 100000));

        if (fitness > highest) {
          highest = fitness;
        }
        rockets[i]->setFitness(fitness);
      }

      for (int i = 0; i < rockets.size(); ++i) {

        rockets[i]->setFitness(
            floor(NUM_ROCKETS * (rockets[i]->getFitness() / highest)));
      }

      std::sort(rockets.begin(), rockets.end(), [](auto *lhs, auto *rhs) {
        return lhs->getFitness() > rhs->getFitness();
      });

      std::vector<Rocket *> breedingPool;
      for (int i = 0; i < rockets.size(); ++i) {

        for (int j = 0; j < rockets[i]->getFitness(); ++j) {
          breedingPool.push_back(rockets[i]);
        }

      }
      std::vector<Rocket *> population;

      for (int i = 0; i < rockets.size(); ++i) {

        DNA *newDna = rockets[rand() % rockets.size()]->breed(
            rockets[rand() % rockets.size()]);

        Rocket *temp = new Rocket(ROCKET_LIFESPAN, 40,
                                  window.getSize().y - GROUND_THICKNESS, world,
                                  ROCKET, BOUNDARY);
        temp->setDna(newDna);
        population.push_back(temp);
      }
    
      for(int i = 0; i < rockets.size(); ++i){
          delete rockets.at(i);
      }
      rockets = population;
      // need to get two random rockets from the vector.
    }
  }

  // Makes rockets go up
  void manipRocket() {

    for (int i = 0; i < rockets.size(); ++i) {

      rockets[i]->applyUpwardForce();
    }
  }

  // Makes rockets turn to either side
  void pushRocket(bool left) {

    for (int i = 0; i < rockets.size(); ++i) {

      rockets[i]->applySideForce(left);
    }
  }

  // Creates a Body with a triangle geometry and certain properties to the scene
  void addRocket() {
    for (int i = 0; i < NUM_ROCKETS; ++i) {

      rockets.push_back(new Rocket(ROCKET_LIFESPAN, 40,
                                   window.getSize().y - GROUND_THICKNESS, world,
                                   ROCKET, BOUNDARY));
    }
  }
};

#endif
