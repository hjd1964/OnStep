// -----------------------------------------------------------------------------------
// Auxiliary Features

void featuresInit() {
#if FEATURE1_PURPOSE == SWITCH || FEATURE1_PURPOSE == ANALOG
  if (FEATURE1_PIN >= 0 && FEATURE1_PIN <= 255) pinMode((int)FEATURE1_PIN,OUTPUT);
#elif FEATURE1_PURPOSE == DEW_HEATER
  // with ambient temperature: zero = -5 (dew forming 100% power), span = 15 (dew NOT forming 0% power)
  if (FEATURE1_PIN >= 0 && FEATURE1_PIN <= 255) dewHeater1.init((int)FEATURE1_PIN,EE_feature1Value1); else dewHeater1.init(-1,EE_feature1Value1);
  // with individual temperature: zero = 1 (dew almost forming 100% power), span = 3 (dew NOT forming 0% power)
  if (FEATURE1_TEMP != OFF) { dewHeater1.setZero(1); dewHeater1.setSpan(3); }
#endif

#if FEATURE2_PURPOSE == SWITCH || FEATURE2_PURPOSE == ANALOG
  if (FEATURE2_PIN >= 0 && FEATURE2_PIN <= 255) pinMode((int)FEATURE2_PIN,OUTPUT);
#elif FEATURE2_PURPOSE == DEW_HEATER
  if (FEATURE2_PIN >= 0 && FEATURE2_PIN <= 255) dewHeater2.init((int)FEATURE2_PIN,EE_feature2Value1); else dewHeater2.init(-1,EE_feature2Value1);
  if (FEATURE2_TEMP != OFF) { dewHeater2.setZero(1); dewHeater2.setSpan(3); }
#endif

#if FEATURE3_PURPOSE == SWITCH || FEATURE3_PURPOSE == ANALOG
  if (FEATURE3_PIN >= 0 && FEATURE3_PIN <= 255) pinMode((int)FEATURE3_PIN,OUTPUT);
#elif FEATURE3_PURPOSE == DEW_HEATER
  if (FEATURE3_PIN >= 0 && FEATURE3_PIN <= 255) dewHeater3.init((int)FEATURE3_PIN,EE_feature3Value1); else dewHeater3.init(-1,EE_feature3Value1);
  if (FEATURE3_TEMP != OFF) { dewHeater3.setZero(1); dewHeater3.setSpan(3); }
#endif

#if FEATURE4_PURPOSE == SWITCH || FEATURE4_PURPOSE == ANALOG
  if (FEATURE4_PIN >= 0 && FEATURE4_PIN <= 255) pinMode((int)FEATURE4_PIN,OUTPUT);
#elif FEATURE4_PURPOSE == DEW_HEATER
  if (FEATURE4_PIN >= 0 && FEATURE4_PIN <= 255) dewHeater4.init((int)FEATURE4_PIN,EE_feature4Value1); else dewHeater4.init(-1,EE_feature4Value1);
  if (FEATURE4_TEMP != OFF) { dewHeater4.setZero(1); dewHeater4.setSpan(3); }
#endif

#if FEATURE5_PURPOSE == SWITCH || FEATURE5_PURPOSE == ANALOG
  if (FEATURE5_PIN >= 0 && FEATURE5_PIN <= 255) pinMode((int)FEATURE5_PIN,OUTPUT);
#elif FEATURE5_PURPOSE == DEW_HEATER
  if (FEATURE5_PIN >= 0 && FEATURE5_PIN <= 255) dewHeater5.init((int)FEATURE5_PIN,EE_feature5Value1); else dewHeater5.init(-1,EE_feature5Value1);
  if (FEATURE5_TEMP != OFF) { dewHeater5.setZero(1); dewHeater5.setSpan(3); }
#endif

#if FEATURE6_PURPOSE == SWITCH || FEATURE6_PURPOSE == ANALOG
  if (FEATURE6_PIN >= 0 && FEATURE6_PIN <= 255) pinMode((int)FEATURE6_PIN,OUTPUT);
#elif FEATURE6_PURPOSE == DEW_HEATER
  if (FEATURE6_PIN >= 0 && FEATURE6_PIN <= 255) dewHeater6.init((int)FEATURE6_PIN,EE_feature6Value1); else dewHeater6.init(-1,EE_feature6Value1);
  if (FEATURE6_TEMP != OFF) { dewHeater6.setZero(1); dewHeater6.setSpan(3); }
#endif

#if FEATURE7_PURPOSE == SWITCH || FEATURE7_PURPOSE == ANALOG
  if (FEATURE7_PIN >= 0 && FEATURE7_PIN <= 255) pinMode((int)FEATURE7_PIN,OUTPUT);
#elif FEATURE7_PURPOSE == DEW_HEATER
  if (FEATURE7_PIN >= 0 && FEATURE7_PIN <= 255) dewHeater7.init((int)FEATURE7_PIN,EE_feature7Value1); else dewHeater7.init(-1,EE_feature7Value1);
  if (FEATURE7_TEMP != OFF) { dewHeater7.setZero(1); dewHeater7.setSpan(3); }
#endif

#if FEATURE8_PURPOSE == SWITCH || FEATURE8_PURPOSE == ANALOG
  if (FEATURE8_PIN >= 0 && FEATURE8_PIN <= 255) pinMode((int)FEATURE8_PIN,OUTPUT);
#elif FEATURE8_PURPOSE == DEW_HEATER
  if (FEATURE8_PIN >= 0 && FEATURE8_PIN <= 255) dewHeater8.init((int)FEATURE8_PIN,EE_feature8Value1); else dewHeater8.init(-1,EE_feature8Value1);
  if (FEATURE8_TEMP != OFF) { dewHeater8.setZero(1); dewHeater8.setSpan(3); }
#endif
}

