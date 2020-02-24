// -----------------------------------------------------------------------------------
// Control a single dew Heater

#pragma once

class dewHeaterControl {
  public:
    void init(int pin) {
      _pin=pin;
      pinMode(_pin,OUTPUT);
    }
    void poll(int deltaFromDewPointC) {
      #if MIRROR_TEMPERATURE == DS1820
	    if (deltaFromDewPointC < DeltaC) {
		    digitalWrite(_pin,HIGH);
		    heaterOn=true;
	    }
	    else {
		    digitalWrite(_pin,LOW);
		    heaterOn=false;
	    }
      #else	    
      int switchTimeMs=0;
      deltaFromDewPointC=constrain(deltaFromDewPointC,lowDeltaC,highDeltaC);
      switchTimeMs=map(deltaFromDewPointC,lowDeltaC,highDeltaC,1000,0);

      currentTime = millis();
      if (!heaterOn && (long)(currentTime - (lastHeaterCycle + switchTimeMs)) <= 0)
      {
        digitalWrite(_pin,HIGH);
        heaterOn=true;
      }
      else if (heaterOn && (long)(currentTime - (lastHeaterCycle + switchTimeMs)) > 0) 
      {
        digitalWrite(_pin,LOW);
        heaterOn=false;
      }
      else if ((long)(currentTime - (lastHeaterCycle + 1000)) > 0) {
        lastHeaterCycle=currentTime;
      }
      #endif
    }

    void setLowDeltaC(int t) { lowDeltaC=t; }
    void setHighDeltaC(int t) { highDeltaC=t; }
  private:
    unsigned long lastHeaterCycle=0;
    unsigned long currentTime=0;
    bool heaterOn=false;
    int lowDeltaC=-5;
    int highDeltaC=15;
    int DeltaC=2;

    int _pin=0;
};
