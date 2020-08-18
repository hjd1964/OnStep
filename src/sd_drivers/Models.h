// Driver models

// Macros, these variations are converted into "Models" (below) during compilation
#define TMC2130 100        // converted into TMC_SPI w/spreadCycle always enabled
#define TMC2130_QUIET 101  // converted into TMC_SPI w/stealthChop tracking and spreadCycle slews
#define TMC2130_VQUIET 102 // converted into TMC_SPI w/stealthChop always enabled

#define TMC5160 110        // converted into TMC_SPI w/spreadCycle always enabled
#define TMC5160_QUIET 111  // converted into TMC_SPI w/stealthChop tracking and spreadCycle slews
#define TMC5160_VQUIET 112 // converted into TMC_SPI w/stealthChop always enabled

#define TMC2209_QUIET 121  // converted into TMC2209 w/stealthChop tracking and spreadCycle slews
#define TMC2209_VQUIET 122 // converted into TMC2209 w/stealthChop always enabled

// Models
#define DRIVER_MODEL_FIRST 2
#define A4988    2  // step/dir stepper driver with EN LOW,  allows M0,M1,M2 bit patterens for 1x,2x,4x,8x,16x
#define DRV8825  3  // step/dir stepper driver with EN LOW,  allows M0,M1,M2 bit patterens for 1x,2x,4x,8x,16x,32x
#define S109     4  // step/dir stepper driver with EN LOW,  allows M0,M1,M2 bit patterens for 1x,2x,4x,8x,16x,32x
#define LV8729   5  // step/dir stepper driver with EN LOW,  allows M0,M1,M2 bit patterens for 1x,2x,4x,8x,16x,32x,64x,128x
#define RAPS128  6  // step/dir stepper driver with EN LOW,  allows M0,M1,M2 bit patterens for 1x,2x,4x,8x,16x,32x,64x,128x
#define TMC2100  7  // step/dir stepper driver with EN LOW,  allows M0,M1    bit patterens for 1x,2x,4x,16x   (spreadCycle only, no 256x intpol)
#define TMC2208  8  // step/dir stepper driver with EN LOW,  allows M0,M1    bit patterens for 2x,4x,8x,16x   (stealthChop default, uses 256x intpol)
#define TMC2209  9  // step/dir stepper driver with EN LOW,  allows M0,M1    bit patterens for 8x,16x,32x,64x (M2 sets spreadCycle/stealthChop, uses 256x intpol)
#define ST820    10 // step/dir stepper driver with EN LOW,  allows M0,M1,M2 bit patterens for 1x,2x,4x,8x,16x,32x,128x,256x
#define TMC_SPI  11 // step/dir stepper driver with EN LOW,  uses TMC protocol SPI comms   for 1x,2x...,256x  (SPI sets spreadCycle/stealthChop etc. for TMC2130 & TMC5160)
#define GENERIC  12 // alias for GENERIC1
#define GENERIC1 12 // step/dir stepper driver with EN LOW,  allows                        for 1x,2x,4x,8x,16x,32x,64x,128x,256x (no mode switching)
#define GENERIC2 13 // step/dir stepper driver with EN HIGH, otherwise as above
#define GENERIC3 14 // step/dir stepper driver with EN LOW,  inverted step signal (steps on LOW), otherwise as above
#define GENERIC4 15 // step/dir stepper driver with EN HIGH, inverted step signal (steps on LOW), otherwise as above
#define SERVO    16 // alias for SERVO1
#define SERVO1   16 // step/dir servo   driver with EN LOW,  allows M0 bit pattern for LOW = native mode & goto HIGH = 2x,4x,8x,16x,32x,64x, or 128x *larger* steps
#define SERVO2   17 // step/dir servo   driver with EN HIGH, otherwise as above
#define DRIVER_MODEL_LAST 17

// Minimum pulse width in nS
#define A4988_PULSE_WIDTH   1000
#define DRV8825_PULSE_WIDTH 2000
#define S109_PULSE_WIDTH    300
#define LV8729_PULSE_WIDTH  500
#define RAPS128_PULSE_WIDTH 5000 // 7000 is correct? appears to work with the Mega2560 at 5000?
#define TMC2100_PULSE_WIDTH 103
#define TMC2208_PULSE_WIDTH 103
#define TMC2209_PULSE_WIDTH 103
#define ST820_PULSE_WIDTH   20
#define TMC_SPI_PULSE_WIDTH 103
#define GENERIC_PULSE_WIDTH 5000  // enough for 100KHz stepping
#define SERVO_PULSE_WIDTH   1000  // enough for 500KHz stepping

// Wave forms
#define SQUARE 2
#define PULSE 3
#define DEDGE 4

