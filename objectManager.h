#include "box2d/b2_body.h"
#include "box2d/b2_math.h"
#include <iostream>
#ifndef OBJECT_MANAGER_H
#define OBJECT_MANAGER .H

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include <vector>

#include "object.h"
#include "obstacle.h"
#include "rocket.h"
class ObjectManager {

private:
  std::vector<Object *> objects;
  sf::RenderWindow &window;

  b2Vec2 *gravity;
  b2World *world;

  const int SCALE = 30;
  const int GROUND_THICKNESS = 30;

  enum entityCategory {
    BOUNDARY = 0x0001,
    ROCKET = 0x0002,
  };

public:

  ObjectManager(sf::RenderWindow &win)
      : window(win), gravity(new b2Vec2(0.f, 9.8f)) {

    world = new b2World(*gravity);

    // Makes the boundaries
    
    objects.push_back(new Obstacle(GROUND_THICKNESS, window.getSize().y / 3.f,
                                   window.getSize().x / 3,
                                   window.getSize().y * (5.f / 6.f), world));
    objects.push_back(new Obstacle(window.getSize().x, GROUND_THICKNESS,
                                   window.getSize().x / 2, window.getSize().y,
                                   world));
    objects.push_back(new Obstacle(GROUND_THICKNESS, window.getSize().y, 0,
                                   window.getSize().y / 2, world));
    objects.push_back(new Obstacle(GROUND_THICKNESS, window.getSize().y,
                                   window.getSize().x, window.getSize().y / 2,
                                   world));
    objects.push_back(new Obstacle(window.getSize().x, GROUND_THICKNESS,
                                   window.getSize().x / 2, 0, world));
    addRocket();
  }

  // Delete all new'd objects in this class
  ~ObjectManager() {

    for (Object *ob : objects) {
      delete ob;
    }

    delete gravity;
    delete world;
  }

  void draw() {
    // Calls all objects draw function 
    for (Object *ob : objects) {
      ob->drawObject(window);
    }
  }

  // Performs an iteration in the physics engine
  void update() { world->Step(1 / 60.f, 8, 3); }


  // Makes rockets go up
  void manipRocket() {

    for (int i = 5; i < objects.size(); ++i) {

      objects[i]->applyUpwardForce();
    }
  }
  // Makes rockets turn to either side
  void pushRocket(bool left) {

    for (int i = 5; i < objects.size(); ++i) {

      objects[i]->applySideForce(left);
    }
  }

  // Creates a Body with a triangle geometry and certain properties to the scene
  void addRocket() {
    for (int i = 0; i < 20; ++i) {

      objects.push_back(
          new Rocket (25, 90, 40 * (i + 1), 400, world, ROCKET, BOUNDARY));
    }
  }
};

#endif
