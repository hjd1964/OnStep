// -----------------------------------------------------------------------------------------------------------------------------
// Sound related functions

// sound/buzzer
void soundAlert() {
  if (soundEnabled) {
    #ifdef BUZZER_ON
      digitalWrite(TonePin,HIGH); buzzerDuration=100;
    #endif
    #ifdef BUZZER
      tone(TonePin,BUZZER,1000);
    #endif
  }
}

// sound/beep
void soundBeep() {
  if (soundEnabled) {
    #ifdef BUZZER_ON
      digitalWrite(TonePin,HIGH); buzzerDuration=25;
    #endif
    #ifdef BUZZER
      tone(TonePin,BUZZER,250);
    #endif
  }
}

// sound/click
void soundClick() {
  if (soundEnabled) {
    #ifdef BUZZER_ON
      digitalWrite(TonePin,HIGH); buzzerDuration=5;
    #endif
    #ifdef BUZZER
      tone(TonePin,BUZZER,50);
    #endif
  }
}
