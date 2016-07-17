#ifndef ___ControlLoopSTEPPER___
#define ___ControlLoopSTEPPER___


#include <Arduino.h>
#include <stdint.h>
#include "Config.h"


class ControlLoopSTEPPER {

    
  uint16_t currentStep;
 int16_t nextDelta_A;
 bool   currentDirection;



typedef int32_t bres_t;

 volatile bres_t A_theD;
 volatile bres_t A_twoDy;
 volatile bres_t A_twoDy_twoDx;

  
  
  
  
public:

             ControlLoopSTEPPER();
    int32_t homeSensorPosition;
    int32_t currentPosition;
    int32_t nextPosition;
    bool homeSensorState;


         
    void     init();
    void    enableMotor();
    void    disableMotor();
    void resetAndStartAnim();
     void     update();

  
    
 

  
  
} ;



#endif
