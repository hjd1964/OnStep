// -----------------------------------------------------------------------------------
// Object Libraries

#include "Arduino.h"

#ifndef Library_h
#define Library_h

#pragma pack(1)
const int rec_size = 16;
typedef struct {
  char name[11]; // 11
  byte code;     // 1 (low 4 bits are object class, high are catalog #)
  uint16_t RA;   // 2
  uint16_t Dec;  // 2
} libRecBase_t;

typedef union {
  libRecBase_t libRec;
  byte libRecBytes[rec_size];
} libRec_t;
#pragma pack()

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
    boolean nameRec();
    boolean firstFreeRec();
    boolean prevRec();
    boolean nextRec();
    boolean gotoRec(int num);

    void clearCurrentRec(); // clears this record
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

Library Lib;
char const * objectStr[] = {"UNK", "OC", "GC", "PN", "DN", "SG", "EG", "IG", "KNT", "SNR", "GAL", "CN", "STR", "PLA", "CMT", "AST"};

#endif
