// -------------------------------------------------------------------------------------------------
// Functions to handle periodic error correction

// enables code to clean-up PEC readings after record (use PECprep or a spreadsheet to fix readings otherwise)
// this cleans up any tracking rate variations that would be introduced by recording more guiding corrections to either the east or west, default=_ON
#define PEC_CLEANUP_ON

#ifdef PEC_SENSE_OFF
  boolean wormSensedFirst=true;
#else
  boolean wormSensedFirst=false;
#endif

long pecRecordStopTime  = 0;
long wormRotationPos    = 0;
long lastWormRotationPos=-1;

void pec() {
  // PEC is only active when we're tracking at the sidereal rate with a guide rate that makes sense

  // keep track of our current step position, and when the step position on the worm wraps during playback
  cli(); long pecPos=(long)targetAxis1.part.m; sei();
  
  #ifdef PEC_SENSE_OFF
    wormSensedFirst=true;
  #endif
  #ifdef PEC_SENSE
    // for analog sense, with 60 second delay before redetect
    long dist; if (wormSensePos>pecPos) dist=wormSensePos-pecPos; else dist=pecPos-wormSensePos;
    if ((dist>StepsPerSecondAxis1*60.0) && (pecAnalogValue>PEC_SENSE)) {
      wormSensePos=pecPos;
      wormSensedAgain=true;
      wormSensedFirst=true;
      pecBufferStart=true;
    } else pecBufferStart=false;
  #endif
  #if defined(PEC_SENSE_ON) || defined(PEC_SENSE_PULLUP)
    // for digital sense, with 60 second delay before redetect
    long dist; if (wormSensePos>pecPos) dist=wormSensePos-pecPos; else dist=pecPos-wormSensePos;
    if ((dist>StepsPerSecondAxis1*60.0) && (digitalRead(PecPin)==PEC_SENSE_STATE)) {
      wormSensePos=pecPos;
      wormSensedAgain=true;
      wormSensedFirst=true;
      pecBufferStart=true;
    } else pecBufferStart=false;
  #endif

  if (pecStatus==IgnorePEC) { pecTimerRateAxis1=0.0; return; }
  if (!wormSensedFirst) return;

  // worm step position corrected for any index found
  lastWormRotationPos=wormRotationPos;
  wormRotationPos=(pecPos-wormSensePos);
  while (wormRotationPos>=(long)StepsPerWormRotationAxis1) wormRotationPos-=(long)StepsPerWormRotationAxis1;
  while (wormRotationPos<0) wormRotationPos+=(long)StepsPerWormRotationAxis1;
  // "soft" PEC index sense
  #ifdef PEC_SENSE_OFF
  if ((wormRotationPos-lastWormRotationPos)<0) pecBufferStart=true; else pecBufferStart=false;
  #endif
    
  // handle playing back and recording PEC
  cli(); long t=lst; sei();

  // start playing PEC
  if (pecStatus==ReadyPlayPEC) {
    // makes sure the index is at the start of a second before resuming play
    if ((long)fmod(wormRotationPos,StepsPerSecondAxis1)==0) {
      pecStatus=PlayPEC;
      pecIndex=wormRotationPos/StepsPerSecondAxis1;

      // playback starts now
      PecSiderealTimer=t;
    }
  } else
  // start recording PEC
  if (pecStatus==ReadyRecordPEC) {
    if ((long)fmod(wormRotationPos,StepsPerSecondAxis1)==0) {
      pecStatus=RecordPEC;
      pecIndex=wormRotationPos/StepsPerSecondAxis1;
      pecRecorded=false;

      // recording starts now
      PecSiderealTimer=t;
      pecRecordStopTime=PecSiderealTimer+(long)SecondsPerWormRotationAxis1*100L;
      accPecGuideHA.fixed=0;
    }
  } else
  // and once the PEC data is all stored, indicate that it's valid and start using it
  if ((pecStatus==RecordPEC) && (t-pecRecordStopTime>0)) {
    pecStatus=PlayPEC;
    pecRecorded=true;
    pecFirstRecord=false;
#ifdef PEC_CLEANUP_ON
    cleanupPec();
#endif
  }

  // reset the buffer index to match the worm index
  if (pecBufferStart && (pecStatus!=RecordPEC)) { pecIndex=0; PecSiderealTimer=t; }
  // Increment the PEC index once a second and make it go back to zero when the worm finishes a rotation
  if (t-PecSiderealTimer>99) {
    PecSiderealTimer=t; pecIndex=(pecIndex+1)%SecondsPerWormRotationAxis1;
  }
  pecIndex1=pecIndex; if (pecIndex1<0) pecIndex1+=SecondsPerWormRotationAxis1; if (pecIndex1>=SecondsPerWormRotationAxis1) pecIndex1-=SecondsPerWormRotationAxis1;

  accPecGuideHA.fixed+=guideAxis1.fixed;
  
  // falls in whenever the pecIndex changes, which is once a sidereal second
  if (pecIndex1!=lastPecIndex) {
    lastPecIndex=pecIndex1;

    // assume no change to tracking rate
    pecTimerRateAxis1=0.0;

    if (pecStatus==RecordPEC) {
      // save the correction as 1 of 3 weighted average
      int l=round(fixedToDouble(accPecGuideHA));
      if (l<-StepsPerSecondAxis1) l=-StepsPerSecondAxis1; if (l>StepsPerSecondAxis1) l=StepsPerSecondAxis1;   // +/-1 sidereal rate range for corrections
      if (l<-127) l=-127; if (l>127) l=127;                                                                   // prevent overflow if StepsPerSecondAxis1>127
      if (!pecFirstRecord) l=(l+((int)pecBuffer[pecIndex1]-128)*2)/3; 
      pecBuffer[pecIndex1]=l+128;  // save the correction
      accPecGuideHA.part.m-=l;     // remove from the accumulator
    }

    if (pecStatus==PlayPEC) {
      // pecIndex2 adjusts one second before the value was recorded, an estimate of the latency between image acquisition and response
      // if sending values directly to OnStep from PECprep, etc. be sure to account for this
      int pecIndex2=pecIndex1-1; if (pecIndex2<0) pecIndex2+=SecondsPerWormRotationAxis1;
      // number of steps ahead or behind for this 1 second slot, up to +/-127
      int l=pecBuffer[pecIndex2]-128;
      if (l>StepsPerSecondAxis1) l=StepsPerSecondAxis1; if (l<-StepsPerSecondAxis1) l=-StepsPerSecondAxis1;
      pecTimerRateAxis1=((double)l/StepsPerSecondAxis1);
    }
  }
}
 
