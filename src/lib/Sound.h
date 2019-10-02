// -----------------------------------------------------------------------------------------------------------------------------
// Sound related functions

// sound/buzzer
void soundAlert() {
  if (soundEnabled) {
    #if BUZZER == ON
      digitalWrite(TonePin,HIGH); buzzerDuration=100;
    #endif
    #if BUZZER >= 0
      tone(TonePin,BUZZER,1000);
    #endif
  }
}

// sound/beep
void soundBeep() {
  if (soundEnabled) {
    #if BUZZER == ON
      digitalWrite(TonePin,HIGH); buzzerDuration=25;
    #endif
    #if BUZZER >= 0
      tone(TonePin,BUZZER,250);
    #endif
  }
}

// sound/click
void soundClick() {
  if (soundEnabled) {
    #if BUZZER == ON
      digitalWrite(TonePin,HIGH); buzzerDuration=5;
    #endif
    #if BUZZER >= 0
      tone(TonePin,BUZZER,50);
    #endif
  }
}
