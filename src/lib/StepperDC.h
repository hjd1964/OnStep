// -----------------------------------------------------------------------------------
// StepperDC

// supports two DC motors attached to an A4988 or DRV8825 (etc?)

#pragma once

bool __initialized=false;

class stepperDC {
  public:
    void init(int stepPin, int dirPin, int enPin, long maxRate) {
      if (__initialized) return;
      __initialized=true;
      
      _stepPin=stepPin;
      _dirPin=dirPin;
      _enPin=enPin;
      _maxRate=maxRate;

      if ((_enPin<0) || (_stepPin<0) || (_dirPin<0)) return; 
      
      pinMode(_enPin,OUTPUT); 
      pinMode(_dirPin,OUTPUT);
      pinMode(_stepPin,OUTPUT);

      // Designed for half-stepping mode
      //   A B
      // 1 + 0
      // 2 + + home
      // 3 0 +
      // 4 - +
      // 5 - 0
      // 6 - -
      // 7 0 -
      // 8 + -

      // enable the stepper driver
      digitalWrite(_enPin,_enableState); delayMicroseconds(2);

      // take a step backwards to our new home, position 1, coil A +
      digitalWrite(_dirPin,LOW); delayMicroseconds(2);
      step();
      _phase=1;

      // move forward from now on
      digitalWrite(_dirPin,HIGH); delayMicroseconds(2);

      // disable the stepper driver
      digitalWrite(_enPin,_disableState); delayMicroseconds(2);

      setPower(0);
      enabled(false);
    }

    void poll() {
      if (_enPin<0) return;

      static unsigned long nextPass=0;
      unsigned long tempMs=millis();
      if ((long)(tempMs-nextPass)>0) {
        nextPass=tempMs+(unsigned long)_maxRate;      

        if (_power==0 || !_en) { digitalWrite(_enPin,_disableState); delayMicroseconds(2); return; }

        static int seq=0;
        seq++; if (seq>9) seq=0;

        static byte pwr[10][10] = {
          {0,0,0,0,1,0,0,0,0,0},
          {0,0,1,0,0,0,0,1,0,0},
          {0,1,0,0,1,0,0,0,1,0},
          {0,1,0,1,0,1,0,1,0,0},
          {0,1,0,1,0,1,0,1,0,1},
          {0,1,1,1,0,1,0,1,0,1},
          {0,1,1,1,0,1,1,1,0,1},
          {0,1,1,1,0,1,1,1,1,1},
          {0,1,1,1,1,1,1,1,1,1},
          {1,1,1,1,1,1,1,1,1,1}
        };
        if ((bool)pwr[_power-1][seq]) digitalWrite(_enPin,_enableState); else digitalWrite(_enPin,_disableState);
        delayMicroseconds(2);
      }
    }

    // power = 0 to 100%
    void setPower(int power) {
      _power=round(power/10.0);
      if (_power<0) _power=0; 
      if (_power>10) _power=10;
    }
    void enabled(bool enState) {
      _en=enState;
      if (!_en) { digitalWrite(_enPin,_disableState); delayMicroseconds(2); }
    }

    // sets logic state for disabling stepper driver
    void setDisableState(boolean disableState) {
      _disableState=disableState;
      if (_disableState==LOW) _enableState=HIGH; else _enableState=LOW;
    }

    void setDirectionIn() {
      if (_phase==1 || _phase==3) nextDirection();  // switch to move in
    }
    void setDirectionOut() {
      if (_phase==5 || _phase==7) nextDirection();  // switch to move out
    }
    
    void setPhaseA() {
      if (_phase==3 || _phase==7) priorPhase(); // switch to phase A
    }
    void setPhaseB() {
      if (_phase==1 || _phase==5) nextPhase(); // switch to phase B
    }
  
  private:
    void nextDirection() {
      if ((_enPin<0) || (_stepPin<0)) return;
      if (!_A4988) digitalWrite(_enPin,_enableState); else digitalWrite(_enPin,_disableState); delayMicroseconds(2);
      step(); step(); step(); step();
      if (!_A4988) { digitalWrite(_enPin,_disableState); delayMicroseconds(2); }
    }

    void nextPhase() {
      if ((_enPin<0) || (_stepPin<0)) return;
      if (!_A4988) digitalWrite(_enPin,_enableState); else digitalWrite(_enPin,_disableState); delayMicroseconds(2);
      step(); step();
      if (!_A4988) { digitalWrite(_enPin,_disableState); delayMicroseconds(2); }
    }

    void priorPhase() {
      if ((_enPin<0) || (_stepPin<0)) return;
      if (!_A4988) digitalWrite(_enPin,_enableState); else digitalWrite(_enPin,_disableState); delayMicroseconds(2);
      step(); step(); step(); step(); step(); step();
      if (!_A4988) { digitalWrite(_enPin,_disableState); delayMicroseconds(2); }
    }

    void step() {
      digitalWrite(_stepPin,HIGH); delayMicroseconds(2); digitalWrite(_stepPin,LOW); delayMicroseconds(2);
      _phase++; if (_phase>8) _phase=1;
    }

    byte _phase=1;
    bool _A4988=false;
    int _power=0;
    bool _en=false;
    int _stepPin=-1;
    int _dirPin=-1;
    int _enPin=-1;
    long _maxRate=-1;
    bool _disableState=HIGH;
    int _enableState=LOW;
};

stepperDC dcMotor;
