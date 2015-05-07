
Library::Library()
{
  catalog=0;

  byteMin=(PECBufferSize-824)+1024;    // default=1024
  
  #if defined(__arm__) && defined(TEENSYDUINO)
  #define E2END 2047
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
  if (num<0) return false;   if (num>13) return false;

  catalog=num;
  return firstRec();
}

void Library::writeVars(char* name, int code, double RA, double Dec)
{
  libRec_t work;
  for (int l=0; l<7; l++) work.libRec.name[l] = name[l];
  work.libRec.code = (code | (catalog<<4));
  work.libRec.RA   = RA;
  work.libRec.Dec  = Dec;

  writeRec(recPos,work);
}

void Library::readVars(char* name, int* code, double* RA, double* Dec)
{
  libRec_t work;
  work=readRec(recPos);

  int cat = work.libRec.code>>4;

  // empty? or not found
  if ((cat==15) || (cat!=catalog)) { name[0]=0; *code=0; *RA=0.0; *Dec=0.0; return; }

  for (int l=0; l<7; l++) name[l]=work.libRec.name[l]; name[7]=0;
  
  *code = work.libRec.code & 15;
  *RA = work.libRec.RA;
  *Dec = work.libRec.Dec;
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
  int l=address*rec_size+byteMin;
  for (int m=0;m<16;m++) EEPROM.write(l+m,data.libRecBytes[m]);
}

void Library::clearRec(int address)
{
  int l=address*rec_size+byteMin;
  int code=15<<4;
  EEPROM.write(l+7,(byte)code); // catalog code 15 = deleted
}

boolean Library::firstRec()
{
  libRec_t work;

  int c=0;

  // see if first record is for the currentLib
  recPos=0;
  work=readRec(recPos);
  int cat=(int)work.libRec.code>>4;
  if (cat==catalog) return true;

  // otherwise find the first one, if it exists
  return nextRec();
}

// move to first unused record for this catalog
boolean Library::firstFreeRec()
{
  libRec_t work;

  int cat;
  int c=0;
  recPos=-1;
  
  do
  {
    recPos++; if (recPos>=recMax) { break; }
    work=readRec(recPos);

    cat=(int)work.libRec.code>>4;
  
    if (cat==15) break;
  } while (recPos<recMax);
  if (recPos>=recMax) { recPos=recMax-1; return false; }

  return true;
}

// read the previous record, if it exists
boolean Library::prevRec()
{
  libRec_t work;

  int cat;
  int c=0;
  
  do
  {
    recPos--; if (recPos<0) break;
    work=readRec(recPos);

    cat=(int)work.libRec.code>>4;
    if (cat==catalog) break;
  } while (recPos>=0);
  if (recPos<0) { recPos=0; return false; }

  return true;
}

// read the next record, if it exists
boolean Library::nextRec()
{
  libRec_t work;

  int cat;
  int c=0;
  
  do
  {
    recPos++; if (recPos>=recMax) break;
    work=readRec(recPos);

    cat=(int)work.libRec.code>>4;
    if (cat==catalog) break;
  } while (recPos<recMax);
  if (recPos>=recMax) { recPos=recMax-1; return false; }

  return true;
}

// read the specified record (of this catalog), if it exists
boolean Library::gotoRec(int num)
{
  libRec_t work;

  int cat;
  int l,r;
  int c=0;
  
  for (l=0;l<recMax;l++) {
    work=readRec(l); r=l;

    cat=(int)work.libRec.code>>4;
    if (cat==catalog) c++;
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
    if (cat==catalog) c++;
  }
  
  return c;
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