void disablePec() {
  // give up recording if we stop tracking at the sidereal rate
  if (pecStatus==RecordPEC)  { pecStatus=IgnorePEC; pecTimerRateAxis1=0.0; } // don't zero the PEC offset, we don't want things moving and it really doesn't matter 
  // get ready to re-index when tracking comes back
  if (pecStatus==PlayPEC)  { pecStatus=ReadyPlayPEC; pecTimerRateAxis1=0.0; } 
}

void cleanupPec() {
  // low pass filter ----------------------------------------------------------
  int j,J1,J4,J9,J17;
  for (int scc=0+3; scc<SecondsPerWormRotationAxis1+3; scc++) {
    j=pecBuffer[((scc)%SecondsPerWormRotationAxis1)]-128;

    J1=(int)round((float)j*0.01);
    J4=(int)round((float)j*0.04);
    J9=(int)round((float)j*0.09);
    J17=(int)round((float)j*0.17);
    pecBuffer[((scc-4)%SecondsPerWormRotationAxis1)]=(((int)pecBuffer[((scc-4)%SecondsPerWormRotationAxis1)]-128)+J1)+128;
    pecBuffer[((scc-3)%SecondsPerWormRotationAxis1)]=(((int)pecBuffer[((scc-3)%SecondsPerWormRotationAxis1)]-128)+J4)+128;
    pecBuffer[((scc-2)%SecondsPerWormRotationAxis1)]=(((int)pecBuffer[((scc-2)%SecondsPerWormRotationAxis1)]-128)+J9)+128;
    pecBuffer[((scc-1)%SecondsPerWormRotationAxis1)]=(((int)pecBuffer[((scc-1)%SecondsPerWormRotationAxis1)]-128)+J17)+128;
    pecBuffer[((scc  )%SecondsPerWormRotationAxis1)]=(((int)pecBuffer[((scc  )%SecondsPerWormRotationAxis1)]-128)-(J17+J17+J9+J9+J4+J4+J1+J1))+128;
    pecBuffer[((scc+1)%SecondsPerWormRotationAxis1)]=(((int)pecBuffer[((scc+1)%SecondsPerWormRotationAxis1)]-128)+J17)+128;
    pecBuffer[((scc+2)%SecondsPerWormRotationAxis1)]=(((int)pecBuffer[((scc+2)%SecondsPerWormRotationAxis1)]-128)+J9)+128;
    pecBuffer[((scc+3)%SecondsPerWormRotationAxis1)]=(((int)pecBuffer[((scc+3)%SecondsPerWormRotationAxis1)]-128)+J4)+128;
    pecBuffer[((scc+4)%SecondsPerWormRotationAxis1)]=(((int)pecBuffer[((scc+4)%SecondsPerWormRotationAxis1)]-128)+J1)+128;
  }
  
  // linear regression ----------------------------------------------------------
  // the number of steps added should equal the number of steps subtracted (from the cycle)
  // first, determine how far we've moved ahead or backward in steps
  long sum_pec=0; for (int scc=0; scc<SecondsPerWormRotationAxis1; scc++) { sum_pec+=(int)pecBuffer[scc]-128; }

  // this is the correction coefficient for a given location in the sequence
  double Ccf = (double)sum_pec/(double)SecondsPerWormRotationAxis1;

  // now, apply the correction to the sequence to make the PEC adjustments null out
  // this process was simulated in a spreadsheet and the roundoff error might leave us at +/- a step which is tacked on at the beginning
  long lp2=0; sum_pec=0; 
  for (int scc=0; scc<SecondsPerWormRotationAxis1; scc++) {
    // the correction, "now"
    long lp1=lround(-(double)scc*Ccf);
    
    // if the correction increases or decreases then add or subtract that many steps
    pecBuffer[scc]=(int)pecBuffer[scc]+(lp1-lp2);

    // sum the values for a final adjustment, if necessary
    sum_pec+=(int)pecBuffer[scc]-128;     
    lp2=lp1;
  }
  pecBuffer[0]-=sum_pec;

  // a reality check, make sure the buffer data looks good, if not forget it
  if ((sum_pec>2) || (sum_pec<-2)) { pecRecorded=false; pecStatus=IgnorePEC; }
}

