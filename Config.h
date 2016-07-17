//
//
//    config.h
//
//


#define MACRO_SLIDER
//#define PAN_STEPPER

//#define MAIN_SLIDER



#ifdef MAIN_SLIDER

#define THIS_INTERFACE_ID 1



#define pin_STEP_A   10
#define pin_STEP_GND 9
#define pin_DIR_A    8
#define pin_DIR_GND 7
#define pin_ENABLE 6
#define pin_ENABLE_GND 5 

#define CAN_DISABLE_MOTOR true


#define HOME_SENSOR_GROUND 99
#define HOME_SENSOR_SIGNAL 23
#define HOME_SENSOR_POWER 99



#endif









#ifdef PAN_STEPPER

#define THIS_INTERFACE_ID 2



#define pin_STEP_A   4
#define pin_STEP_GND 5
#define pin_DIR_A    6
#define pin_DIR_GND 7
#define pin_ENABLE 8
#define pin_ENABLE_GND 9 

#define CAN_DISABLE_MOTOR true


#define HOME_SENSOR_GROUND 99
#define HOME_SENSOR_SIGNAL 23
#define HOME_SENSOR_POWER 99



#endif









#ifdef MACRO_SLIDER

#define THIS_INTERFACE_ID 6

#define INVERTED_PINS

#define pin_STEP_A   23
#define pin_DIR_A    22
#define pin_STEP_GND 7
#define pin_DIR_GND 8
#define pin_ENABLE 99
#define pin_ENABLE_GND 99 

#define CAN_DISABLE_MOTOR false


#define HOME_SENSOR_GROUND 99
#define HOME_SENSOR_SIGNAL 99
#define HOME_SENSOR_POWER 99



#endif




// for the big slider:
/*
#define pin_STEP_A   10
#define pin_DIR_A    8
#define pin_STEP_GND 9
#define pin_DIR_GND 7
*/