void featuresPoll() {
#if FEATURE1_PURPOSE == DEW_HEATER
    dewHeater1.poll(ambient.getFeatureTemperature(0)-ambient.getDewPoint());
    if ((FEATURE1_PIN & DS_MASK) == DS2413) ambient.setDS2413State(0,dewHeater1.isOn());
#endif
#if FEATURE2_PURPOSE == DEW_HEATER
    dewHeater2.poll(ambient.getFeatureTemperature(1)-ambient.getDewPoint());
    if ((FEATURE2_PIN & DS_MASK) == DS2413) ambient.setDS2413State(1,dewHeater2.isOn());
#endif
#if FEATURE3_PURPOSE == DEW_HEATER
    dewHeater3.poll(ambient.getFeatureTemperature(2)-ambient.getDewPoint());
    if ((FEATURE3_PIN & DS_MASK) == DS2413) ambient.setDS2413State(2,dewHeater3.isOn());
#endif
#if FEATURE4_PURPOSE == DEW_HEATER
    dewHeater4.poll(ambient.getFeatureTemperature(3)-ambient.getDewPoint());
    if ((FEATURE4_PIN & DS_MASK) == DS2413) ambient.setDS2413State(3,dewHeater4.isOn());
#endif
#if FEATURE5_PURPOSE == DEW_HEATER
    dewHeater5.poll(ambient.getFeatureTemperature(4)-ambient.getDewPoint());
    if ((FEATURE5_PIN & DS_MASK) == DS2413) ambient.setDS2413State(4,dewHeater5.isOn());
#endif
#if FEATURE6_PURPOSE == DEW_HEATER
    dewHeater6.poll(ambient.getFeatureTemperature(5)-ambient.getDewPoint());
    if ((FEATURE6_PIN & DS_MASK) == DS2413) ambient.setDS2413State(5,dewHeater6.isOn());
#endif
#if FEATURE7_PURPOSE == DEW_HEATER
    dewHeater7.poll(ambient.getFeatureTemperature(6)-ambient.getDewPoint());
    if ((FEATURE7_PIN & DS_MASK) == DS2413) ambient.setDS2413State(6,dewHeater7.isOn());
#endif
#if FEATURE8_PURPOSE == DEW_HEATER
    dewHeater8.poll(ambient.getFeatureTemperature(7)-ambient.getDewPoint());
    if ((FEATURE8_PIN & DS_MASK) == DS2413) ambient.setDS2413State(7,dewHeater8.isOn());
#endif
}

