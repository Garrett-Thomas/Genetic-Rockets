#ifndef DNA_H
#define DNA_H
#include "box2d/b2_body.h"
#include "box2d/b2_world.h"
#include <cstdlib>
#include <iostream>
#include <random>
class DNA {

private:
  const int NUM_VECTORS = 3;
  const int MAX_MAGNITUDE = 1000;
  const float sideVecDamping = MAX_MAGNITUDE * 0.03f;
  int dnaSize;
  double mass;

public:
  b2Vec2 **dna;
  DNA(int dS, double m) : dnaSize(dS), mass(m) {

    this->dna = new b2Vec2 *[NUM_VECTORS];

    for (int i = 0; i < NUM_VECTORS; ++i) {

      this->dna[i] = new b2Vec2[dnaSize];

      for (int j = 0; j < dnaSize; ++j) {

        if (i == 0) {

          this->dna[i][j] = randomVector(i);
        }

        else if (i == 1) {

          this->dna[i][j] = randomVector(i); 
        } 
        else {

          this->dna[i][j] = randomVector(i); 
        }
      }
    }
  } 

  // Type meaning where the vector will add force to on the rocket (up, left, right)
  b2Vec2 randomVector(int type){

    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_real_distribution<double> bottomVec(0, mass * sideVecDamping);

    std::uniform_real_distribution<double> sideVec(
        -1 * (MAX_MAGNITUDE) / sideVecDamping, MAX_MAGNITUDE / sideVecDamping);

    switch(type){
        case 0:
        return b2Vec2(-1 * abs(sideVec(generator)), 0);
        break;
        case 1:
        return b2Vec2(abs(sideVec(generator)), 0);
        break;
        case 2: 
        return b2Vec2(bottomVec(generator), bottomVec(generator));
        break;
        default:
        return b2Vec2(0, 0);
    }

  }

  DNA(int dS) {
    this->dna = new b2Vec2 *[NUM_VECTORS];
    for (int i = 0; i < NUM_VECTORS; ++i) {
      this->dna[i] = new b2Vec2[dS];
    }
  }
  // Cleanup
  ~DNA() {

    for (int i = 0; i < NUM_VECTORS; ++i) {
      delete[] dna[i];
    }
    delete[] dna;
  }
};

#endif
