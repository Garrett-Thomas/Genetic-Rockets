#include "object.h"
#include <iostream>
#include <stdexcept>
class Obstacle : public Object {

private:
  float width;
  float height;
  int xCord;
  int yCord;
  b2World *world;
  b2Body *obBody;

public:
  Obstacle(int w, int h, int x, int y, b2World *wrld)
      : width(w), height(h), xCord(x), yCord(y), world(wrld) {

          // Static Body
    b2BodyDef BodyDef;
    BodyDef.position.Set(xCord / SCALE, yCord / SCALE);
    BodyDef.type = b2_staticBody;
    b2Body *Body = world->CreateBody(&BodyDef);

    b2PolygonShape Shape;
    Shape.SetAsBox((width / 2) / SCALE, (height / 2) / SCALE);

    // 0 Density means it really has infinity density
    b2FixtureDef FixtureDef;
    FixtureDef.density = 0.f;
    FixtureDef.friction = 100.f;
    FixtureDef.shape = &Shape;
    Body->CreateFixture(&FixtureDef);

    this->obBody = Body;
  }

  void drawObject(sf::RenderWindow &window) {

    sf::RectangleShape Sprite(sf::Vector2f(width, height));
    Sprite.setFillColor(sf::Color::Black);
    Sprite.setOrigin(width / 2.f, height / 2.f);
    Sprite.setOutlineColor(sf::Color::Green);
    Sprite.setOutlineThickness(1.0f);

    Sprite.setPosition(obBody->GetPosition().x * SCALE,
                       obBody->GetPosition().y * SCALE);
    window.draw(Sprite);
  };
};
