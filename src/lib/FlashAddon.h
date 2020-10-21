// -----------------------------------------------------------------------------------
// Flash an Addon on the SerialB interface by passing data to/from SerialA

#pragma once

class flashAddon {
  public:
    // TriggerPin  HIGH for run and LOW for trigger serial passthrough mode
    // ResetPin    HIGH for run and LOW for reset
    // BootModePin HIGH for run and LOW for firmware upload
    void init(int triggerPin, int resetPin, int bootModePin) {
      _triggerPin = triggerPin;
      _resetPin = resetPin;
      _bootModePin = bootModePin;

      if (_triggerPin != OFF) pinMode(_triggerPin,INPUT);
      if (_bootModePin != -1) {
        pinMode(_bootModePin,OUTPUT);
        digitalWrite(_bootModePin,HIGH); delay(20); // Run mode is HIGH
      }
      if (_resetPin != -1) {
        pinMode(_resetPin,OUTPUT);
        digitalWrite(_resetPin,LOW);  delay(200);   // Reset, if LOW
        digitalWrite(_resetPin,HIGH);               // Reset, inactive HIGH
      }
    }

    void poll() {
      if (_triggerPin != OFF && digitalRead(_triggerPin) == LOW) go(true);
    }

    void go(bool inf) {
#ifdef SERIAL_B_RX
      SerialB.begin(115200, SERIAL_8N1, SERIAL_B_RX, SERIAL_B_TX);
#else
      SerialB.begin(115200);
#endif
      SerialA.begin(115200);
      delay(1000);
    
      if (_bootModePin != OFF) { digitalWrite(_bootModePin,LOW); delay(100); } // Pgm mode LOW
      if (_resetPin != OFF) { digitalWrite(_resetPin,LOW); delay(20); digitalWrite(_resetPin,HIGH); } // Reset LOW (active) HIGH (inactive)
      delay(20);
      
      unsigned long lastRead=millis()+85000; // so we have a total of 1.5 minutes to start the upload
      while (true) {
        // read from port 1, send to port 0:
        if (SerialB.available()) {
          int inByte = SerialB.read(); delayMicroseconds(5);
          SerialA.write(inByte); delayMicroseconds(5);
        }
        // read from port 0, send to port 1:
        if (SerialA.available()) {
          int inByte = SerialA.read(); delayMicroseconds(5);
          SerialB.write(inByte); delayMicroseconds(5);
          if (millis() > lastRead) lastRead=millis();
        }
        yield();
        if (!inf && (long)(millis()-lastRead) > 5000) break; // wait 5 seconds w/no traffic before resuming normal operation
      }
    
      if (_bootModePin != OFF) { digitalWrite(_bootModePin,HIGH); delay(100); } // Run mode HIGH
      if (_resetPin != OFF) { digitalWrite(_resetPin,LOW); delay(20); digitalWrite(_resetPin,HIGH); } // Reset LOW (active) HIGH (inactive)
      delay(20);

#ifdef SERIAL_B_RX
      SerialB.begin(SERIAL_B_BAUD_DEFAULT, SERIAL_8N1, SERIAL_B_RX, SERIAL_B_TX);
#else
      SerialB.begin(SERIAL_B_BAUD_DEFAULT);
#endif
      SerialA.begin(SERIAL_A_BAUD_DEFAULT);
    }

  private:
    int _triggerPin = -1;
    int _resetPin = -1;
    int _bootModePin = -1;
};
