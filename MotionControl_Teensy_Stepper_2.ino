// --------------------------------------------
//
//    TEENSY_STEPPER controller
//    (c) h 2015
//
// -------------------------------------------
//
//   version 3: 26/10/2015
//      Conforming to same protocol as PID / servo firmware
//



#include "Config.h"
#include "ControlStepper.h"


// -------------------------------------------
//
//   Vars
//
// -------------------------------------------

bool flasher = false;

byte outgoingBuffer[64];
byte incomingBuffer[64];

union {
  float floatValue;
  uint8_t bytes[4];
} floatConvertBuffer;


// -------------------------------------------
//
//   STATES
//
// -------------------------------------------

#define STATE_MOTOR_OFF 0
#define STATE_MOTOR_ON_IDLE 1
#define STATE_MOTOR_ON_PLAYING_FRAME 2
#define STATE_MOTOR_OFF_OUT_OF_BOUNDS 3
#define STATE_MOTOR_OFF_RAN_OUT_OF_FRAMES 4
#define STATE_DOING_STICTION_CALIBRATION 5
#define BUFFER_STATE_NEED_NEXT_FRAME 1
#define BUFFER_STATE_OK 0

volatile uint8_t state;
volatile uint8_t buffer_state;
int32_t nextPosition;
//long manualPosition;


// -------------------------------------------
//
//   Stepper controller and interrupt setup
//
// -------------------------------------------

ControlLoopSTEPPER controller;
IntervalTimer innerTimer;

void stepperIntHandler() {
  controller.update();
}



// -------------------------------------------
//
//   SETUP
//
// -------------------------------------------

void setup() {
  Serial.begin(115200);
  pinMode(13, OUTPUT);
  pinMode(23, INPUT);
  digitalWrite(23, HIGH);

  controller.init();

  innerTimer.begin(stepperIntHandler, 16);
  innerTimer.priority(20);
  state = STATE_MOTOR_ON_IDLE;
}

// -------------------------------------------
//
//   LOOP
//
//   Only has to handle comms - motion is
//   handled in the interrupt-driven
//   controller update method
//
// -------------------------------------------




void loop() {
  checkForInput();
}


// -------------------------------------------
//
//   Communications
//
// -------------------------------------------


void checkForInput() {

  if (Serial.available() > 63) {
    for (int i = 0; i < 64; i++) {
      incomingBuffer[i] = Serial.read();
    }
    handleInput();
  }
}



void handleInput() {

  switch (incomingBuffer[0]) {
    case 105:
      // identify!
      sendIdentificationPacket();
      break;
    case 0:
      // manual control
      controller.disableMotor();
      state = STATE_MOTOR_OFF;
      break;
    case 1:
      controller.enableMotor();
      state = STATE_MOTOR_ON_IDLE;
      break;
    case 2:
      receiveFrame();
      break;
    case 16:
      doSendUpdate();
      break;
  }

}


void receiveFrame() {
  if (state == STATE_MOTOR_ON_IDLE) {
    state = STATE_MOTOR_ON_PLAYING_FRAME;

    controller.resetAndStartAnim();
  } else {

  }

  nextPosition = getLongFromIncomingBufferAtPosition(1);
  buffer_state = BUFFER_STATE_OK;
}




void sendIdentificationPacket() {
  outgoingBuffer[0] = 0;
  outgoingBuffer[1] = 1;
  outgoingBuffer[2] = 1;
  outgoingBuffer[3] = THIS_INTERFACE_ID;
  sendBuffer();
}

//floatConvertBuffer myConverter;


void doSendUpdate() {
  floatConvertBuffer.floatValue = controller.currentPosition;
  
  outgoingBuffer[0] = 1; // live data
  outgoingBuffer[1] = 1;
  outgoingBuffer[2] = (controller.homeSensorState) ? (state | 64) : state;
  outgoingBuffer[3] = buffer_state;
  
  outgoingBuffer[4] = floatConvertBuffer.bytes[0];
  outgoingBuffer[5] = floatConvertBuffer.bytes[1];
  outgoingBuffer[6] = floatConvertBuffer.bytes[2];
  outgoingBuffer[7] = floatConvertBuffer.bytes[3];
  
//  putLongInOutgoingBufferAtPosition(controller.currentPosition, 4);
  putLongInOutgoingBufferAtPosition(controller.homeSensorPosition, 44);

  sendBuffer();
}



void sendBuffer() {
  for (int i = 0; i < 64; i++) {
    Serial.write(outgoingBuffer[i]);
  }
}

int32_t getNextPosition() {
  if (state == STATE_MOTOR_ON_PLAYING_FRAME) {
    if (buffer_state == BUFFER_STATE_OK) {
      buffer_state = BUFFER_STATE_NEED_NEXT_FRAME;
      doSendUpdate();
    } else {
      state = STATE_MOTOR_OFF_RAN_OUT_OF_FRAMES;
      doSendUpdate();
      state = STATE_MOTOR_ON_IDLE;
    }
  }
  return nextPosition;
}

void putLongInOutgoingBufferAtPosition(long theValue, uint8_t bufferPosition) {
  outgoingBuffer[bufferPosition]     = theValue & 0xFF;
  outgoingBuffer[bufferPosition + 1] = (theValue >> 8)  & 0xFF;
  outgoingBuffer[bufferPosition + 2] = (theValue >> 16) & 0xFF;
  outgoingBuffer[bufferPosition + 3] = (theValue >> 24) & 0xFF;
}

int32_t getLongFromIncomingBufferAtPosition(uint8_t bufferPosition) {
  int32_t theVal = ((int32_t) incomingBuffer[bufferPosition] & 0xFF);
  theVal |= (((int32_t) incomingBuffer[bufferPosition + 1] & 0xFF)) << 8;
  theVal |= (((int32_t) incomingBuffer[bufferPosition + 2] & 0xFF)) << 16;
  theVal |= (((int32_t) incomingBuffer[bufferPosition + 3] & 0xFF)) << 24;
  return theVal;
}
