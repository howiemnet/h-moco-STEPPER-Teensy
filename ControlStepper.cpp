#include "ControlStepper.h"



//  Test points for monitoring with scope:

#define TP_INT_CLOCK 21
#define TP_GETNEXT 16

#define FREQ_INNER_INTERRUPT 62500
#define FREQ_OUTER_INTERRUPT 25
#define FREQ_FRAME 25
#define INNERS_PER_OUTER (FREQ_INNER_INTERRUPT / FREQ_OUTER_INTERRUPT)








extern int16_t getNextPosition();


ControlLoopSTEPPER::ControlLoopSTEPPER(){
}

void ControlLoopSTEPPER::init() {
    // set up output pins:  

#ifdef INVERTED_PINS

  pinMode(pin_STEP_A, OUTPUT);
  digitalWrite(pin_STEP_A,HIGH);
  pinMode(pin_DIR_A, OUTPUT);
  digitalWrite(pin_DIR_A,HIGH);
//  pinMode(pin_OPTOPOWER, OUTPUT);
//  digitalWrite(pin_OPTOPOWER,HIGH);
  

#else



  pinMode(pin_STEP_A, OUTPUT);
  pinMode(pin_DIR_A, OUTPUT);
  pinMode(pin_STEP_GND, OUTPUT);
  pinMode(pin_DIR_GND, OUTPUT);
  pinMode(pin_ENABLE, OUTPUT);
  pinMode(pin_ENABLE_GND, OUTPUT);
  
  
  digitalWrite(pin_STEP_GND, LOW);
  digitalWrite(pin_DIR_GND, LOW);
  digitalWrite(pin_ENABLE_GND, LOW);


#endif


  // set up test point pins:    

  pinMode(TP_INT_CLOCK,OUTPUT);
  pinMode(TP_GETNEXT,OUTPUT);
  pinMode(LED_BUILTIN,OUTPUT);



  currentStep = 1;

}



void ControlLoopSTEPPER::update() {

  // -------------------------------
  //
  //  for timing / testing:
  //
  // -------------------------------

  digitalWriteFast(TP_INT_CLOCK,HIGH);

#ifdef INVERTED_PINS

  digitalWriteFast(pin_STEP_A, HIGH);

#else

  digitalWriteFast(pin_STEP_A, LOW);

#endif

  // -------------------------------
  //
  // do the bresenham's:
  //
  // -------------------------------

  if (A_theD > 0) {

    // do a step!

#ifdef INVERTED_PINS

  digitalWriteFast(pin_STEP_A, LOW);

#else

  digitalWriteFast(pin_STEP_A, HIGH);

#endif

    
    
    A_theD += A_twoDy_twoDx;

    /* ---- Keep track of position ---- */
    
    if (currentDirection) {
      currentPosition ++;
    } else {
      currentPosition --;
    }

    /* ---- check for home sensor ---- */
    
    if (digitalReadFast(HOME_SENSOR_SIGNAL) != homeSensorState) {
      if (!homeSensorState) {

        // sensor going high so record position
      
        homeSensorState = true;
        homeSensorPosition = currentPosition;
      
      } else {
        homeSensorState = false;
      }
    }
    
  
  } else {
  
    A_theD += A_twoDy;
  
  }


  // -------------------------------
  //
  //  Prepare for next time round
  //
  // -------------------------------




   // are we at the end of this subframe?
  currentStep--;
  if (currentStep == 0) {

    // yes, so get next deltas and calc Bres coefficients

    // for testing:
    digitalWriteFast(TP_GETNEXT, HIGH);

    // channel A:
    nextPosition = getNextPosition();
    nextDelta_A = nextPosition - currentPosition;
    
    if (nextDelta_A < 0) {
      // direction positive
      digitalWrite(pin_DIR_A,HIGH);
      currentDirection = false;
      nextDelta_A = -nextDelta_A;
    } 
    else {
      // direction negative
      digitalWrite(pin_DIR_A,LOW);
      currentDirection = true;
    }
    A_twoDy = 2 * nextDelta_A;
    A_twoDy_twoDx = A_twoDy - (INNERS_PER_OUTER * 2);
    A_theD = A_twoDy_twoDx + INNERS_PER_OUTER;

    // tidy up:
    nextDelta_A = 0;
    currentStep = INNERS_PER_OUTER;

    digitalWriteFast(TP_GETNEXT, LOW);

  } 

  // for timing testing:
  digitalWriteFast(TP_INT_CLOCK,LOW);
  
}

void ControlLoopSTEPPER::resetAndStartAnim() {
  noInterrupts();
  currentStep = 1;
  A_twoDy = 0;
  A_theD = 0;
  A_twoDy_twoDx = 0;
  interrupts();
}

void ControlLoopSTEPPER::enableMotor() {
  digitalWrite(pin_ENABLE,LOW);
  
}

void ControlLoopSTEPPER::disableMotor() {
    digitalWrite(pin_ENABLE,HIGH);

}


