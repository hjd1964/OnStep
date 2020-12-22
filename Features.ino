// -----------------------------------------------------------------------------------
// Auxiliary Features

#ifdef FEATURES_PRESENT
void featuresInit() {
  for (int i=0; i < 8; i++) {
    if (feature[i].value == ON) feature[i].value=1; else if (feature[i].value < 0 || feature[i].value > 255) feature[i].value=0;
    if (feature[i].purpose == SWITCH || feature[i].purpose == SWITCH_UNPARKED) {
      if (feature[i].pin >= 0 && feature[i].pin <= 255) {
        pinMode(feature[i].pin,OUTPUT); digitalWrite(feature[i].pin,feature[i].value==feature[i].active?HIGH:LOW);
      } else ambient.setDS2413State(i,feature[i].value==feature[i].active?HIGH:LOW);
    } else if (feature[i].purpose == ANALOG_OUTPUT) {
      if (feature[i].pin >= 0 && feature[i].pin <= 255) { pinMode(feature[i].pin,OUTPUT); analogWrite(feature[i].pin,feature[i].value); }
    } else if (feature[i].purpose == DEW_HEATER) {
      feature[i].dewHeater = new dewHeaterControl;
      feature[i].dewHeater->init(EE_feature1Value1+i*3);
      if (feature[i].pin >= 0 && feature[i].pin <= 255) pinMode(feature[i].pin,OUTPUT);
      feature[i].dewHeater->enable(feature[i].value);
    } else if (feature[i].purpose == INTERVALOMETER) {
      feature[i].intervalometer = new intervalometerControl;
      feature[i].intervalometer->init(EE_feature1Value1+i*3);
      if (feature[i].pin >= 0 && feature[i].pin <= 255) {
        pinMode(feature[i].pin,OUTPUT); digitalWrite(feature[i].pin,feature[i].value==feature[i].active?HIGH:LOW);
      }
    }
  }
}

void featuresPoll() {
#ifdef FEATURES_PRESENT
  for (int i=0; i < 8; i++) {
    if (feature[i].purpose == SWITCH_UNPARKED) {
      int v=feature[i].value;
      if (parkStatus != Parked) feature[i].value=1; else feature[i].value=0;
      if (feature[i].value != v) {
        if (feature[i].pin >= 0 && feature[i].pin <= 255) { digitalWrite(feature[i].pin,feature[i].value==feature[i].active?HIGH:LOW); } else {
          ambient.setDS2413State(i,feature[i].value==feature[i].active?1:0); if (ambient.getDS2413Failure(i)) feature[i].value=0;
        }
      }
    } else
    if (feature[i].purpose == DEW_HEATER) {
      feature[i].dewHeater->poll(ambient.getFeatureTemperature(i)-ambient.getDewPoint());
      if (isDS2413(feature[i].pin)) {
        ambient.setDS2413State(i,feature[i].dewHeater->isOn()==feature[i].active?HIGH:LOW); if (ambient.getDS2413Failure(i)) feature[i].dewHeater->enable(false);
      } else {
        if (feature[i].pin >= 0 && feature[i].pin <= 255) { digitalWrite(feature[i].pin,feature[i].dewHeater->isOn()==feature[i].active?HIGH:LOW); }
      }
    } else
    if (feature[i].purpose == INTERVALOMETER) {
      feature[i].intervalometer->poll();
      if (isDS2413(feature[i].pin)) {
        ambient.setDS2413State(i,feature[i].intervalometer->isOn()==feature[i].active?HIGH:LOW); if (ambient.getDS2413Failure(i)) feature[i].intervalometer->enable(false);
      } else {
        if (feature[i].pin >= 0 && feature[i].pin <= 255) { digitalWrite(feature[i].pin,feature[i].intervalometer->isOn()==feature[i].active?HIGH:LOW); }
      }
    }
  }
#endif
}

// :GXXn#
void featuresGetCommand(char *parameter, char *reply, bool &boolReply) {
  int i=parameter[1]-'1';
  if (i < 0 || i > 7)  { commandError=CE_PARAM_FORM; return; }

  char s[255];
  if (feature[i].purpose == SWITCH || feature[i].purpose == SWITCH_UNPARKED) {
    if (feature[i].pin >= 0 && feature[i].pin <= 255) sprintf(s,"%d",feature[i].value); else sprintf(s,"%d",ambient.getDS2413State(i)==feature[i].active?1:0); strcat(reply,s);
  } else if (feature[i].purpose == ANALOG_OUTPUT) {
    if (feature[i].pin >= 0 && feature[i].pin <= 255) sprintf(s,"%d",feature[i].value); else strcpy(s,"0"); strcat(reply,s);
  } else if (feature[i].purpose == DEW_HEATER) {
    sprintf(s,"%d",(int)feature[i].dewHeater->isEnabled()); strcat(reply,s); strcat(reply,",");
    dtostrf(feature[i].dewHeater->getZero(),3,1,s); strcat(reply,s); strcat(reply,",");
    dtostrf(feature[i].dewHeater->getSpan(),3,1,s); strcat(reply,s); strcat(reply,",");
    dtostrf(ambient.getFeatureTemperature(i)-ambient.getDewPoint(),3,1,s); strcat(reply,s);
  } else if (feature[i].purpose == INTERVALOMETER) {
    sprintf(s,"%d",(int)feature[i].intervalometer->getCurrentCount()); strcat(reply,s); strcat(reply,",");
    float v; int d;
    v=feature[i].intervalometer->getExposure(); if (v < 1.0) d=3; else if (v < 10.0) d=2; else if (v < 30.0) d=1; else d=0;
    dtostrf(v,0,d,s); strcat(reply,s); strcat(reply,",");
    v=feature[i].intervalometer->getDelay(); if (v < 10.0) d=2; else if (v < 30.0) d=1; else d=0;
    dtostrf(v,0,d,s); strcat(reply,s); strcat(reply,",");
    sprintf(s,"%d",(int)feature[i].intervalometer->getCount()); strcat(reply,s);
  } else { commandError=CE_CMD_UNKNOWN; return; }
  boolReply=false;
}