// :GXXn#
void featuresGetCommand(char *parameter, char *reply, bool &booleanReply) {
#if FEATURE1_PURPOSE != OFF || FEATURE2_PURPOSE != OFF || FEATURE3_PURPOSE != OFF || FEATURE4_PURPOSE != OFF || FEATURE5_PURPOSE != OFF || FEATURE6_PURPOSE != OFF || FEATURE7_PURPOSE != OFF || FEATURE8_PURPOSE != OFF
  char s[255];
#endif
  switch (parameter[1]) {
#if FEATURE1_PURPOSE != OFF
    case '1': strcpy(s,FEATURE1_NAME); if (strlen(s)>10) s[10]=0; strcpy(reply,s); strcat(reply,","); sprintf(s,"%d",(int)FEATURE1_PURPOSE); strcat(reply,s);
#endif
#if FEATURE1_PURPOSE == SWITCH
    strcat(reply,","); if (FEATURE1_PIN >= 0 && FEATURE1_PIN <= 255) sprintf(s,"%d",(int)digitalRead((int)FEATURE1_PIN)==LOW?0:1); else strcpy(s,"0"); strcat(reply,s);
#elif FEATURE1_PURPOSE == ANALOG
    strcat(reply,","); if (FEATURE1_PIN >= 0 && FEATURE1_PIN <= 255) sprintf(s,"%d",(int)analogRead((int)FEATURE1_PIN)); else strcpy(s,"0"); strcat(reply,s);
#elif FEATURE1_PURPOSE == DEW_HEATER
    strcat(reply,","); sprintf(s,"%d",(int)dewHeater1.isEnabled()); strcat(reply,s); strcat(reply,","); dtostrf(dewHeater1.getZero(),3,1,s); strcat(reply,s); strcat(reply,","); dtostrf(dewHeater1.getSpan(),3,1,s); strcat(reply,s);
#endif
#if FEATURE1_PURPOSE != OFF
    booleanReply=false; break;
#endif

#if FEATURE2_PURPOSE != OFF
    case '2': strcpy(s,FEATURE2_NAME); if (strlen(s)>10) s[10]=0; strcpy(reply,s); strcat(reply,","); sprintf(s,"%d",(int)FEATURE2_PURPOSE); strcat(reply,s);
#endif
#if FEATURE2_PURPOSE == SWITCH
    strcat(reply,","); if (FEATURE2_PIN >= 0 && FEATURE2_PIN <= 255) sprintf(s,"%d",(int)digitalRead((int)FEATURE2_PIN)==LOW?0:1); else strcpy(s,"0"); strcat(reply,s);
#elif FEATURE2_PURPOSE == ANALOG
    strcat(reply,","); if (FEATURE2_PIN >= 0 && FEATURE2_PIN <= 255) sprintf(s,"%d",(int)analogRead((int)FEATURE2_PIN)); else strcpy(s,"0"); strcat(reply,s);
#elif FEATURE2_PURPOSE == DEW_HEATER
    strcat(reply,","); sprintf(s,"%d",(int)dewHeater2.isEnabled()); strcat(reply,s); strcat(reply,","); dtostrf(dewHeater2.getZero(),3,1,s); strcat(reply,s); strcat(reply,","); dtostrf(dewHeater2.getSpan(),3,1,s); strcat(reply,s);
#endif
#if FEATURE2_PURPOSE != OFF
    booleanReply=false; break;
#endif

#if FEATURE3_PURPOSE != OFF
    case '3': strcpy(s,FEATURE3_NAME); if (strlen(s)>10) s[10]=0; strcpy(reply,s); strcat(reply,","); sprintf(s,"%d",(int)FEATURE3_PURPOSE); strcat(reply,s);
#endif
#if FEATURE3_PURPOSE == SWITCH
    strcat(reply,","); if (FEATURE3_PIN >= 0 && FEATURE3_PIN <= 255) sprintf(s,"%d",(int)digitalRead((int)FEATURE3_PIN)==LOW?0:1); else strcpy(s,"0"); strcat(reply,s);
#elif FEATURE3_PURPOSE == ANALOG
    strcat(reply,","); if (FEATURE3_PIN >= 0 && FEATURE3_PIN <= 255) sprintf(s,"%d",(int)analogRead((int)FEATURE3_PIN)); else strcpy(s,"0"); strcat(reply,s);
#elif FEATURE3_PURPOSE == DEW_HEATER
    strcat(reply,","); sprintf(s,"%d",(int)dewHeater3.isEnabled()); strcat(reply,s); strcat(reply,","); dtostrf(dewHeater3.getZero(),3,1,s); strcat(reply,s); strcat(reply,","); dtostrf(dewHeater3.getSpan(),3,1,s); strcat(reply,s);
#endif
#if FEATURE3_PURPOSE != OFF
    booleanReply=false; break;
#endif

#if FEATURE4_PURPOSE != OFF
    case '4': strcpy(s,FEATURE4_NAME); if (strlen(s)>10) s[10]=0; strcpy(reply,s); strcat(reply,","); sprintf(s,"%d",(int)FEATURE4_PURPOSE); strcat(reply,s);
#endif
#if FEATURE4_PURPOSE == SWITCH
    strcat(reply,","); if (FEATURE4_PIN >= 0 && FEATURE4_PIN <= 255) sprintf(s,"%d",(int)digitalRead((int)FEATURE4_PIN)==LOW?0:1); else strcpy(s,"0"); strcat(reply,s);
#elif FEATURE4_PURPOSE == ANALOG
    strcat(reply,","); if (FEATURE4_PIN >= 0 && FEATURE4_PIN <= 255) sprintf(s,"%d",(int)analogRead((int)FEATURE4_PIN)); else strcpy(s,"0"); strcat(reply,s);
#elif FEATURE4_PURPOSE == DEW_HEATER
    strcat(reply,","); sprintf(s,"%d",(int)dewHeater4.isEnabled()); strcat(reply,s); strcat(reply,","); dtostrf(dewHeater4.getZero(),3,1,s); strcat(reply,s); strcat(reply,","); dtostrf(dewHeater4.getSpan(),3,1,s); strcat(reply,s);
#endif
#if FEATURE4_PURPOSE != OFF
    booleanReply=false; break;
#endif

#if FEATURE5_PURPOSE != OFF
    case '5': strcpy(s,FEATURE5_NAME); if (strlen(s)>10) s[10]=0; strcpy(reply,s); strcat(reply,","); sprintf(s,"%d",(int)FEATURE5_PURPOSE); strcat(reply,s);
#endif
#if FEATURE5_PURPOSE == SWITCH
    strcat(reply,","); if (FEATURE5_PIN >= 0 && FEATURE5_PIN <= 255) sprintf(s,"%d",(int)digitalRead((int)FEATURE5_PIN)==LOW?0:1); else strcpy(s,"0"); strcat(reply,s);
#elif FEATURE5_PURPOSE == ANALOG
    strcat(reply,","); if (FEATURE5_PIN >= 0 && FEATURE5_PIN <= 255) sprintf(s,"%d",(int)analogRead((int)FEATURE5_PIN)); else strcpy(s,"0"); strcat(reply,s);
#elif FEATURE5_PURPOSE == DEW_HEATER
    strcat(reply,","); sprintf(s,"%d",(int)dewHeater5.isEnabled()); strcat(reply,s); strcat(reply,","); dtostrf(dewHeater5.getZero(),3,1,s); strcat(reply,s); strcat(reply,","); dtostrf(dewHeater5.getSpan(),3,1,s); strcat(reply,s);
#endif
#if FEATURE5_PURPOSE != OFF
    booleanReply=false; break;
#endif

#if FEATURE6_PURPOSE != OFF
    case '6': strcpy(s,FEATURE6_NAME); if (strlen(s)>10) s[10]=0; strcpy(reply,s); strcat(reply,","); sprintf(s,"%d",(int)FEATURE6_PURPOSE); strcat(reply,s);
#endif
#if FEATURE6_PURPOSE == SWITCH
    strcat(reply,","); if (FEATURE6_PIN >= 0 && FEATURE6_PIN <= 255) sprintf(s,"%d",(int)digitalRead((int)FEATURE6_PIN)==LOW?0:1); else strcpy(s,"0"); strcat(reply,s);
#elif FEATURE6_PURPOSE == ANALOG
    strcat(reply,","); if (FEATURE6_PIN >= 0 && FEATURE6_PIN <= 255) sprintf(s,"%d",(int)analogRead((int)FEATURE6_PIN)); else strcpy(s,"0"); strcat(reply,s);
#elif FEATURE6_PURPOSE == DEW_HEATER
    strcat(reply,","); sprintf(s,"%d",(int)dewHeater6.isEnabled()); strcat(reply,s); strcat(reply,","); dtostrf(dewHeater6.getZero(),3,1,s); strcat(reply,s); strcat(reply,","); dtostrf(dewHeater6.getSpan(),3,1,s); strcat(reply,s);
#endif
#if FEATURE6_PURPOSE != OFF
    booleanReply=false; break;
#endif

#if FEATURE7_PURPOSE != OFF
    case '7': strcpy(s,FEATURE7_NAME); if (strlen(s)>10) s[10]=0; strcpy(reply,s); strcat(reply,","); sprintf(s,"%d",(int)FEATURE7_PURPOSE); strcat(reply,s);
#endif
#if FEATURE7_PURPOSE == SWITCH
    strcat(reply,","); if (FEATURE7_PIN >= 0 && FEATURE7_PIN <= 255) sprintf(s,"%d",(int)digitalRead((int)FEATURE7_PIN)==LOW?0:1); else strcpy(s,"0"); strcat(reply,s);
#elif FEATURE7_PURPOSE == ANALOG
    strcat(reply,","); if (FEATURE7_PIN >= 0 && FEATURE7_PIN <= 255) sprintf(s,"%d",(int)analogRead((int)FEATURE7_PIN)); else strcpy(s,"0"); strcat(reply,s);
#elif FEATURE7_PURPOSE == DEW_HEATER
    strcat(reply,","); sprintf(s,"%d",(int)dewHeater7.isEnabled()); strcat(reply,s); strcat(reply,","); dtostrf(dewHeater7.getZero(),3,1,s); strcat(reply,s); strcat(reply,","); dtostrf(dewHeater7.getSpan(),3,1,s); strcat(reply,s);
#endif
#if FEATURE7_PURPOSE != OFF
    booleanReply=false; break;
#endif

#if FEATURE8_PURPOSE != OFF
    case '2': strcpy(s,FEATURE8_NAME); if (strlen(s)>10) s[10]=0; strcpy(reply,s); strcat(reply,","); sprintf(s,"%d",(int)FEATURE8_PURPOSE); strcat(reply,s);
#endif
#if FEATURE8_PURPOSE == SWITCH
    strcat(reply,","); if (FEATURE8_PIN >= 0 && FEATURE8_PIN <= 255) sprintf(s,"%d",(int)digitalRead((int)FEATURE8_PIN)==LOW?0:1); else strcpy(s,"0"); strcat(reply,s);
#elif FEATURE8_PURPOSE == ANALOG
    strcat(reply,","); if (FEATURE8_PIN >= 0 && FEATURE8_PIN <= 255) sprintf(s,"%d",(int)analogRead((int)FEATURE8_PIN)); else strcpy(s,"0"); strcat(reply,s);
#elif FEATURE8_PURPOSE == DEW_HEATER
    strcat(reply,","); sprintf(s,"%d",(int)dewHeater8.isEnabled()); strcat(reply,s); strcat(reply,","); dtostrf(dewHeater8.getZero(),3,1,s); strcat(reply,s); strcat(reply,","); dtostrf(dewHeater8.getSpan(),3,1,s); strcat(reply,s);
#endif
#if FEATURE8_PURPOSE != OFF
    booleanReply=false; break;
#endif
    default: commandError=CE_CMD_UNKNOWN;
  }
}

