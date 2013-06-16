#include <iostream>
#include <iostream>

#include "topdowngame.h"

using namespace CapEngine;
using namespace TopDown;
using namespace std;

TopDownGame::TopDownGame() : sp_map(nullptr)
			   , hero("res/hero.png"), speed(200.0) {}

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
  vManager= &VideoManager::getInstance();
  screenConfig.width = 640;
  screenConfig.height = 480;
  screenConfig.pDepth = 16;
  screenConfig.opengl = false;

  // setup events
  eventDispatcher = &EventDispatcher::getInstance();
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
  hero.position.x = sp_map->width / 2.0;
  hero.position.y = sp_map->height / 2.0;
  
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
    // update the Keyboard structure
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
    // update the Keyboard structure
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
  Vector translation;
  
  // next four check if hero is moving
  if(keyboard.keyMap[Keyboard::CAP_KEYUP].state == Keyboard::CAP_PRESSED){
    mapPosition.y -= increment;
    
    translation.y = (-1) * increment;
    hero.position = hero.position + translation;
    hero.setState(HERO_WALKING);
  }

  if(keyboard.keyMap[Keyboard::CAP_KEYDOWN].state == Keyboard::CAP_PRESSED){
    mapPosition.y += increment;

    translation.y = increment;
    hero.position = hero.position + translation; 
    hero.setState(HERO_WALKING);
  }

  if(keyboard.keyMap[Keyboard::CAP_KEYLEFT].state == Keyboard::CAP_PRESSED){
    mapPosition.x -= increment;

    translation.x = (-1) * increment;
    hero.position = hero.position + translation;
    hero.direction.x = -1.0;
    hero.setState(HERO_WALKING);
  }

  if(keyboard.keyMap[Keyboard::CAP_KEYRIGHT].state == Keyboard::CAP_PRESSED){
    mapPosition.x += increment;

    translation.x = increment;
    hero.position = hero.position + translation;
    hero.direction.x = 1.0;
    hero.setState(HERO_WALKING);
  }
  
  // Is hero still?
  if(keyboard.keyMap[Keyboard::CAP_KEYRIGHT].state == Keyboard::CAP_UNPRESSED &&
     keyboard.keyMap[Keyboard::CAP_KEYLEFT].state == Keyboard::CAP_UNPRESSED &&
     keyboard.keyMap[Keyboard::CAP_KEYUP].state == Keyboard::CAP_UNPRESSED &&
     keyboard.keyMap[Keyboard::CAP_KEYDOWN].state == Keyboard::CAP_UNPRESSED){
    hero.setState(HERO_STILL);
  }

  // validate hero location
  if(hero.position.x < 0.0){
    hero.position.x = 0.0;
  }
  if(hero.position.x > sp_map->width){
    hero.position.x = sp_map->width;
  }
  if(hero.position.y > sp_map->height){
    hero.position.y = sp_map->height;
  }
  if(hero.position.y < 0.0){
    hero.position.y = 0.0;
  }


}
void TopDownGame::render(){
  // validate map draw position
  Vector heroTopLeft;
  heroTopLeft.x = hero.position.x - (hero.getWidth() / 2);
  heroTopLeft.y = hero.position.y - (hero.getHeight() / 2);

  // hero is not in center of map
  if(heroTopLeft.x - (screenConfig.width / 2) < 0 ||
     sp_map->width - (heroTopLeft.x + hero.getWidth()) < (screenConfig.width / 2) ||
     heroTopLeft.y - (screenConfig.height / 2) < 0 ||
     sp_map->height - (heroTopLeft.y + hero.getHeight()) < (screenConfig.height /2)){

    Rect mapRect;
    Rect heroRect;
    // Draw map
    // // Is map drawn to the left or the right or neither
    // map all the way to the left
    if(heroTopLeft.x - (screenConfig.width / 2) < 0){
      mapRect.x = 0.0;
      heroRect.x = hero.position.x - (hero.getWidth() / 2);
    }
    // map all the way to the right
    else if(sp_map->width - (heroTopLeft.x + hero.getWidth()) < (screenConfig.width / 2)){
      mapRect.x = sp_map->width - screenConfig.width;
      heroRect.x = screenConfig.width - (sp_map->width - hero.position.x) - (hero.getWidth() / 2); 
    }
    else{
      mapRect.x = hero.position.x - (screenConfig.width / 2);
      //heroRect.x = hero.position.x - (hero.getWidth() / 2);
      heroRect.x = (screenConfig.width / 2) - (hero.getWidth() / 2);
    }

    // map all the way to the top
    if(heroTopLeft.y - (screenConfig.height / 2) < 0){
      mapRect.y = 0.0;
      heroRect.y = hero.position.y - (hero.getHeight() / 2);
    }
    //map all the way to the bottom
    else if(sp_map->height - (heroTopLeft.y + hero.getHeight()) < (screenConfig.height /2)){
      mapRect.y = sp_map->height - screenConfig.height;
      heroRect.y = screenConfig.height - (sp_map->height - hero.position.y) - (hero.getHeight() / 2);
    }
    else{
      mapRect.y = (hero.position.y) - (hero.getHeight() / 2) - (screenConfig.height / 2);
      //      heroRect.y = hero.position.y - (hero.getHeight() / 2);
      heroRect.y = (screenConfig.height / 2) - (hero.getHeight() / 2);
    }

    mapRect.w = screenConfig.width;
    mapRect.h = screenConfig.height;
    heroRect.w = hero.getWidth();
    heroRect.h = hero.getHeight();

    vManager->drawSurface(0, 0, sp_map->surface, &mapRect);

    Rect heroSpriteRect;
    int x, y, w, h;
    Surface* heroSurface =  hero.getSpriteSurface(x, y, w, h);
    heroSpriteRect.x = x;
    heroSpriteRect.y = y;
    heroSpriteRect.w = w;
    heroSpriteRect.h = h;
    vManager->drawSurface(heroRect.x, heroRect.y, heroSurface, &heroSpriteRect); 
    
  }
  // hero is in center of map
  else{
    // render map
    //Rect rect = calcMapDrawArea();  // Map is drawn relative to player
    Rect rect;
    rect.x = hero.position.x - (screenConfig.width / 2);
    rect.y = hero.position.y - (screenConfig.height / 2);
    rect.w = screenConfig.width;
    rect.y = screenConfig.height;
    vManager->drawSurface(0, 0, sp_map->surface, &rect);
    
    // render ultimate ninja hero in center of screen
    int x, y, w, h;
    Surface* heroSurface = hero.getSpriteSurface(x, y, w, h);
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    vManager->drawSurface(screenConfig.width/2.0,  screenConfig.height/2.0, heroSurface, &rect); 
  }
}
 

void TopDownGame::update(){
  updateMovement();
  hero.update(timeStep.lastTimeStep);
}