// :GXYn#
void featuresGetInfoCommand(char *parameter, char *reply, bool &boolReply) {
  int i=parameter[1]-'1';
  // return active features
  if (i == -1) {
    for (int j=0; j < 8; j++) { if (feature[j].purpose == OFF) reply[j] = '0'; else reply[j] = '1'; reply[j+1]=0; }
    boolReply=false;
    return;
  }
  if (i < 0 || i > 7)  { commandError=CE_PARAM_FORM; return; }
  if (feature[i].purpose == OFF) { commandError=CE_0; return; }
  
  char s[255];
  strcpy(s,feature[i].name); if (strlen(s)>10) s[10]=0; strcpy(reply,s); strcat(reply,",");
  int p=feature[i].purpose;
  if (p == SWITCH_UNPARKED) p = SWITCH;
  sprintf(s,"%d",p); strcat(reply,s);
  boolReply=false;
}

// :SXX[n],V[Z][S][v]#
// for example :SXX1,V1#  :SXX1,Z0.5#
void featuresSetCommand(char *parameter) {
  int i=parameter[1]-'1';
  if (i < 0 || i > 7)  { commandError=CE_PARAM_FORM; return; }
  if (feature[i].purpose == OFF) { commandError=CE_CMD_UNKNOWN; return; }
  
  char* conv_end;
  double f=strtod(&parameter[4],&conv_end);
  if (&parameter[4] == conv_end) { commandError=CE_PARAM_FORM; return; }
  long v = lround(f);

  if (parameter[3] == 'V' && v >= 0 && v <= 255) feature[i].value=v;
  if (feature[i].purpose == SWITCH || feature[i].purpose == SWITCH_UNPARKED) {
    if (parameter[3] == 'V') {
      if (v >= 0 && v <= 1) {
        if (feature[i].purpose == SWITCH) {
          if (feature[i].pin >= 0 && feature[i].pin <= 255) { digitalWrite(feature[i].pin,v==feature[i].active?1:0); } else ambient.setDS2413State(i,v==feature[i].active?1:0);
        }
      } else commandError=CE_PARAM_RANGE;
    } else commandError=CE_PARAM_FORM;
  } else if (feature[i].purpose == ANALOG_OUTPUT) {
    if (parameter[3] == 'V') {
      if (v >= 0 && v <= 255) {
        if (feature[i].pin >= 0 && feature[i].pin <= 255) analogWrite(feature[i].pin,v);
      } else commandError=CE_PARAM_RANGE;
    } else commandError=CE_PARAM_FORM;
  } else if (feature[i].purpose == DEW_HEATER) {
    if (parameter[3] == 'V') {
      if (v >= 0 && v <= 1) feature[i].dewHeater->enable(v); else commandError=CE_PARAM_RANGE;
    } else if (parameter[3] == 'Z') { // zero
      if (f >= -5.0 && f <= 20.0) feature[i].dewHeater->setZero(f); else commandError=CE_PARAM_RANGE;
    } else if (parameter[3] == 'S') { // span
      if (f >= -5.0 && f <= 20.0) feature[i].dewHeater->setSpan(f); else commandError=CE_PARAM_RANGE;
    } else commandError=CE_PARAM_FORM;
  } else if (feature[i].purpose == INTERVALOMETER) {
    if (parameter[3] == 'V') {
      if (v >= 0 && v <= 1) feature[i].intervalometer->enable(v); else commandError=CE_PARAM_RANGE;
    } else if (parameter[3] == 'E') { // exposure length
      if (f >= 0.0 && f <= 3600.0) feature[i].intervalometer->setExposure(f); else commandError=CE_PARAM_RANGE;
    } else if (parameter[3] == 'D') { // delay
      if (f >= 1.0 && f <= 3600.0) feature[i].intervalometer->setDelay(f); else commandError=CE_PARAM_RANGE;
    } else if (parameter[3] == 'C') { // count
      if (f >= 0 && f <= 255.0) feature[i].intervalometer->setCount(f); else commandError=CE_PARAM_RANGE;
    } else commandError=CE_PARAM_FORM;
  }
}

// work-around for Mega2560 64 bit conditional comparison bug ((feature[i].pin & DS_MASK) == DS2413)
bool isDS2413(int64_t v) {
  uint32_t l1 = (v & DS_MASK)&0xffff;
  uint32_t h1 = (v & DS_MASK)>>32;
  uint32_t l2 = DS2413&0xffff;
  uint32_t h2 = DS2413>>32;
  return (l1 == l2 && h1 == h2);
}
#endif
