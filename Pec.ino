// PEC ---------------------------------------------------------------------------------------------
// functions to handle periodic error correction

int LastPecPinState=PEC_SENSE_STATE;
#if defined(PEC_SENSE_ON) || defined(PEC_SENSE)
boolean initPecIndex=true;
#else
boolean initPecIndex=false;
#endif
void Pec() {
  // PEC is only active when we're tracking at the sidereal rate with a guide rate that makes sense

  // keep track of our current step position, and when the step position on the worm wraps during playback
  lastWormRotationStepPos = wormRotationStepPos;
  cli(); long posPEC=(long int)targetHA.part.m; sei();
  
  // where we're at (HA), must always be positive, so add 360 degrees (in steps)
  posPEC=posPEC+StepsPerDegreeHA*360L;
  wormRotationStepPos =(posPEC-PECindex_record) % StepsPerWormRotation;
  
  // handle playing back and recording PEC
  if (PECstatus!=IgnorePEC) {
    // start playing PEC
    if (PECstatus==ReadyPlayPEC) {
      // approximate, might be off by nearly a second, this makes sure that the index is at the start of a second before resuming play
      if ((long)fmod(wormRotationStepPos,StepsPerSecond)==0) {
        PECstatus=PlayPEC;
        PECindex=wormRotationStepPos/StepsPerSecond; 
        // sum corrections to this point
        long m=0; for (int l=0; l<PECindex; l++) {
          long l1=l-PECindex_sense; if (l1<0) l1+=SecondsPerWormRotation; if (l1>SecondsPerWormRotation-1) l1-=SecondsPerWormRotation;
          m+=PEC_buffer[l1]-128; 
        }
        // move to the corrected location, this might take a few seconds and can be abrupt (after goto's or when unparking)
        cli(); PEC_HA = m; sei(); 
        PECstartDelta=abs(m)/StepsPerSecond; // in units of seconds, ahead or behind, all that matters is how long the rate has to be increased for

        // playback starts now
        cli(); PecSiderealTimer = lst; sei();
      }
    }

    // at the start of a worm cycle (if wormRotationStepPos rolled over)
    // this only works because targetHA can't move backward while tracking and guiding at +/- 1x sidereal
    // it also works while <=1X guiding and playing PEC because the PEC corrections don't get counted in the targetHA location
    if (lastWormRotationStepPos>wormRotationStepPos+StepsPerSecond) {
      // start recording PEC
      if (PECstatus==ReadyRecordPEC) {
        PECstatus=RecordPEC;
        PECrecorded=false;
      } else
      // and once the PEC data is all stored, indicate that it's valid and start using it
      if (PECstatus==RecordPEC) {
        PECstatus=PlayPEC;
        PECrecorded=true;
        PECfirstRecord=false;
#ifdef PEC_CLEANUP_ON
        // the number of steps added should equal the number of steps subtracted (from the cycle)
        // first, determine how far we've moved ahead or backward in steps
        long sum_pec=0; for (int scc=0; scc<SecondsPerWormRotation; scc++) { sum_pec+=(int)PEC_buffer[scc]-128; }

        // this is the correction coefficient for a given location in the sequence
        double Ccf = (double)sum_pec/(double)SecondsPerWormRotation;

        // now, apply the correction to the sequence to make the PEC adjustments null out
        // this process was simulated in a spreadsheet and the roundoff error might leave us at +/- a step which is tacked on at the beginning
        long lp2=0; sum_pec=0; 
        for (int scc=0; scc<SecondsPerWormRotation; scc++) {
          // the correction, "now"
          long lp1=lround(-scc*Ccf);
          
          // if the correction increases or decreases then add or subtract that many steps
          PEC_buffer[scc]=(int)PEC_buffer[scc]+(lp1-lp2);

          // sum the values for a final adjustment, if necessary
          sum_pec+=(int)PEC_buffer[scc]-128;     
          lp2=lp1;
        }
        PEC_buffer[0]-=sum_pec;

        // a reality check, make sure the buffer data looks good, if not forget it
        if ((sum_pec>2) || (sum_pec<-2)) { PECrecorded=false; PECstatus=IgnorePEC; }
#endif
      }
      
      // zero the accululators, the index timer, also the index
      accPecGuideHA = 0;
      cli(); PecSiderealTimer = lst-1; sei();  // keeps PECindex from advancing immediately
      PECindex = 0; lastPECindex = -1;         // starts record/playback now
    #if defined(PEC_SENSE_ON) || defined(PEC_SENSE)
      if (next_PECindex_sense>=0) { PECindex_sense=next_PECindex_sense; next_PECindex_sense=-1; }
    #endif
    }

    // Increment the PEC index once a second and make it go back to zero when the worm finishes a rotation
    cli(); long t=lst; sei(); if (t-PecSiderealTimer>99) { PecSiderealTimer=t; PECindex=(PECindex+1)%SecondsPerWormRotation; }
    PECindex1=(PECindex-PECindex_sense); if (PECindex1<0) PECindex1+=SecondsPerWormRotation; if (PECindex1>SecondsPerWormRotation-1) PECindex1-=SecondsPerWormRotation;

  #ifdef PEC_SENSE_ON
    // if the HALL sensor (etc.) has just arrived at the index and it's been more than 60 seconds since
    // it was there before, set this as the next start of PEC playback/recording
    int PecPinState=digitalRead(PecPin);
    cli();
    if (((LastPecPinState!=PEC_SENSE_STATE)) && (PecPinState==PEC_SENSE_STATE) && (lst-PECtime_lastSense>6000)) {
      PECindexDetected=true;
      PECtime_lastSense=lst;
      next_PECindex_sense=PECindex;
      if (initPecIndex) { PECindex_record=posPEC; next_PECindex_sense=0; initPecIndex=false; }
    }
    sei();
    LastPecPinState=PecPinState;
  #endif

  #ifdef PEC_SENSE
    // as above except for Analog sense
    cli();
    if ((PECav>PEC_SENSE) && (lst-PECtime_lastSense>6000)) {
      PECindexDetected=true;
      PECtime_lastSense=lst;
      next_PECindex_sense=PECindex;
      if (initPecIndex) { PECindex_record=posPEC; next_PECindex_sense=0; initPecIndex=false; }
    }
    sei();
  #endif

    accPecGuideHA+=guideHA;
  } else {
    // if we're ignoring PEC, zero the offset and keep it that way
    cli(); PEC_HA=0; sei(); 
    pecTimerRateHA=0;
  }

  // falls in whenever the PECindex changes, which is once a sidereal second
  if (PECindex1!=lastPECindex) {
    lastPECindex=PECindex1;

    // assume no change to tracking rate
    pecTimerRateHA=0;

    if (PECstatus==RecordPEC) {
      // save the correction 1:2 weighted average
      int l=accPecGuideHA;
      if (l<-StepsPerSecond) l=-StepsPerSecond; if (l>StepsPerSecond) l=StepsPerSecond;   // +/-1 sidereal rate range for corrections
      if (l<-127) l=-127; if (l>127) l=127;                                               // prevent overflow if StepsPerSecond>127
      if (!PECfirstRecord) l=(l+((int)PEC_buffer[PECindex1]-128)*2)/3; 
      PEC_buffer[PECindex1]=l+128;  // save the correction
      accPecGuideHA=0;              // and clear the accumulator
    }

    if (!initPecIndex) { // wait for the index, if we have one
      if (PECstatus==PlayPEC) {
        // PECindex2 adjusts one second before the value was recorded, an estimate of the latency between image acquisition and response
        // if sending values directly to OnStep from PECprep, etc. be sure to account for this
        int PECindex2=PECindex1-1; if (PECindex2<0) PECindex2+=SecondsPerWormRotation;
        // accPecPlayHA play back speed can be +/-1 of sidereal
        // PEC_Skip is the number of ticks between the added (or skipped) steps
        int l=PEC_buffer[PECindex2]-128;
        if (l>StepsPerSecond) l=StepsPerSecond; if (l<-StepsPerSecond) l=-StepsPerSecond;
  
        if (PECstartDelta>0) { 
          // when (re)starting PEC not worried about how smooth the play back is, this will be over in a moment just run fast to get there in position
          // run the PEC rate at another +/-5x to cover any corrections that should have been applied up to this point in the worm cycle
          PECstartDelta-=30;
          pecTimerRateHA+=5; 
          pstep.fixed=doubleToFixed(0.0); 
        } else {
          // otherwise set the rates to playback the correct number of steps per second
          pecTimerRateHA=(l/StepsPerSecond);
          pstep.fixed=doubleToFixed(l/100.0);
        }
      }
    }
  }
}
 
void disablePec() {
  // give up recording if we stop tracking at the sidereal rate
  if (PECstatus==RecordPEC)  { PECstatus=IgnorePEC; pecTimerRateHA=0; } // don't zero the PEC offset, we don't want things moving and it really doesn't matter 
  // get ready to re-index when tracking comes back
  if (PECstatus==PlayPEC)  { PECstatus=ReadyPlayPEC; pecTimerRateHA=0; cli(); PEC_HA=0; sei(); } 
}
