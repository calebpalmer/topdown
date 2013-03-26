#include "hero.h"

#include <stdexcept>

using namespace TopDown;
using namespace CapEngine;
using namespace std;

Hero::Hero(const string& filePath){
  spriteSheet.filepath = filePath;
  spriteSheet.width = 32;
  spriteSheet.height = 32;
  spriteSheet.numSprites = 2;
  spriteSheet.rows = 3;
  spriteSheet.cols = 2;

  //face the player right by default
  direction.x = 1.0;
  direction.y = 0.0;
  direction.z = 0.0;

  // update current sprite
  currentSpriteColumn = 0;
  currentSpriteRow = 0;

  animationSpeed = 6.0;
  currentState = HERO_STILL;
  
}

Hero::~Hero(){
  if(surface != nullptr){
    VideoManager& videoManager = VideoManager::getInstance();
    if(videoManager.initialized){
      videoManager.closeSurface(surface);
    }
  }
}

Rectangle Hero::getBoundingRectangle() const{
  Rectangle rect;
  rect.x = position.x;
  rect.y = position.y;
  rect.width = spriteSheet.width;
  rect.height = spriteSheet.height;

  return rect;
}
Surface* Hero::getSpriteSurface(real& x, real& y, real& w,real& h ) const{

  if(surface == nullptr){
    throw runtime_error("Hero has not been initialized");
  }

  x = currentSpriteColumn * spriteSheet.width;
  y = currentSpriteRow * spriteSheet.height;
  w = spriteSheet.width;
  h = spriteSheet.height;

  return surface;
}

void Hero::initialize(){
  VideoManager& videoManager = VideoManager::getInstance();
  if(!videoManager.initialized){
    throw CapEngineException("Unable to load surface.  Video not initialized");
  }

  // check existence of file path
  if(!fileExists(spriteSheet.filepath)){
    throw CapEngineException("Invalid path: " + spriteSheet.filepath);
  }
  
  surface = videoManager.loadImage(spriteSheet.filepath);

}

void Hero::update(real timestep){
  if(timestep < 0){
    return;
  }
  
  // update current frame row if moving
  if(currentState == HERO_WALKING){
    int walkFrames = 3;
    real elapsedTime = timestep + lastFrameSwitch;
    int frames = static_cast<int>((elapsedTime / 1000.0 * animationSpeed)) % walkFrames;
    lastFrameSwitch = elapsedTime;
    if(frames > 0){
      lastFrameSwitch = 0.0;
      currentSpriteRow = (currentSpriteRow + frames) % walkFrames;
    }

    // update frame column 
    if(direction.x == 1.0){
      // sprite 1 looking left
      currentSpriteColumn = 1;
    }
    else if(direction.x == -1.0){
      // sprite 2 looking left
      currentSpriteColumn = 0;
    }
    else{
      throw runtime_error("Hero has invalid direction");
    }
  }
}

void Hero::setState(HeroState state){
  currentState = state;
  if(currentState == HERO_WALKING  and state == HERO_STILL){
    lastFrameSwitch = 0.0;
  }
}
