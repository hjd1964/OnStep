
Library::Library()
{
  catalog=0;

  byteMin=(PECBufferSize-824)+1024;    // default=1024
  
  #if defined(TEENSYDUINO) && !defined(E2END)
  #define E2END 2047
  #elif defined(__TM4C123GH6PM__) || defined(__LM4F120H5QR__)
  #define E2END 2047
  #elif defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__)
  #define E2END 6143
  #endif
  byteMax=E2END;                       // default=4095 (or 2047 on Teensy3.1)

  byteCount=(byteMax-byteMin)+1;
  bytePos=byteMin;
  
  recMax=byteCount/rec_size;           // maximum number of records

  firstRec();
}

Library::~Library()
{
}

boolean Library::setCatalog(int num)
{
  if ((num<0) || (num>14)) return false;

  catalog=num;
  return firstRec();
}

void Library::writeVars(char* name, int code, double RA, double Dec)
{
  libRec_t work;
  for (int l=0; l<11; l++) work.libRec.name[l] = name[l];
  work.libRec.code = (code | (catalog<<4));

  // convert into ulong, RA=0..360
  RA=degRange(RA)/360.0;
  // convert into ulong, Dec=0..180
  if (Dec>90.0) Dec=90.0; if (Dec<-90.0) Dec=-90.0; Dec=Dec+90.0; Dec=Dec/180.0;
  uint16_t r=round(RA*65536.0);
  uint16_t d=round(Dec*65536.0);
  
  work.libRec.RA   = r;
  work.libRec.Dec  = d;

  writeRec(recPos,work);
}

void Library::readVars(char* name, int* code, double* RA, double* Dec)
{
  libRec_t work;
  work=readRec(recPos);

  int cat = work.libRec.code>>4;

  // empty? or not found
  if ((cat==15) || (cat!=catalog)) { name[0]=0; *code=0; *RA=0.0; *Dec=0.0; return; }

  for (int l=0; l<11; l++) name[l]=work.libRec.name[l]; name[11]=0;
  
  *code = work.libRec.code & 15;
  uint16_t r = work.libRec.RA;
  uint16_t d = work.libRec.Dec;
  
  // convert from ulong
  *RA=(double)r;
  *RA=(*RA/65536.0)*360.0;
  *Dec=(double)d;
  *Dec=((*Dec/65536.0)*180.0)-90.0;
}

libRec_t Library::readRec(int address)
{
  libRec_t work;
  int l=address*rec_size+byteMin;
  for (int m=0;m<16;m++) work.libRecBytes[m]=EEPROM.read(l+m);
  return work;
}

void Library::writeRec(int address, libRec_t data)
{
  if ((address>=0) && (address<recMax)) {
    int l=address*rec_size+byteMin;
    for (int m=0;m<16;m++) EEPROM.write(l+m,data.libRecBytes[m]);
  }
}

void Library::clearRec(int address)
{
  if ((address>=0) && (address<recMax)) {
    int l=address*rec_size+byteMin;
    int code=15<<4;
    EEPROM.write(l+11,(byte)code); // catalog code 15 = deleted
  }
}

boolean Library::firstRec()
{
  libRec_t work;

  // see if first record is for the currentLib
  recPos=0;
  work=readRec(recPos);
  int cat=(int)work.libRec.code>>4;
  if ((work.libRec.name[0]!='$') && (cat==catalog)) return true;

  // otherwise find the first one, if it exists
  return nextRec();
}

// move to the catalog name rec
boolean Library::nameRec()
{
  libRec_t work;

  int cat;
  recPos=-1;
  
  do
  {
    recPos++; if (recPos>=recMax) { break; }
    work=readRec(recPos);

    cat=(int)work.libRec.code>>4;

    if ((work.libRec.name[0]=='$') && (cat==catalog)) break;
  } while (recPos<recMax);
  if (recPos>=recMax) { recPos=recMax-1; return false; }

  return true;
}

// move to first unused record for this catalog
boolean Library::firstFreeRec()
{
  libRec_t work;

  int cat;
  recPos=-1;
  
  do
  {
    recPos++; if (recPos>=recMax) { break; }
    work=readRec(recPos);

    cat=(int)work.libRec.code>>4;
  
    if (cat==15) break; // unused?
  } while (recPos<recMax);
  if (recPos>=recMax) { recPos=recMax-1; return false; }

  return true;
}

// read the previous record, if it exists
boolean Library::prevRec()
{
  libRec_t work;

  int cat;
  
  do
  {
    recPos--; if (recPos<0) break;
    work=readRec(recPos);

    cat=(int)work.libRec.code>>4;
    if ((work.libRec.name[0]!='$') && (cat==catalog)) break;
  } while (recPos>=0);
  if (recPos<0) { recPos=0; return false; }

  return true;
}

// read the next record, if it exists
boolean Library::nextRec()
{
  libRec_t work;

  int cat;
 
  do
  {
    recPos++; if (recPos>=recMax) break;
    work=readRec(recPos);

    cat=(int)work.libRec.code>>4;
    if ((work.libRec.name[0]!='$') && (cat==catalog)) break;
  } while (recPos<recMax);
  if (recPos>=recMax) { recPos=recMax-1; return false; }

  return true;
}

// read the specified record (of this catalog), if it exists
boolean Library::gotoRec(int num)
{
  libRec_t work;

  int cat;
  int l,r=0;
  int c=0;
  
  for (l=0;l<recMax;l++) {
    work=readRec(l); r=l;

    cat=(int)work.libRec.code>>4;
    if ((work.libRec.name[0]!='$') && (cat==catalog)) c++;
    if (c==num) break;
  }
  if (c==num) { recPos=r; return true; } else return false;
}

// count all catalog records
int Library::recCount()
{
  libRec_t work;

  int cat;
  int c=0;
  
  for (int l=0;l<recMax;l++) {
    work=readRec(l);

    cat=(int)work.libRec.code>>4;
    if ((work.libRec.name[0]!='$') && (cat==catalog)) c++;
  }
  
  return c;
}

// mark this catalog record as empty
void Library::clearCurrentRec()
{
  libRec_t work;

  int cat;

  work=readRec(recPos);

  cat=(int)work.libRec.code>>4;
  if (cat==catalog) clearRec(recPos);
}

// mark all catalog records as empty
void Library::clearLib()
{
  libRec_t work;

  int cat;

  for (int l=0;l<recMax;l++) {
    work=readRec(l);

    cat=(int)work.libRec.code>>4;
    if (cat==catalog) clearRec(l);
  }
}

// mark all records as empty
void Library::clearAll()
{
  for (int l=0;l<recMax;l++) clearRec(l);
}
