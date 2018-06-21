// -----------------------------------------------------------------------------------
// Constants

// some defines to help with configuration
#define TMC_LOWPWR      64
#define TMC_STEALTHCHOP 32
#define TMC_NINTPOL     16
#define SYNC_ANYWHERE_ON

// Stepper Driver Models
#define A4988   1
#define DRV8825 2
#define LV8729  3
#define RAPS128 3
#define TMC2100 4
#define TMC2208 5

#define TMC2130 100
#define TMC2130_QUIET 101  // these variations don't appear at run-time and are changed to "TMC2130"
#define TMC2130_VQUIET 102
#define TMC2130_QUIET_LOWPWR 103
#define TMC2130_VQUIET_LOWPWR 104

// Length of each stepper's table
#define LEN_A4988   5
#define LEN_DRV8825 6
#define LEN_LV8729  8
#define LEN_TMC2100 4
#define LEN_TMC2208 4
#define LEN_TMC2130 9
   
// The various microsteps for different driver models, with the bit modes for each
unsigned int StepsA4988  [LEN_A4988]  [2] = { {1,0}, {2,1}, {4,2}, {8,3}, {16,7} };
unsigned int StepsDRV8825[LEN_DRV8825][2] = { {1,0}, {2,1}, {4,2}, {8,3}, {16,4}, {32,5} };
unsigned int StepsLV8729 [LEN_LV8729] [2] = { {1,0}, {2,1}, {4,2}, {8,3}, {16,4}, {32,5}, {64,6}, {128,7} };
unsigned int StepsTMC2208[LEN_TMC2208][2] = {        {2,1}, {4,2}, {8,0}, {16,3} };
unsigned int StepsTMC2100[LEN_TMC2100][2] = { {1,0}, {2,1}, {4,2}, {16,3} };
unsigned int StepsTMC2130[LEN_TMC2130][2] = { {1,8}, {2,7}, {4,6}, {8,5}, {16,4}, {32,3}, {64,2}, {128,1}, {256,0} };

