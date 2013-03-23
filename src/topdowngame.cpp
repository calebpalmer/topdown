#include <iostream>
#include <iostream>

#include "topdowngame.h"

using namespace CapEngine;
using namespace TopDown;
using namespace std;

TopDownGame::TopDownGame() : sp_map(nullptr) {}

void TopDownGame::init(){
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

  // load map and set position
  string mapPath = "res/map1.cem";
  sp_map.reset(new Map2D(mapPath));
  mapPosition.x = sp_map->width / 2;
  mapPosition.y = sp_map->height / 2;

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
    
    //render
    Rect rect = calcMapDrawArea();
    vManager->drawSurface(0, 0, sp_map->surface, &rect);
    
    // refresh screen
    vManager->drawScreen();
    timeStep.renderStep();

  }
  shutdown();
}

// IEventSubscriber virtual function implementation
void TopDownGame::receiveEvent(const SDL_Event* event, CapEngine::Time* time){
  if(event->type == SDL_QUIT){
    quit = true;
  }
  
  // handle screen movement
  if(event->type == SDL_KEYUP){
    SDLKey ksym = ((SDL_KeyboardEvent*)event)->keysym.sym;
    switch(ksym){
    case SDLK_UP:
      mapPosition.y--;
      break;
    case SDLK_DOWN:
      mapPosition.y++;
      break;
    case SDLK_LEFT:
      mapPosition.x--;
      break;
    case SDLK_RIGHT:
      mapPosition.x++;
      break;
    default:
      break;
    }
  }
  delete event;
}

Rect TopDownGame::calcMapDrawArea(){
    Rect rect;
    if((sp_map->width - mapPosition.x) < screenConfig.width){
      mapPosition.x = mapPosition.x - (screenConfig.width - (sp_map->width - mapPosition.x));
    }
    if((sp_map->height - mapPosition.y) < screenConfig.height){
      mapPosition.y = mapPosition.y - (screenConfig.height - ( sp_map->height - mapPosition.y));
    }
    rect.x = mapPosition.x;
    rect.y = mapPosition.y;
    rect.w = screenConfig.width;
    rect.h = screenConfig.height;

    return rect;
}
