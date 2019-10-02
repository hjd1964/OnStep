// Driver models

// Macros, these variations don't appear at run-time and are changed to "TMC_SPI"
#define TMC2130 100
#define TMC2130_QUIET 101
#define TMC2130_VQUIET 102

#define TMC5160 110
#define TMC5160_QUIET 111
#define TMC5160_VQUIET 112

// Macros, these variations don't appear at run-time and are changed to "TMC2209"
#define TMC2209_QUIET 121
#define TMC2209_VQUIET 122

// Models
#define DRIVER_MODEL_FIRST 1
#define A4988   1
#define DRV8825 2
#define S109    3
#define LV8729  4
#define RAPS128 5
#define TMC2100 6
#define TMC2208 7
#define TMC2209 8
#define ST820   9
#define TMC_SPI 10
#define DRIVER_MODEL_LAST 10

// Minimum pulse width
#define A4988_PULSE_WIDTH   1000
#define DRV8825_PULSE_WIDTH 2000
#define S109_PULSE_WIDTH    300
#define LV8729_PULSE_WIDTH  500
#define RAPS128_PULSE_WIDTH 5000 // 7000 is correct? appears to work with the Mega2560 at 5000?
#define TMC2100_PULSE_WIDTH 103
#define TMC2208_PULSE_WIDTH 103
#define TMC2209_PULSE_WIDTH 103
#define TMC_SPI_PULSE_WIDTH 103
#define ST820_PULSE_WIDTH   20

// Wave forms
#define SQUARE 1
#define PULSE 2
#define DEDGE 3

// Decay Modes
#define STEALTHCHOP 1
#define SPREADCYCLE 0
#define OPEN 2

// EN signal state defaults LOW
#define AXIS1_DRIVER_ENABLE LOW
#define AXIS2_DRIVER_ENABLE LOW
#define AXIS3_DRIVER_ENABLE LOW
#define AXIS4_DRIVER_ENABLE LOW
#define AXIS5_DRIVER_ENABLE LOW

// default mode switch state and sleep is disabled
#define MODE_SWITCH_BEFORE_SLEW OFF
#define MODE_SWITCH_SLEEP OFF

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

unsigned int searchTable(unsigned int Table[][2], int TableLen, unsigned int Microsteps) {
  int i;
  
  for(i = 0; i < TableLen; i++) {
    if (Table[i][0] == Microsteps) {
      return Table[i][1];
    }
  }
  
  return 0;
}

// different models of stepper drivers have different bit settings for microsteps
// translate the human readable microsteps in the configuration to modebit settings 
unsigned int translateMicrosteps(int axis, int DriverModel, unsigned int Microsteps) {
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
    default:
      Mode=1;
  }
  return Mode;
}
