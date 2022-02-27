#ifndef OBJECT_H
#define OBJECT_H
#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>


// "Abstract" class that is in dear need of refactoring.
class Object {
protected:
  const float SCALE = 30.f;

public:
  Object() = default;
  virtual ~Object() = default;

  virtual void drawObject(sf::RenderWindow &window) = 0;
  virtual void applyUpwardForce(){};
  virtual void applySideForce(bool left){};
};

#endif
