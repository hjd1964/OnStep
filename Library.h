// -----------------------------------------------------------------------------------
// Object Libraries

#include "Arduino.h"

const int rec_size = 16;
typedef struct libRecBase_t{
  char name[11]; // 11
  byte code;     // 1 (low 4 bits are object class, high are catalog #)
  uint16_t RA;   // 2
  uint16_t Dec;  // 2
};

typedef union libRec_t{
  libRecBase_t libRec;
  byte libRecBytes[rec_size];
};

class Library
{
  public:
    Library();
    ~Library();
    
    boolean setCatalog(int num);

    // 16 byte record
    libRec_t list;
    
    void writeVars(char* name, int code, double RA, double Dec);
    void readVars(char* name, int* code, double* RA, double* Dec);

    boolean firstRec();
    boolean firstFreeRec();
    boolean prevRec();
    boolean nextRec();
    boolean gotoRec(int num);

    void clearLib(); // clears this library
    void clearAll(); // clears all libraries

    int recCount(); // actual number of records for this library
    int recPos;     // currently selected record#
    int recMax;     // last record#
    
  private:
    libRec_t readRec(int address);
    void writeRec(int address, libRec_t data);
    void clearRec(int address);

    int catalog;

    int byteCount;
    int bytePos;
    int byteMin;
    int byteMax;
};