// Decay Modes
#define OPEN 2
#define STEALTHCHOP 3
#define SPREADCYCLE 4

// Wait Times (milliseconds)
#define WAIT_DRIVER_ENABLE 5
#define WAIT_MODE_SWITCH 3

// ------------------------------------------------------------------------------------------------------------------------
// search function

// Length of each stepper's table
#define LEN_A4988   5
#define LEN_DRV8825 6
#define LEN_S109    6
#define LEN_LV8729  8
#define LEN_ST820   8
#define LEN_TMC2100 4
#define LEN_TMC2208 4
#define LEN_TMC2209 4
#define LEN_TMC_SPI 9
#define LEN_GENERIC 9
#define LEN_SERVO   8

// The various microsteps for different driver models, with the bit modes for each
unsigned int StepsA4988  [LEN_A4988]  [2] = { {1,0}, {2,1}, {4,2}, {8,3}, {16,7} };
unsigned int StepsDRV8825[LEN_DRV8825][2] = { {1,0}, {2,1}, {4,2}, {8,3}, {16,4}, {32,5} };
unsigned int StepsS109   [LEN_S109]   [2] = { {1,4}, {2,2}, {4,6}, {8,5}, {16,3}, {32,7} };
unsigned int StepsLV8729 [LEN_LV8729] [2] = { {1,0}, {2,1}, {4,2}, {8,3}, {16,4}, {32,5}, {64,6}, {128,7} };
unsigned int StepsST820  [LEN_ST820]  [2] = { {1,0}, {2,1}, {4,2}, {8,3}, {16,4}, {32,5},         {128,6}, {256,7} };
unsigned int StepsTMC2208[LEN_TMC2208][2] = {        {2,1}, {4,2}, {8,0}, {16,3} };
unsigned int StepsTMC2209[LEN_TMC2209][2] = {                      {8,0}, {16,3}, {32,1}, {64,2} };
unsigned int StepsTMC2100[LEN_TMC2100][2] = { {1,0}, {2,1}, {4,2},        {16,3} };
unsigned int StepsTMC_SPI[LEN_TMC_SPI][2] = { {1,8}, {2,7}, {4,6}, {8,5}, {16,4}, {32,3}, {64,2}, {128,1}, {256,0} };
unsigned int StepsGENERIC[LEN_GENERIC][2] = { {1,0}, {2,0}, {4,0}, {8,0}, {16,0}, {32,0}, {64,0}, {128,0}, {256,0} };
unsigned int StepsSERVO  [LEN_SERVO][2]   = { {1,0}, {2,1}, {4,1}, {8,1}, {16,1}, {32,1}, {64,1}, {128,1} };

unsigned int searchTable(unsigned int Table[][2], int TableLen, unsigned int Microsteps) {
  int i;
  
  for(i = 0; i < TableLen; i++) {
    if (Table[i][0] == Microsteps) {
      return Table[i][1];
    }
  }
  
  return 255;
}

// different models of stepper drivers have different bit settings for microsteps
// translate the human readable microsteps in the configuration to modebit settings 
unsigned int translateMicrosteps(int DriverModel, unsigned int Microsteps, bool err = false) {
  unsigned int Mode;
    
  // we search for each model, since they are different
  switch(DriverModel) {
    case A4988:
      Mode = searchTable(StepsA4988, LEN_A4988, Microsteps);
      break;
    case DRV8825:
      Mode = searchTable(StepsDRV8825, LEN_DRV8825, Microsteps);
      break;
    case S109:
      Mode = searchTable(StepsS109, LEN_S109, Microsteps);
      break;
    case LV8729: case RAPS128:
      Mode = searchTable(StepsLV8729, LEN_LV8729, Microsteps);
      break;
    case ST820:
      Mode = searchTable(StepsST820, LEN_ST820, Microsteps);
      break;
    case TMC2100:
      Mode = searchTable(StepsTMC2100, LEN_TMC2100, Microsteps);
      break;
    case TMC2208:
      Mode = searchTable(StepsTMC2208, LEN_TMC2208, Microsteps);
      break;
    case TMC2209:
      Mode = searchTable(StepsTMC2209, LEN_TMC2209, Microsteps);
      break;
    case TMC_SPI:
      Mode = searchTable(StepsTMC_SPI, LEN_TMC_SPI, Microsteps);
      break;
    case GENERIC:
      Mode = searchTable(StepsGENERIC, LEN_GENERIC, Microsteps);
      break;
    case SERVO:
      Mode = searchTable(StepsSERVO, LEN_SERVO, Microsteps);
      break;
    default:
      if (!err) Mode=1; else Mode=255;
  }
  if (!err && Mode == 255) Mode=0;
  return Mode;
}
