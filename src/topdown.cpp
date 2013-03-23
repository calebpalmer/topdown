
#include <iostream>

#include "CapEngine.h"
#include "topdowngame.h"

using namespace std;
using namespace CapEngine;

int main(){
  cout << "topdown" << endl;
  try{
    TopDown::TopDownGame game;
    game.init();
    game.start();
    //  game.shutdown();
    return 1;
  }
  catch(const CapEngineException& e){
    Logger::getInstance().log(e.what(), Logger::CERROR);
  }

  catch(const exception& e){
    Logger::getInstance().log(e.what(), Logger::CERROR);
  }
}
