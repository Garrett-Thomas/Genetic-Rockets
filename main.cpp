#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window/Event.hpp"
#include "SFML/Window/Keyboard.hpp"
#include "objectManager.h"
#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

/*
 * Wasn't able to get the body to update. Think it was because I kept using
 * references and setting temp world objects. Wasn't being conscious about
 * scope. Changed to pointers and now the world finally updates
 */

const int WINDOW_WIDTH = 1600;
const int WINDOW_HEIGHT = 1280;
const int MAX_SPEED_MULT = 16;
// Want this to be the top level component of the app. Don't want tons of logic
// in here.
int main() {
  bool isSpace = false;
  bool isLeft = false;
  bool isRight = false;
  bool isSpeed = false;

  int speedMult = 1;
  sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
                          "Rockets");
  window.setFramerateLimit(60);

  ObjectManager *manager = new ObjectManager(window);

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }

      // Only here for testing. Makes the inputs smoooth
      switch (event.type) {
      case sf::Event::KeyPressed:
        if (event.key.code == sf::Keyboard::Space) {
          isSpace = true;
          break;
        }

        if (event.key.code == sf::Keyboard::Right) {
          isRight = true;
          break;
        }

        if (event.key.code == sf::Keyboard::Left) {
          isLeft = true;
          break;
        }

        if (event.key.code == sf::Keyboard::S) {
          isSpeed = speedMult - MAX_SPEED_MULT == 0 ? false : true;
          speedMult =
              ((speedMult < 1 ? 1 : speedMult) * 2) % (MAX_SPEED_MULT * 2);
          std::cout << speedMult << std::endl;
          break;
        }
      case sf::Event::KeyReleased:

        if (event.key.code == sf::Keyboard::Space) {
          isSpace = false;
          break;
        }
        if (event.key.code == sf::Keyboard::Right) {
          isRight = false;
          break;
        }

        if (event.key.code == sf::Keyboard::Left) {
          isLeft = false;
          break;
        }
      }
    }

    if (isSpace) {
      manager->manipRocket();
    }
    if (isRight) {
      manager->pushRocket(!isRight);
    }
    if (isLeft) {
      manager->pushRocket(isLeft);
    }
    if (isSpeed) {
      manager->speedUp(speedMult);
    }

    window.clear(sf::Color(180, 180, 180));
    manager->update();
    manager->draw();
    window.display();
  }
  delete manager;
  return 0;
}