void featuresGetTempCommand(char *parameter, char *reply, bool &booleanReply) {
  switch (parameter[1]) {
#if FEATURE1_PURPOSE == DEW_HEATER
    case '1': dtostrf(ambient.getFeatureTemperature(0),3,1,reply); booleanReply=false; break;
#endif
#if FEATURE2_PURPOSE == DEW_HEATER
    case '2': dtostrf(ambient.getFeatureTemperature(1),3,1,reply); booleanReply=false; break;
#endif
#if FEATURE3_PURPOSE == DEW_HEATER
    case '3': dtostrf(ambient.getFeatureTemperature(2),3,1,reply); booleanReply=false; break;
#endif
#if FEATURE4_PURPOSE == DEW_HEATER
    case '4': dtostrf(ambient.getFeatureTemperature(3),3,1,reply); booleanReply=false; break;
#endif
#if FEATURE5_PURPOSE == DEW_HEATER
    case '5': dtostrf(ambient.getFeatureTemperature(4),3,1,reply); booleanReply=false; break;
#endif
#if FEATURE6_PURPOSE == DEW_HEATER
    case '6': dtostrf(ambient.getFeatureTemperature(5),3,1,reply); booleanReply=false; break;
#endif
#if FEATURE7_PURPOSE == DEW_HEATER
    case '7': dtostrf(ambient.getFeatureTemperature(6),3,1,reply); booleanReply=false; break;
#endif
#if FEATURE8_PURPOSE == DEW_HEATER
    case '8': dtostrf(ambient.getFeatureTemperature(7),3,1,reply); booleanReply=false; break;
#endif
    default: commandError=CE_CMD_UNKNOWN;
  }
}

