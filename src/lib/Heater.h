// -----------------------------------------------------------------------------------
// Control a single dew Heater

#pragma once

#define DH_PULSE_WIDTH_MS 2000

class dewHeaterControl {
  public:
    void init(int pin) {
      _pin=pin;
      if (_pin>0 && _pin<1024) pinMode(_pin,OUTPUT);
    }

    void poll(int deltaAboveDewPointC) {
      int switchTimeMs=0;
      deltaAboveDewPointC=constrain(deltaAboveDewPointC,lowDeltaC,highDeltaC);
      switchTimeMs=map(deltaAboveDewPointC,lowDeltaC,highDeltaC,DH_PULSE_WIDTH_MS,0);
      currentTime = millis();
      if (!heaterOn && (long)(currentTime - (lastHeaterCycle + switchTimeMs)) <= 0)
      {
        if (heaterEnabled && _pin>0 && _pin<1024) digitalWrite(_pin,HIGH);
        heaterOn=true;
      }
      else if (heaterOn && (long)(currentTime - (lastHeaterCycle + switchTimeMs)) > 0) 
      {
        if (_pin>0  && _pin<1024) digitalWrite(_pin,LOW);
        heaterOn=false;
      }
      else if ((long)(currentTime - (lastHeaterCycle + DH_PULSE_WIDTH_MS)) > 0) {
        lastHeaterCycle=currentTime;
      }
    }

	int getLowDeltaC() {
		return lowDeltaC;
	}
	
	int getHighDeltaC() {
		return highDeltaC;
	}

    void setLowDeltaC(int t) { lowDeltaC=t; }
    void setHighDeltaC(int t) { highDeltaC=t; }
	
	void setEnabled(bool e) { 
		heaterEnabled=e; 
		if (!heaterEnabled && _pin>0  && _pin<1024) 
			{
				digitalWrite(_pin,LOW);
				heaterOn=false;
			}
	}
    bool isHeaterOn() { return heaterOn; }
  
  private:
    unsigned long lastHeaterCycle=0;
    unsigned long currentTime=0;
    
    bool heaterOn=false;
	bool heaterEnabled=true;
    
    int lowDeltaC=-5;
    int highDeltaC=15;
 
    int _pin=0;
};
