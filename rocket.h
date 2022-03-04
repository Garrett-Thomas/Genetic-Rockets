#ifndef ROCKET_H
#define ROCKET_H

#include "SFML/Graphics.hpp"
#include "box2d/b2_body.h"
#include "box2d/b2_world.h"
#include "dna.h"
#include "object.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
// Kinda like a wrapper for the box2d body object
class Rocket : public Object {
private:
  int vecLength;
  int lifecount = 0;
  int inputFields = 3;
  double fitness = 0;
  const int width = 25;
  const int height = 90;
  b2World *world;
  b2Body *rocketBody;
  const float MAGNITUDE = 15.f;
  sf::Texture texture;
  DNA *dna;

public:
  Rocket(int vL, int x, int y, b2World *wrld, uint16 categoryBits,
         uint16 maskBits)
      : vecLength(vL), world(wrld) {

    b2BodyDef BodyDef;
    BodyDef.position = b2Vec2(x / SCALE, y / SCALE);
    BodyDef.type = b2_dynamicBody;
    b2Body *Body = world->CreateBody(&BodyDef);

    // This defines the vertices for the triangle shape. The values aren't
    // intuitive
    b2PolygonShape Shape;
    b2Vec2 vertices[3];

    vertices[0].Set(-1 * width / 2.f / SCALE, height / SCALE / 2.f);
    vertices[1].Set(width / 2.f / SCALE, height / SCALE / 2.f);
    vertices[2].Set(width / 2.f / SCALE / 2.f, -1 * height / SCALE / 2.f);
    int32 count = 3;

    Shape.Set(vertices, count);

    // Sets the properties of the object. Note that I added categoryBits and
    // maskBits so that rockets can't collide with one another.
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

    dna = new DNA(vecLength, rocketBody->GetMass());
  }

  ~Rocket() { delete dna; }
  void setDna(DNA *na){
        delete dna;
        dna = na;
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

  b2Vec2 getWorldCenter() { return rocketBody->GetWorldCenter(); }
  void update() {

    if (lifecount < vecLength) {

      // Apply vectors to vectors
      rocketBody->ApplyForce(dna->dna[0][lifecount],
                             b2Vec2(rocketBody->GetPosition().x, 0), true);

      rocketBody->ApplyForce(dna->dna[1][lifecount],
                             b2Vec2(rocketBody->GetPosition().x, 0), true);

      float x = sin(rocketBody->GetAngle() - M_PI);
      float y = cos(rocketBody->GetAngle());

      rocketBody->ApplyForceToCenter(
          b2Vec2(-1 * roundf(x * dna->dna[2][lifecount].x),
                 -1 * roundf(y * dna->dna[2][lifecount].y)),
          true);

      // rocketBody->ApplyForceToCenter(dna->dna[2][lifecount], true);

      // std::cout << std::endl << rocketBody->GetWorldCenter().x;
      //    Calc fitness;
      ++lifecount;
    }
    lifecount = lifecount % vecLength;
  }
  void setFitness(double ft) { fitness = ft; }
  double getFitness() { return fitness; }
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

  DNA* breed(Rocket *parent) {

    DNA* childDna = new DNA(vecLength);

    for (int i = 0; i < inputFields; ++i) {

      int midpoint = rand() % vecLength;
      for (int j = 0; j < vecLength; ++j) {

        if (midpoint < i) {

          childDna->dna[i][j] = parent->dna->dna[i][j];
        } 
        else if ((float)rand() / RAND_MAX <= 0.10) {

          childDna->dna[i][j] = (parent->dna->dna[i][j].Skew());
        } 

        else {

          childDna->dna[i][j] = dna->dna[i][j];
        }
      }
    }

    return childDna;
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
