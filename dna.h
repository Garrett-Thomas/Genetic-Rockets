#ifndef DNA_H
#define DNA_H
#include "box2d/b2_body.h"
#include "box2d/b2_world.h"
#include <cstdlib>
#include <iostream>
#include <random>
class DNA {

private:
  const int numVectors = 3;
  const int maxMagnitude = 1000;
  const float sideVecDamping = maxMagnitude * 0.03f;
  int dnaSize;
  double mass;

public:
  b2Vec2 **dna;
  DNA(int dS, double m) : dnaSize(dS), mass(m) {

    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_real_distribution<double> bottomVec(0, mass * sideVecDamping);

    std::uniform_real_distribution<double> sideVec(
        -1 * (maxMagnitude) / sideVecDamping, maxMagnitude / sideVecDamping);

    this->dna = new b2Vec2 *[numVectors];

    for (int i = 0; i < numVectors; ++i) {

      this->dna[i] = new b2Vec2[dnaSize];

      for (int j = 0; j < dnaSize; ++j) {

        if (i == 0) {

          this->dna[i][j] = b2Vec2(-1 * abs(sideVec(generator)), 0);
        }

        else if (i == 1) {

          this->dna[i][j] = b2Vec2(abs(sideVec(generator)), 0);
        } else {

          this->dna[i][j] = b2Vec2(bottomVec(generator), bottomVec(generator));
        }
      }
    }
  }
    DNA(int ds) {

    this->dna = new b2Vec2 *[numVectors];
    for (int i = 0; i < numVectors; ++i) {
      this->dna[i] = new b2Vec2[dnaSize];
    }
  }
  // Cleanup
  ~DNA() {

    for (int i = 0; i < numVectors; ++i) {
      delete[] dna[i];
    }
    delete[] dna;
  }
};

#endif
