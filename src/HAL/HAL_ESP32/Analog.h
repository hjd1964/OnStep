// -----------------------------------------------------------------------------------
// ******* Arduino compatible analogWrite() and tone() functions for the ESP32 *******
// Takes over 8 of the 16 PWM channels (default, uses channel# 8 to 15) so only channel#'s < 8 can be used elsewhere.
// The 8 PWM channels are shared between the non-blocking tone() and analogWrite() functions and are automatically allocated as required.

portMUX_TYPE __analogOutMux = portMUX_INITIALIZER_UNLOCKED;

#define __ANALOG_PWM_CHANNEL_START 8  // set aside 8 channels for use elsewhere, use 0 to dedicate all 16 channels here

volatile uint8_t __tone_p = 0;
volatile unsigned int __tone_f = 0;
volatile unsigned long __tone_d = 0;
volatile int __channelMap[16] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
TaskHandle_t __toneTask;

// find and disable any PWM already on this pin
void __pwmDeallocateChannel(int pin) {
  for (int channel=__ANALOG_PWM_CHANNEL_START; channel<15; channel++) {
    if (__channelMap[channel]==pin) { ledcDetachPin(pin); ledcWrite(channel,0); __channelMap[channel]=-1; break; }
  }
}

// find a PWM channel for this pin, returns -1 if no channel is available
int __pwmAllocateChannel(int pin, boolean clearExisting=false) {
  if (clearExisting) __pwmDeallocateChannel(pin);
  for (int channel=__ANALOG_PWM_CHANNEL_START; channel<15; channel++) {
    if (__channelMap[channel]==-1) { __channelMap[channel]=pin; return channel; }
  }
  log_e("PWM, no channels available.");
  return -1;
}

int __pwmGetChannel(int pin) {
  for (int channel=__ANALOG_PWM_CHANNEL_START; channel<15; channel++) { if (__channelMap[channel]==pin) return channel; }
  return -1;
}

void __toneStop(void * pvParameters)
{
  delay(__tone_d);

  portENTER_CRITICAL(&__analogOutMux);
  int channel=__pwmGetChannel(__tone_p);
  if (channel>=0) {
    ledcDetachPin(__tone_p);
    ledcWrite(channel,0);
    __pwmDeallocateChannel(__tone_p);
  }
  portEXIT_CRITICAL(&__analogOutMux);
  
  vTaskDelete(NULL);
}

__attribute__ ((weak)) void tone(uint8_t pin, unsigned int frequency, unsigned long duration = 0)
{
  portENTER_CRITICAL(&__analogOutMux);
  int channel=__pwmAllocateChannel(pin);
  if (channel>=0) {
    __tone_p=pin; __tone_f=frequency; __tone_d=duration;
    ledcAttachPin(pin,channel);
    ledcWriteTone(channel,frequency);
    if (duration) xTaskCreatePinnedToCore(__toneStop,"__toneStop",10000,NULL,1,&__toneTask,0);
  } else log_e("Tone, PWM channel in use or unavailable.");
  portEXIT_CRITICAL(&__analogOutMux);
}

__attribute__ ((weak)) void noTone(uint8_t pin)
{
  portENTER_CRITICAL(&__analogOutMux);
  int channel=__pwmGetChannel(pin);
  if (channel>=0) {
    ledcDetachPin(pin);
    ledcWrite(channel,0);
    __pwmDeallocateChannel(pin);
  } else log_e("noTone, no PWM channel?");
  portEXIT_CRITICAL(&__analogOutMux);
}

__attribute__ ((weak)) void analogWrite(uint8_t pin, int value) {
  portENTER_CRITICAL(&__analogOutMux);
  int channel=__pwmAllocateChannel(pin,true);  // using option to clear/detach any existing channel for this pin
  if (channel>=0) {
    ledcSetup(channel,1000,8); // 1000Hz, 8 bit
    ledcAttachPin(pin,channel);
    ledcWrite(channel,value);
    // just disconnect and go digital if value is 0 or 255
    if (value==0) { noTone(pin); digitalWrite(pin,LOW); }
    if (value==255) { noTone(pin); digitalWrite(pin,HIGH); }
  } else log_e("analogWrite, PWM channel in use or unavailable.");
  portEXIT_CRITICAL(&__analogOutMux);
}
