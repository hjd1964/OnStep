// -----------------------------------------------------------------------------------
// combined digital and analog threshold read, 10 bit analog read resolution only
// analog mode reads uses software schmitt trigger with threshold/hysteresis-band
// digital mode reads have basic hf EMI/RFI noise filtering

#pragma once

class DigitalAnalogInput {
  public:
    // pin        = MCU pin#
    // mode       = pin mode (INPUT, INPUT_PULLUP, or INPUT_PULLDOWN)
    // isAnalog   = specifices if pin is analog (true or false) where false is digital
    // threshold  = value (0 to 1023) where result switches from LOW to HIGH
    // hysteresis = range +/- (0 to 1023) where result is igonored (uses last valid reading)
    // invert     = reverses output or input state
    void init(int pin, int mode, bool isAnalog, int threshold, int hysteresis, bool invert) {
      if (mode == OUTPUT) { _mode=OFF; return; }
      _mode=mode;
      _isAnalog=isAnalog;
      _pin=pin;
      _threshold=threshold;
      _hysteresis=hysteresis;
      _invert=invert;
      if (_isAnalog) {
        if (_threshold+_hysteresis > 1023) { _threshold = (int)(1023*0.95); _hysteresis = 0; }
        if (_threshold-_hysteresis < 0) { _threshold = (int)(1023*0.05); _hysteresis = 0; }
      }
      pinMode(_pin,_mode);
      reset();
    }

    int read() {
      if (_mode == OFF) return 0;
      int value = _lastValue;
      if (_isAnalog) {
        int sample = analogRead(_pin);
        if (sample > _threshold+_hysteresis ) value = HIGH;
        if (sample <= _threshold-_hysteresis ) value = LOW;
      } else {
        int sample1=digitalRead(_pin); delayMicroseconds(30);
        int sample2=digitalRead(_pin); delayMicroseconds(30);
        int sample3=digitalRead(_pin);
        if (sample1 == sample2 && sample2 == sample3) value=sample1;
      }
      _lastValue=value;
      if (_invert) { if (value == LOW) return HIGH; else return LOW; } else return value;
    }

    void reset() {
      if (_mode == OFF) return;
      if (_isAnalog) { if (analogRead(_pin) > _threshold) _lastValue = HIGH; else _lastValue = LOW; } else _lastValue=digitalRead(_pin);
    }

  private:
    bool _isAnalog=false;
    int _pin=0;
    int _mode=OFF;
    int _threshold=0;
    int _hysteresis=0;
    bool _invert=false;
    int _lastValue=0;
 };
