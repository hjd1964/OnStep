// -----------------------------------------------------------------------------------
// Auxiliary Features

#ifdef FEATURES_PRESENT
void featuresInit() {
  for (int i=0; i < 8; i++) {
    if (feature[i].purpose == SWITCH || feature[i].purpose == ANALOG_OUTPUT) {
      if (feature[i].pin >= 0 && feature[i].pin <= 255) pinMode(feature[i].pin,OUTPUT);
    } else if (feature[i].purpose == DEW_HEATER) {
      feature[i].dewHeater = new dewHeaterControl;
      if (feature[i].pin >= 0 && feature[i].pin <= 255) feature[i].dewHeater->init(feature[i].pin,EE_feature1Value1+i*3); else feature[i].dewHeater->init(-1,EE_feature1Value1+i*3);
    } else if (feature[i].purpose == INTERVALOMETER) {
      feature[i].intervalometer = new intervalometerControl;
      if (feature[i].pin >= 0 && feature[i].pin <= 255) feature[i].intervalometer->init(feature[i].pin,EE_feature1Value1+i*3); else feature[i].intervalometer->init(-1,EE_feature1Value1+i*3);
    }
  }
}

void featuresPoll() {
#ifdef FEATURES_PRESENT
  for (int i=0; i < 8; i++) {
    if (feature[i].purpose == DEW_HEATER) {
      feature[i].dewHeater->poll(ambient.getFeatureTemperature(0)-ambient.getDewPoint());
      if (isDS2413(feature[i].pin)) ambient.setDS2413State(i,feature[i].dewHeater->isOn());
    } else
    if (feature[i].purpose == INTERVALOMETER) {
      feature[i].intervalometer->poll();
      if (isDS2413(feature[i].pin)) ambient.setDS2413State(i,feature[i].intervalometer->isOn());
    }
  }
#endif
}

// :GXXn#
void featuresGetCommand(char *parameter, char *reply, bool &booleanReply) {
  int i=parameter[1]-'1';
  if (i < 0 || i > 7)  { commandError=CE_PARAM_FORM; return; }

  char s[255];
  if (feature[i].purpose == SWITCH) {
    if (feature[i].pin >= 0 && feature[i].pin <= 255) sprintf(s,"%d",feature[i].value); else sprintf(s,"%d",ambient.getDS2413State(i)); strcat(reply,s);
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
  booleanReply=false;
}

// :GXYn#
void featuresGetInfoCommand(char *parameter, char *reply, bool &booleanReply) {
  int i=parameter[1]-'1';
  // return active features
  if (i == -1) {
    for (int j=0; j < 8; j++) { if (feature[j].purpose == OFF) reply[j] = '0'; else reply[j] = '1'; reply[j+1]=0; }
    booleanReply=false;
    return;
  }
  if (i < 0 || i > 7)  { commandError=CE_PARAM_FORM; return; }
  if (feature[i].purpose == OFF) { commandError=CE_CMD_UNKNOWN; return; }
  
  char s[255];
  strcpy(s,feature[i].name); if (strlen(s)>10) s[10]=0; strcpy(reply,s); strcat(reply,",");
  sprintf(s,"%d",(int)feature[i].purpose); strcat(reply,s);
  booleanReply=false;
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
  if (feature[i].purpose == SWITCH) {
    if (parameter[3] == 'V') {
      if (v >= 0 && v <= 1) {
        if (feature[i].pin >= 0 && feature[i].pin <= 255) { digitalWrite(feature[i].pin,v==0?LOW:HIGH); } else ambient.setDS2413State(feature[i].pin,v==0?0:1);
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
