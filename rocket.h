#ifndef ROCKET_H
#define ROCKET_H

#include "SFML/Graphics.hpp"
#include "box2d/b2_body.h"
#include "box2d/b2_world.h"
#include "object.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
// Kinda like a wrapper for the box2d body object
class Rocket : public Object {

private:
  float width;
  float height;
  b2World *world;
  b2Body *rocketBody;
  const float MAGNITUDE = 15.f;
  sf::Texture texture;

public:
  Rocket(int w, int h, int x, int y, b2World *wrld, uint16 categoryBits,
         uint16 maskBits)
      : width(w), height(h), world(wrld) {

    b2BodyDef BodyDef;
    BodyDef.position = b2Vec2(x / SCALE, y / SCALE);
    BodyDef.type = b2_dynamicBody;
    b2Body *Body = world->CreateBody(&BodyDef);

    // This defines the vertices for the triangle shape. The values aren't intuitive
    b2PolygonShape Shape;
    b2Vec2 vertices[3];

    vertices[0].Set(-1 * width / 2.f / SCALE, height / SCALE / 2.f);
    vertices[1].Set(width / 2.f / SCALE, height / SCALE / 2.f);
    vertices[2].Set(width / 2.f / SCALE / 2.f, -1 * height / SCALE / 2.f);
    int32 count = 3;

    Shape.Set(vertices, count);

    // Sets the properties of the object. Note that I added categoryBits and maskBits so that
    // rockets can't collide with one another.
    b2FixtureDef FixtureDef;
    FixtureDef.density = 100.f;
    FixtureDef.friction = 0.f;
    FixtureDef.filter.categoryBits = categoryBits;
    FixtureDef.filter.maskBits = maskBits;
    FixtureDef.shape = &Shape;
    Body->CreateFixture(&FixtureDef);

    this->rocketBody = Body;

    // grabs the rocket image and makes it into a texture.
    // The second paramater and grabs a 60 x 200 rec starting at (70, 0)
    this->texture.loadFromFile("code/rocket.png", sf::IntRect(70, 0, 60, 200));
  }


  void drawObject(sf::RenderWindow &window) {

      // Still drawing as a rectangle, just simpler
    sf::RectangleShape Sprite(sf::Vector2f(width, height));
    Sprite.setTexture(&texture);

    // Sets the origin to the center of the sprite.
    // Everything is drawn relative to the center
    Sprite.setOrigin(width / 2.f, height / 2.f);

    // The position has to be multiplied by the scale.
    Sprite.setPosition(rocketBody->GetPosition().x * SCALE,
                       rocketBody->GetPosition().y * SCALE);

    // Set the angle
    Sprite.setRotation(rocketBody->GetAngle() * 180 / b2_pi);

    window.draw(Sprite);
  }

  void applySideForce(bool left) {

    int direction = left ? -1 : 1;

    // Fairly certain this pushes the rocket from the nose
    // but this is my first time using vectors so idk.
    b2Vec2 *point = new b2Vec2(rocketBody->GetPosition().x, 0);

    // Apply force with a magnitude proportional to the mass
    rocketBody->ApplyForce(
        b2Vec2(direction * rocketBody->GetMass() * (2.f / MAGNITUDE), 0),
        *point, true);

    // Your welcome valgrind
    delete point;
  }

  void applyUpwardForce() {
      
      // Trig to figure out how much force needs to be added for the
      // physics to be realistic.
      // Not sure why the -M_PI is needed. I suppose you can always multiply by -1
    float x = sin(rocketBody->GetAngle() - M_PI);
    float y = cos(rocketBody->GetAngle());

    rocketBody->ApplyForceToCenter(
        b2Vec2(-1 * roundf(x * rocketBody->GetMass() * MAGNITUDE),
               -1 * roundf(y * rocketBody->GetMass() * MAGNITUDE)),
        true);
  }
};

#endif
