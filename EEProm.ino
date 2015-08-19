// -----------------------------------------------------------------------------------
// EEPROM related functions 

// write int numbers into EEPROM at position i (2 bytes)
void EEPROM_writeInt(int i,int j) {
  uint8_t *k = (uint8_t*)&j;
  EEPROM.update(i+0,*k); k++;
  EEPROM.update(i+1,*k);
}

// read int numbers from EEPROM at position i (2 bytes)
int EEPROM_readInt(int i) {
  uint16_t j;
  uint8_t *k = (uint8_t*)&j;
  *k=EEPROM.read(i+0); k++;
  *k=EEPROM.read(i+1);
  return j;
}

// write 4 byte variable into EEPROM at position i (4 bytes)
void EEPROM_writeQuad(int i,byte *v) {
  EEPROM.update(i+0,*v); v++;
  EEPROM.update(i+1,*v); v++;
  EEPROM.update(i+2,*v); v++;
  EEPROM.update(i+3,*v);
}

// read 4 byte variable from EEPROM at position i (4 bytes)
void EEPROM_readQuad(int i,byte *v) {
  *v=EEPROM.read(i+0); v++;
  *v=EEPROM.read(i+1); v++;
  *v=EEPROM.read(i+2); v++;
  *v=EEPROM.read(i+3);  
}

// write String into EEPROM at position i (16 bytes)
void EEPROM_writeString(int i,char l[]) {
  for (int l1=0; l1<16; l1++) {
    EEPROM.update(i+l1,*l); l++;
  }
}

// read String from EEPROM at position i (16 bytes)
void EEPROM_readString(int i, char l[]) {
  for (int l1=0; l1<16; l1++) {
    *l=EEPROM.read(i+l1); l++;
  }
}
