// -----------------------------------------------------------------------------------
// Constants

// Defines for simplified stepper driver mode setup

// Stepper Driver Models
#define A4988   1
#define DRV8825 2
#define LV8729  3
#define RAPS128 3
#define TMC2130 4
#define TMC2208 5

// Length of each stepper's table
#define LEN_A4988   5
#define LEN_DRV8825 6
#define LEN_LV8729  8
#define LEN_TMC2208 4
#define LEN_TMC2130 9
  
// The various microsteps for different driver models, with the bit modes for each
unsigned int StepsA4988  [LEN_A4988]  [2] = { {1,0}, {2,1}, {4,2}, {8,3}, {16,7} };
unsigned int StepsDRV8825[LEN_DRV8825][2] = { {1,0}, {2,1}, {4,2}, {8,3}, {16,4}, {32,5} };
unsigned int StepsLV8729 [LEN_LV8729] [2] = { {1,0}, {2,1}, {4,2}, {8,3}, {16,4}, {32,5}, {64,6}, {128,7} };
unsigned int StepsTMC2208[LEN_TMC2208][2] = {        {2,0}, {4,1}, {8,2}, {16,3} };
unsigned int StepsTMC2130[LEN_TMC2130][2] = { {1,8}, {2,7}, {4,6}, {8,5}, {16,4}, {32,3}, {64,2}, {128,1}, {256,0} };

