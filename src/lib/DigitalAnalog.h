// -----------------------------------------------------------------------------------
// combined digital and analog threshold read/write, 10 bit analog read resolution only
// analog mode reads use an software schmitt trigger with a threshold trigger level and hysteresis band
// digital mode reads have basic hf EMI/RFI noise filtering

#pragma once

class DigitalAnalog {
  public:
    // pin        = MCU pin#
    // mode       = pin mode (INPUT or OUTPUT)
    // isAnalog   = specifices if pin is analog (true or false) where false is digital
    // threshold  = value (0 to 1023) where result switches from LOW to HIGH
    // hysteresis = range +/- (0 to 1023) where result is igonored (uses last valid reading)
    // invert     = reverses output or input state
    void init(int pin, int mode, bool isAnalog, int threshold, int hysteresis, bool invert) {
      _isAnalog=isAnalog;
      _pin=pin;
      _mode=mode;
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
      if (_mode == OUTPUT) return _lastValue;

      int res = _lastValue;
      if (_isAnalog) {
        int sample = analogRead(_pin);
        if (sample > _threshold+_hysteresis ) res = HIGH;
        if (sample <= _threshold-_hysteresis ) res = LOW;
      } else {
        int sample1=digitalRead(_pin); delayMicroseconds(10);
        int sample2=digitalRead(_pin); delayMicroseconds(10);
        int sample3=digitalRead(_pin);
        if (sample1 == sample2 && sample2 == sample3) res=sample1;
      }
      _lastValue=res;
      if (_invert) { if (res == LOW) return HIGH; else return LOW; } else return res;
    }

    void write(int value) {
      if (_mode == INPUT) return;

      if (value > _threshold+_hysteresis ) _lastValue=HIGH;
      if (value <= _threshold-_hysteresis ) _lastValue=LOW;
      int res = _lastValue;
      if (_invert) { if (res == LOW) res=HIGH; else res=LOW; }
      if (_isAnalog) { if (res == HIGH) analogWrite(_pin,1023); else analogWrite(_pin,0); } else digitalWrite(_pin,res);
    }

    void reset() {
      if (_mode == OUTPUT) {
        _lastValue=LOW;
        int res=LOW; if (_invert) res=HIGH;
        if (_isAnalog) { if (res == HIGH) analogWrite(_pin,1023); else analogWrite(_pin,0); } else digitalWrite(_pin,res);
      } else {
        if (_isAnalog) { if (analogRead(_pin) > _threshold) _lastValue = HIGH; else _lastValue = LOW; } else _lastValue=digitalRead(_pin);
      }
    }

  private:
    bool _isAnalog=false;
    int _pin=0;
    int _mode=0;
    int _threshold=0;
    int _hysteresis=0;
    bool _invert=false;
    int _lastValue=0;
 };
