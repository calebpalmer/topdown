#include <iostream>
#include <iostream>

#include "topdowngame.h"

using namespace CapEngine;
using namespace TopDown;
using namespace std;

TopDownGame::TopDownGame() : sp_map(nullptr)
			   , hero("res/hero.png"), speed(250.0) {}

void TopDownGame::init(){
  quit = false;
  
  // get Logger
  logger = &(Logger::getInstance());
  logger->currentLevel = LOGLEVEL;
  logger->log("logger established", Logger::CDEBUG);

  // set current directory
  setCurrentDir("../");
  logger->log("current directory is set to " + getCurrentDir(), Logger::CDEBUG);

  // start video
  vManager.reset(&(VideoManager::getInstance()));
  screenConfig.width = 640;
  screenConfig.height = 480;
  screenConfig.pDepth = 16;

  // setup events
  eventDispatcher.reset(new EventDispatcher());
  eventDispatcher->subscribe(this, systemEvent);
  eventDispatcher->subscribe(this, keyboardEvent);
  logger->log("Events subscribed", Logger::CDEBUG);
  
}

void TopDownGame::start(){
  vManager->initSystem(screenConfig);

  // initialize resources

  //initialize keyboard map
  keyboard.keyMap[Keyboard::CAP_KEYUP].state = Keyboard::CAP_UNPRESSED;
  keyboard.keyMap[Keyboard::CAP_KEYDOWN].state = Keyboard::CAP_UNPRESSED;
  keyboard.keyMap[Keyboard::CAP_KEYLEFT].state = Keyboard::CAP_UNPRESSED;
  keyboard.keyMap[Keyboard::CAP_KEYRIGHT].state = Keyboard::CAP_UNPRESSED;

  // load map and set position
  string mapPath = "res/map1.cem";
  sp_map.reset(new Map2D(mapPath));
  mapPosition.x = sp_map->width / 2;
  mapPosition.y = sp_map->height / 2;

  hero.initialize();

  mainLoop();
}

void TopDownGame::shutdown(){
  logger->log("Shutting Down", Logger::CWARNING);
  vManager->shutdown();
}

void TopDownGame::mainLoop(){
  Logger::getInstance().log("Starting main loop", Logger::CDEBUG);
  while(!quit){
    //handleevents
    eventDispatcher->getEvents();
    if(eventDispatcher->hasEvents()){
	eventDispatcher->flushQueue();
    }

    //update
    timeStep.updateStep();
    update();
    
    //render
    render();

    // refresh screen
    vManager->drawScreen();
    timeStep.renderStep();

  }
  shutdown();
}

// IEventSubscriber virtual function implementation
void TopDownGame::receiveEvent(const SDL_Event* event, CapEngine::Time* time){
  if(event->type == SDL_QUIT){
    Logger::getInstance().log("User chosen quit", Logger::CDEBUG);
    quit = true;
  }
  
  else if(event->type == SDL_KEYUP){
    SDLKey ksym = ((SDL_KeyboardEvent*)event)->keysym.sym;
    switch(ksym){
    case SDLK_UP:
      keyboard.keyMap[Keyboard::CAP_KEYUP].state = Keyboard::CAP_UNPRESSED;
      break;
    case SDLK_DOWN:
      keyboard.keyMap[Keyboard::CAP_KEYDOWN].state = Keyboard::CAP_UNPRESSED;      
      break;
    case SDLK_LEFT:
      keyboard.keyMap[Keyboard::CAP_KEYLEFT].state = Keyboard::CAP_UNPRESSED;
      break;
    case SDLK_RIGHT:
      keyboard.keyMap[Keyboard::CAP_KEYRIGHT].state = Keyboard::CAP_UNPRESSED;
      break;
    default:
      break;
    }
  }

  else if(event->type == SDL_KEYDOWN){
    SDLKey ksym = ((SDL_KeyboardEvent*)event)->keysym.sym;
    switch(ksym){
    case SDLK_UP:
      keyboard.keyMap[Keyboard::CAP_KEYUP].state = Keyboard::CAP_PRESSED;
      break;
    case SDLK_DOWN:
      keyboard.keyMap[Keyboard::CAP_KEYDOWN].state = Keyboard::CAP_PRESSED;      
      break;
    case SDLK_LEFT:
      keyboard.keyMap[Keyboard::CAP_KEYLEFT].state = Keyboard::CAP_PRESSED;
      break;
    case SDLK_RIGHT:
      keyboard.keyMap[Keyboard::CAP_KEYRIGHT].state = Keyboard::CAP_PRESSED;
      break;
    default:
      break;
    }
  }
  delete event;
}

Rect TopDownGame::calcMapDrawArea(){
    Rect rect;
    rect.x = mapPosition.x;
    rect.y = mapPosition.y;
    rect.w = screenConfig.width;
    rect.h = screenConfig.height;

    return rect;
}

void TopDownGame::updateMovement(){
  real increment = timeStep.lastTimeStep / 1000.0 * speed;
  if(keyboard.keyMap[Keyboard::CAP_KEYUP].state == Keyboard::CAP_PRESSED){
    mapPosition.y -= increment;
    hero.setState(HERO_WALKING);
  }
  if(keyboard.keyMap[Keyboard::CAP_KEYDOWN].state == Keyboard::CAP_PRESSED){
    mapPosition.y += increment;
    hero.setState(HERO_WALKING);
  }
  if(keyboard.keyMap[Keyboard::CAP_KEYLEFT].state == Keyboard::CAP_PRESSED){
    mapPosition.x -= increment;
    hero.direction.x = -1.0;
    hero.setState(HERO_WALKING);
  }
  if(keyboard.keyMap[Keyboard::CAP_KEYRIGHT].state == Keyboard::CAP_PRESSED){
    mapPosition.x += increment;
    hero.direction.x = 1.0;
    hero.setState(HERO_WALKING);
  }
  
  if(keyboard.keyMap[Keyboard::CAP_KEYRIGHT].state == Keyboard::CAP_UNPRESSED &&
     keyboard.keyMap[Keyboard::CAP_KEYLEFT].state == Keyboard::CAP_UNPRESSED &&
     keyboard.keyMap[Keyboard::CAP_KEYUP].state == Keyboard::CAP_UNPRESSED &&
     keyboard.keyMap[Keyboard::CAP_KEYDOWN].state == Keyboard::CAP_UNPRESSED){
    hero.setState(HERO_STILL);
  }

  // validate location
  if((sp_map->width - mapPosition.x) < screenConfig.width){
    mapPosition.x = mapPosition.x - (screenConfig.width - (sp_map->width - mapPosition.x));
  }
  if((sp_map->height - mapPosition.y) < screenConfig.height){
    mapPosition.y = mapPosition.y - (screenConfig.height - ( sp_map->height - mapPosition.y));
  }

}
void TopDownGame::render(){
  
  // render map
    Rect rect = calcMapDrawArea();
    vManager->drawSurface(0, 0, sp_map->surface, &rect);

    // render ultimate ninja hero
    real x;
    real y;
    real w;
    real h;
    Surface* heroSurface = hero.getSpriteSurface(x, y, w, h);
    rect.x = x;
    rect.y =  y;
    rect.w = w;
    rect.h = h;
    vManager->drawSurface(screenConfig.width/2.0,  screenConfig.height/2.0, heroSurface, &rect); 
    
}

void TopDownGame::update(){
  updateMovement();
  hero.update(timeStep.lastTimeStep);
}
