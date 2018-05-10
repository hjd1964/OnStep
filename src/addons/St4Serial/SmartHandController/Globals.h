// -------------------------------------------------------------------------------
// Globals

bool Move[4] = { false,false,false,false };

char* BreakRC[4] = { ":Qn#" ,":Qs#" ,":Qe#" ,":Qw#" };
char* RC[4] = { ":Mn#" , ":Ms#" ,":Me#" ,":Mw#" };

enum Button {B_SHIFT,B_NORTH, B_SOUTH, B_EAST, B_WEST, B_F, B_f};
enum ButtonEvent { E_NONE, E_CLICK, E_DOUBLECLICK, E_LONGPRESSTART, E_LONGPRESS, E_LONGPRESSSTOP };
byte eventbuttons[7] = { E_NONE ,E_NONE ,E_NONE ,E_NONE ,E_NONE ,E_NONE ,E_NONE };

enum Menu { M_NONE, M_ALL, M_REF, M_GOTO, M_DISPlAY, M_SPIRAL, M_SPEED, M_MOT1, M_MOT2, M_DIVERSE };
enum Errors {ERR_NONE, ERR_MOTOR_FAULT, ERR_ALT, ERR_LIMIT_SENSE, ERR_DEC, ERR_AZM, ERR_UNDER_POLE, ERR_MERIDIAN, ERR_SYNC};
Errors lastError = ERR_NONE;

enum TrackState {TRK_OFF, TRK_ON, TRK_SLEWING, TRK_UNKNOW};
enum ParkState {PRK_UNPARKED, PRK_PARKED, PRK_FAILED, PRK_PARKING, PRK_UNKNOW};
enum PierState {PIER_E, PIER_W, PIER_UNKNOW};
enum Mount {GEM,FEM};
Mount mountType = GEM;

#define PierSideNone     0
#define PierSideEast     1
#define PierSideWest     2
#define PierSideBest     3
#define PierSideFlipWE1  10
#define PierSideFlipWE2  11
#define PierSideFlipWE3  12
#define PierSideFlipEW1  20
#define PierSideFlipEW2  21
#define PierSideFlipEW3  22
byte pierSide = PierSideNone;
int AlignMaxNumStars = -1;

byte button = 0;
unsigned long lastpageupdate = millis();
unsigned long time_last_action = millis();
uint8_t maxContrast = 255;
bool wifiOn = true;
bool powerCylceRequired = false;
bool sleepDisplay = false;
bool lowContrast = false;
bool buttonPressed = false;
byte page = 0;

// base timeouts
int CmdTimeout = TIMEOUT_CMD;  // default 30

char writeBuffer[40] = "";
int writeBufferPos = 0;