// :SXX[n],V[Z][S][v]#
// for example :SXX1,V1#  :SXX1,Z0.5#
void featuresSetCommand(char *parameter) {
#if FEATURE1_PURPOSE != OFF || FEATURE2_PURPOSE != OFF || FEATURE3_PURPOSE != OFF || FEATURE4_PURPOSE != OFF || FEATURE5_PURPOSE != OFF || FEATURE6_PURPOSE != OFF || FEATURE7_PURPOSE != OFF || FEATURE8_PURPOSE != OFF
  char* conv_end;
  double f=strtod(&parameter[4],&conv_end);
    if (&parameter[4] == conv_end) commandError=CE_PARAM_FORM; else {
    long i = lround(f);

    switch (parameter[1]) {
#if FEATURE1_PURPOSE != OFF
      case '1':
        switch (parameter[3]) {
  #if FEATURE1_PURPOSE == SWITCH
          case 'V': if (i >= 0 && i <= 1) { if (FEATURE2_PIN >= 0 && FEATURE2_PIN <= 255) digitalWrite((int)FEATURE1_PIN,i==0?LOW:HIGH); else ambient.setDS2413State(0,i==0?0:1); } else commandError=CE_PARAM_RANGE; break;
  #elif FEATURE1_PURPOSE == ANALOG
          case 'V': if (i >= 0 && i <= 255) { if (FEATURE2_PIN >= 0 && FEATURE2_PIN <= 255) analogWrite((int)FEATURE1_PIN,l); } else commandError=CE_PARAM_RANGE; break;
  #elif FEATURE1_PURPOSE == DEW_HEATER
          case 'V': if (i >= 0 && i <= 1) dewHeater1.enable(i); else commandError=CE_PARAM_RANGE; break;
          case 'Z': if (f >= -5.0 && f <= 20.0) dewHeater1.setZero(f); else commandError=CE_PARAM_RANGE; break;
          case 'S': if (f >= -5.0 && f <= 20.0) dewHeater1.setSpan(f); else commandError=CE_PARAM_RANGE; break;
  #endif
        } break;
#endif

#if FEATURE2_PURPOSE != OFF
      case '2':
        switch (parameter[3]) {
  #if FEATURE2_PURPOSE == SWITCH
          case 'V': if (i >= 0 && i <= 1) { if (FEATURE2_PIN >= 0 && FEATURE2_PIN <= 255) digitalWrite((int)FEATURE2_PIN,i==0?LOW:HIGH); else ambient.setDS2413State(1,i==0?0:1); } else commandError=CE_PARAM_RANGE; break;
  #elif FEATURE2_PURPOSE == ANALOG
          case 'V': if (i >= 0 && i <= 255) { if (FEATURE2_PIN >= 0 && FEATURE2_PIN <= 255) analogWrite((int)FEATURE2_PIN,l); } else commandError=CE_PARAM_RANGE; break;
  #elif FEATURE2_PURPOSE == DEW_HEATER
          case 'V': if (i >= 0 && i <= 1) dewHeater2.enable(i); else commandError=CE_PARAM_RANGE; break;
          case 'Z': if (f >= -5.0 && f <= 20.0) dewHeater2.setZero(f); else commandError=CE_PARAM_RANGE; break;
          case 'S': if (f >= -5.0 && f <= 20.0) dewHeater2.setSpan(f); else commandError=CE_PARAM_RANGE; break;
  #endif
        } break;
#endif

#if FEATURE3_PURPOSE != OFF
      case '3':
        switch (parameter[3]) {
  #if FEATURE3_PURPOSE == SWITCH
          case 'V': if (i >= 0 && i <= 1) { if (FEATURE3_PIN >= 0 && FEATURE3_PIN <= 255) digitalWrite((int)FEATURE3_PIN,i==0?LOW:HIGH); else ambient.setDS2413State(2,i==0?0:1); } else commandError=CE_PARAM_RANGE; break;
  #elif FEATURE3_PURPOSE == ANALOG
          case 'V': if (i >= 0 && i <= 255) { if (FEATURE3_PIN >= 0 && FEATURE3_PIN <= 255) analogWrite((int)FEATURE3_PIN,l); } else commandError=CE_PARAM_RANGE; break;
  #elif FEATURE3_PURPOSE == DEW_HEATER
          case 'V': if (i >= 0 && i <= 1) dewHeater3.enable(i); else commandError=CE_PARAM_RANGE; break;
          case 'Z': if (f >= -5.0 && f <= 20.0) dewHeater3.setZero(f); else commandError=CE_PARAM_RANGE; break;
          case 'S': if (f >= -5.0 && f <= 20.0) dewHeater3.setSpan(f); else commandError=CE_PARAM_RANGE; break;
  #endif
        } break;
#endif

#if FEATURE4_PURPOSE != OFF
      case '4':
        switch (parameter[3]) {
  #if FEATURE4_PURPOSE == SWITCH
          case 'V': if (i >= 0 && i <= 1) { if (FEATURE4_PIN >= 0 && FEATURE4_PIN <= 255) digitalWrite((int)FEATURE4_PIN,i==0?LOW:HIGH); else ambient.setDS2413State(3,i==0?0:1); } else commandError=CE_PARAM_RANGE; break;
  #elif FEATURE4_PURPOSE == ANALOG
          case 'V': if (i >= 0 && i <= 255) { if (FEATURE4_PIN >= 0 && FEATURE4_PIN <= 255) analogWrite((int)FEATURE4_PIN,l); } else commandError=CE_PARAM_RANGE; break;
  #elif FEATURE4_PURPOSE == DEW_HEATER
          case 'V': if (i >= 0 && i <= 1) dewHeater4.enable(i); else commandError=CE_PARAM_RANGE; break;
          case 'Z': if (f >= -5.0 && f <= 20.0) dewHeater4.setZero(f); else commandError=CE_PARAM_RANGE; break;
          case 'S': if (f >= -5.0 && f <= 20.0) dewHeater4.setSpan(f); else commandError=CE_PARAM_RANGE; break;
  #endif
        } break;
#endif

#if FEATURE5_PURPOSE != OFF
      case '5':
        switch (parameter[3]) {
  #if FEATURE5_PURPOSE == SWITCH
          case 'V': if (i >= 0 && i <= 1) { if (FEATURE5_PIN >= 0 && FEATURE5_PIN <= 255) digitalWrite((int)FEATURE5_PIN,i==0?LOW:HIGH); else ambient.setDS2413State(4,i==0?0:1); } else commandError=CE_PARAM_RANGE; break;
  #elif FEATURE5_PURPOSE == ANALOG
          case 'V': if (i >= 0 && i <= 255) { if (FEATURE5_PIN >= 0 && FEATURE5_PIN <= 255) analogWrite((int)FEATURE5_PIN,l); } else commandError=CE_PARAM_RANGE; break;
  #elif FEATURE5_PURPOSE == DEW_HEATER
          case 'V': if (i >= 0 && i <= 1) dewHeater5.enable(i); else commandError=CE_PARAM_RANGE; break;
          case 'Z': if (f >= -5.0 && f <= 20.0) dewHeater5.setZero(f); else commandError=CE_PARAM_RANGE; break;
          case 'S': if (f >= -5.0 && f <= 20.0) dewHeater5.setSpan(f); else commandError=CE_PARAM_RANGE; break;
  #endif
        } break;
#endif

#if FEATURE6_PURPOSE != OFF
      case '6':
        switch (parameter[3]) {
  #if FEATURE6_PURPOSE == SWITCH
          case 'V': if (i >= 0 && i <= 1) { if (FEATURE6_PIN >= 0 && FEATURE6_PIN <= 255) digitalWrite((int)FEATURE6_PIN,i==0?LOW:HIGH); else ambient.setDS2413State(5,i==0?0:1); } else commandError=CE_PARAM_RANGE; break;
  #elif FEATURE6_PURPOSE == ANALOG
          case 'V': if (i >= 0 && i <= 255) { if (FEATURE6_PIN >= 0 && FEATURE6_PIN <= 255) analogWrite((int)FEATURE6_PIN,l); } else commandError=CE_PARAM_RANGE; break;
  #elif FEATURE6_PURPOSE == DEW_HEATER
          case 'V': if (i >= 0 && i <= 1) dewHeater6.enable(i); else commandError=CE_PARAM_RANGE; break;
          case 'Z': if (f >= -5.0 && f <= 20.0) dewHeater6.setZero(f); else commandError=CE_PARAM_RANGE; break;
          case 'S': if (f >= -5.0 && f <= 20.0) dewHeater6.setSpan(f); else commandError=CE_PARAM_RANGE; break;
  #endif
        } break;
#endif

#if FEATURE7_PURPOSE != OFF
      case '7':
        switch (parameter[3]) {
  #if FEATURE7_PURPOSE == SWITCH
          case 'V': if (i >= 0 && i <= 1) { if (FEATURE7_PIN >= 0 && FEATURE7_PIN <= 255) digitalWrite((int)FEATURE7_PIN,i==0?LOW:HIGH); else ambient.setDS2413State(6,i==0?0:1); } else commandError=CE_PARAM_RANGE; break;
  #elif FEATURE7_PURPOSE == ANALOG
          case 'V': if (i >= 0 && i <= 255) { if (FEATURE7_PIN >= 0 && FEATURE7_PIN <= 255) analogWrite((int)FEATURE7_PIN,l); } else commandError=CE_PARAM_RANGE; break;
  #elif FEATURE7_PURPOSE == DEW_HEATER
          case 'V': if (i >= 0 && i <= 1) dewHeater7.enable(i); else commandError=CE_PARAM_RANGE; break;
          case 'Z': if (f >= -5.0 && f <= 20.0) dewHeater7.setZero(f); else commandError=CE_PARAM_RANGE; break;
          case 'S': if (f >= -5.0 && f <= 20.0) dewHeater7.setSpan(f); else commandError=CE_PARAM_RANGE; break;
  #endif
        } break;
#endif

#if FEATURE8_PURPOSE != OFF
      case '8':
        switch (parameter[3]) {
  #if FEATURE8_PURPOSE == SWITCH
          case 'V': if (i >= 0 && i <= 1) { if (FEATURE8_PIN >= 0 && FEATURE8_PIN <= 255) digitalWrite((int)FEATURE8_PIN,i==0?LOW:HIGH); else ambient.setDS2413State(7,i==0?0:1); } else commandError=CE_PARAM_RANGE; break;
  #elif FEATURE8_PURPOSE == ANALOG
          case 'V': if (i >= 0 && i <= 255) { if (FEATURE8_PIN >= 0 && FEATURE8_PIN <= 255) analogWrite((int)FEATURE8_PIN,l); } else commandError=CE_PARAM_RANGE; break;
  #elif FEATURE8_PURPOSE == DEW_HEATER
          case 'V': if (i >= 0 && i <= 1) dewHeater8.enable(i); else commandError=CE_PARAM_RANGE; break;
          case 'Z': if (f >= -5.0 && f <= 20.0) dewHeater8.setZero(f); else commandError=CE_PARAM_RANGE; break;
          case 'S': if (f >= -5.0 && f <= 20.0) dewHeater8.setSpan(f); else commandError=CE_PARAM_RANGE; break;
  #endif
        } break;
#endif
      default: commandError=CE_CMD_UNKNOWN;
    }
  }
#endif
}
