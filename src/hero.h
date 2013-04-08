#ifndef HERO_H
#define HERO_H

#include <string>

#include "CapEngine.h"

#define HERO_WALKLEFT 1
#define HERO_WALKRIGHT 2

namespace TopDown{
  struct SpriteSheet {
    std::string filepath;
    int width;
    int height;
    int numSprites;  // is this needed?
    int rows;
    int cols;
  };

  enum HeroState{
    HERO_STILL,
    HERO_WALKING
  };

  class Hero{
    SpriteSheet spriteSheet;
    CapEngine::Rectangle spritePosition;
    CapEngine::Surface* surface;
    int animationSpeed;  // per second
    CapEngine::real lastFrameSwitch;
    int currentSpriteRow;
    int currentSpriteColumn; 
    HeroState currentState;

    Hero(const Hero&);
    Hero& operator=(const Hero&);
    
  public:
    Hero(const std::string& filePath);
    ~Hero();
    void initialize();
    CapEngine::Surface*  getSpriteSurface(int& x, int& y, int& w, int& h) const;
    CapEngine::Rectangle getBoundingRectangle() const;
    void update(CapEngine::real timestep);
    void setState(HeroState state);
    int getWidth();
    int getHeight();

    CapEngine::Vector direction;
    CapEngine::Vector position;

  };
}

#endif
