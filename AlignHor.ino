// -----------------------------------------------------------------------------------
// GEOMETRIC ALIGN FOR ALT/AZ
//
// by Howard Dutton
//
// Copyright (C) 2012 to 2018 Howard Dutton
//

// -----------------------------------------------------------------------------------
// ADVANCED GEOMETRIC ALIGN FOR ALT/AZ MOUNTS (GOTO ASSIST)

#ifdef MOUNT_TYPE_ALTAZM
#define GOTO_ASSIST_DEBUG_OFF

// Initialize
void TGeoAlignH::init() {
  avgAlt=0.0;
  avgAzm=0.0;
  
  ax1Cor=0;  // align internal index for Axis1
  ax2Cor=0;  // align internal index for Axis2
  altCor=0;  // for geometric coordinate correction/align, - is below the Zenith, + above
  azmCor=0;  // - is right of the Zenith, + is left
  doCor =0;  // altitude axis/optics orthogonal correction
  pdCor =0;  // altitude axis/Azimuth orthogonal correction
  dfCor =0;  // altitude axis axis flex
  tfCor =0;  // tube flex

  geo_ready=false;
}

// remember the alignment between sessions
void TGeoAlignH::readCoe() {
  ax1Cor=nv.readFloat(EE_ax1Cor);
  ax2Cor=nv.readFloat(EE_ax2Cor);
  dfCor=nv.readFloat(EE_dfCor);  // dfCor is ffCor for fork mounts
  tfCor=nv.readFloat(EE_tfCor);
  doCor=nv.readFloat(EE_doCor);
  pdCor=nv.readFloat(EE_pdCor);
  altCor=nv.readFloat(EE_altCor);
  azmCor=nv.readFloat(EE_azmCor);
}

void TGeoAlignH::writeCoe() {
  nv.writeFloat(EE_ax1Cor,ax1Cor);
  nv.writeFloat(EE_ax2Cor,ax2Cor);
  nv.writeFloat(EE_dfCor,dfCor);  // dfCor is ffCor for fork mounts
  nv.writeFloat(EE_tfCor,tfCor);
  nv.writeFloat(EE_doCor,doCor);
  nv.writeFloat(EE_pdCor,pdCor);
  nv.writeFloat(EE_altCor,altCor);
  nv.writeFloat(EE_azmCor,azmCor);
}

// Status
bool TGeoAlignH::isReady() {
  return geo_ready;
}

// I=1 for 1st star, I=2 for 2nd star, I=3 for 3rd star
// N=total number of stars for this align (1 to 9)
// RA, Dec (all in degrees)
bool TGeoAlignH::addStar(int I, int N, double RA, double Dec) {
  double a,z;
  equToHor(haRange(LST()*15.0-RA),Dec,&a,&z);

  // First star, just sync
  if (I==1) { if (syncEqu(RA,Dec)!=GOTO_ERR_NONE) return false; }

  mount[I-1].azm=getInstrAxis1();
  mount[I-1].alt=getInstrAxis2();
  horToEqu(mount[I-1].alt,mount[I-1].azm,&mount[I-1].ha,&mount[I-1].dec);
  mount[I-1].azm=mount[I-1].azm/Rad;
  mount[I-1].alt=mount[I-1].alt/Rad;
  mount[I-1].ha=degRange(mount[I-1].ha)/Rad;
  mount[I-1].dec=mount[I-1].dec/Rad;

  actual[I-1].ha =haRange(LST()*15.0-RA);
  actual[I-1].dec=Dec;
  equToHor(actual[I-1].ha,actual[I-1].dec,&actual[I-1].alt,&actual[I-1].azm);
  actual[I-1].alt=actual[I-1].alt/Rad;
  actual[I-1].azm=actual[I-1].azm/Rad;
  actual[I-1].ha =degRange(actual[I-1].ha)/Rad;
  actual[I-1].dec=actual[I-1].dec/Rad;

  if (getInstrPierSide()==PierSideWest) { actual[I-1].side=-1; mount[I-1].side=-1; } else
  if (getInstrPierSide()==PierSideEast) { actual[I-1].side=1; mount[I-1].side=1; } else { actual[I-1].side=0; mount[I-1].side=0; }

  // two or more stars and finished
  if ((I>=2) && (I==N)) model(N);

  return true;
}

// kick off modeling
void TGeoAlignH::model(int n) {
  static bool busy=false;
  static int numStars=0;
  if (busy) return;                                                           // busy
  if (n>0) { numStars=n; return; }                                            // command
  if (numStars>0) { busy=true; autoModel(numStars); busy=false; numStars=0; } // waiting to solve
}

// returns the correction to be added to the requested RA,Dec to yield the actual RA,Dec that we will arrive at
void TGeoAlignH::correct(double azm, double alt, double pierSide, double sf, double _deo, double _pd, double _pz, double _pe, double _df, double _ff, double _tf, double *z1, double *a1) {
  double DO1,DOh;
  double PD,PDh;
  double PZ,PA;
  double DF,DFd,TF,FF,FFd,TFh,TFd;

  double cosAlt=cos(alt);
  double tanAlt=tan(alt);
  double sinAzm=sin(azm);
  double cosAzm=cos(azm);

// ------------------------------------------------------------
// A. Misalignment due to tube/optics not being perp. to Dec axis
// negative numbers are further (S) from the NCP, swing to the
// equator and the effect on declination is 0. At the SCP it
// becomes a (N) offset.  Unchanged with meridian flips.
  DO1 =_deo*sf;
// works on HA.  meridian flips effect this in HA
  DOh = DO1*(1.0/cosAlt)*pierSide;

// ------------------------------------------------------------
// B. Misalignment, Declination axis relative to Polar axis
// expressed as a correction to where the Polar axis is pointing
// negative numbers are further (S) from the NCP, swing to the
// equator and the effect on declination is 0.
// At the SCP it is, again, a (S) offset
  PD  =_pd*sf;
// works on HA.
  PDh = -PD*tanAlt*pierSide;

// ------------------------------------------------------------
// Misalignment, relative to NCP
// negative numbers are east of the pole
// C. polar left-right misalignment
  PZ  =_pz*sf;
// D. negative numbers are below the pole
// polar below-above misalignment
  PA  =_pe*sf;

// ------------------------------------------------------------
// Axis flex
  DF  =_df*sf;
  DFd =-DF*(cosLat*cosAzm+sinLat*tanAlt);

// ------------------------------------------------------------
// Fork flex
  FF  =_ff*sf;
  FFd =FF*cosAzm;

// ------------------------------------------------------------
// Optical axis sag
  TF  =_tf*sf;

  TFh =TF*(cosLat*sinAzm*(1.0/cosAlt));
  TFd =TF*(cosLat*cosAzm-sinLat*cosAlt);

// ------------------------------------------------------------
  *z1  =(-PZ*cosAzm*tanAlt + PA*sinAzm*tanAlt + DOh +  PDh +       TFh);
  *a1  =(+PZ*sinAzm        + PA*cosAzm              +  DFd + FFd + TFd);
}

void TGeoAlignH::do_search(double sf, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9)
{
  long l,

  _deo_m,_deo_p,
  _pd_m,_pd_p,
  _pz_m,_pz_p,
  _pe_m,_pe_p,
  _df_m,_df_p,
  _tf_m,_tf_p,
  _ff_m,_ff_p,
  _oh_m,_oh_p,
  _od_m,_od_p,
  
  _deo,_pd,_pz,_pe, _df,_tf,_ff, _ode,_ohe;

  double sf1=sf/(3600.0*Rad);

  // search
  // set Parameter Space
  _deo_m=-p1+round(best_deo/sf); _deo_p=p1+round(best_deo/sf);
  _pd_m =-p2+round(best_pd/sf);  _pd_p=p2+round(best_pd/sf);
  _pz_m =-p3+round(best_pz/sf);  _pz_p=p3+round(best_pz/sf);
  _pe_m =-p4+round(best_pe/sf);  _pe_p=p4+round(best_pe/sf);
  _tf_m =-p5+round(best_tf/sf);  _tf_p=p5+round(best_tf/sf);
  _ff_m =-p6+round(best_ff/sf);  _ff_p=p6+round(best_ff/sf);
  _df_m =-p7+round(best_df/sf);  _df_p=p7+round(best_df/sf);
  _od_m =-p8+round(best_ode/sf); _od_p=p8+round(best_ode/sf);
  _oh_m =-p9+round(best_ohe/sf); _oh_p=p9+round(best_ohe/sf);

  double ma,mz;
  for (_deo=_deo_m; _deo<=_deo_p; _deo++)
  for (_pd=_pd_m; _pd<=_pd_p; _pd++)
  for (_pz=_pz_m; _pz<=_pz_p; _pz++)
  for (_pe=_pe_m; _pe<=_pe_p; _pe++)
  for (_df=_df_m; _df<=_df_p; _df++)
  for (_ff=_ff_m; _ff<=_ff_p; _ff++)
  for (_tf=_tf_m; _tf<=_tf_p; _tf++)
  for (_ohe=_oh_m; _ohe<=_oh_p; _ohe++)
  for (_ode=_od_m; _ode<=_od_p; _ode++) {
    ode=((double)_ode)*sf1;
    odw=-ode;
    ohe=((double)_ohe)*sf1;
    ohw=ohe;
    
    // check the combinations for all samples
    for (l=0; l<num; l++) {
      mz=mount[l].azm;
      ma=mount[l].alt;
      
      if (mount[l].side==-1) // west of the mount
      {
        mz=mz+ohw;
        ma=ma+odw;
      } else
      if (mount[l].side==1) // east of the mount, default (fork mounts)
      {
        mz=mz+ohe;
        ma=ma+ode;
      }
      correct(mz,ma,mount[l].side,sf1,_deo,_pd,_pz,_pe,_df,_ff,_tf,&z1,&a1);

      delta[l].azm=actual[l].azm-(mz-z1);
      if (delta[l].azm>PI) delta[l].azm=delta[l].azm-PI*2.0; else
      if (delta[l].azm<-PI) delta[l].azm=delta[l].azm+PI*2.0;
      delta[l].alt=actual[l].alt-(ma-a1);
      delta[l].side=mount[l].side;
    }

    // calculate the standard deviations
    sum1=0.0; for (l=0; l<num; l++) sum1=sum1+sq(delta[l].azm*cos(actual[l].alt)); sz=sqrt(sum1/(num-1));
    sum1=0.0; for (l=0; l<num; l++) sum1=sum1+sq(delta[l].alt); sa=sqrt(sum1/(num-1));
    max_dist=sqrt(sq(sz)+sq(sa));

    // remember the best fit
    if (max_dist<best_dist) {
      best_dist   =max_dist;
      best_deo    =((double)_deo)*sf;
      best_pd     =((double)_pd)*sf;
      best_pz     =((double)_pz)*sf;
      best_pe     =((double)_pe)*sf;

      best_tf     =((double)_tf)*sf;
      best_df     =((double)_df)*sf;
      best_ff     =((double)_ff)*sf;
      
      if (p8!=0) best_odw=odw*Rad*3600.0; else best_odw=best_pe/2.0;
      if (p8!=0) best_ode=ode*Rad*3600.0; else best_ode=-best_pe/2.0;
      if (p9!=0) best_ohw=ohw*Rad*3600.0;
      if (p9!=0) best_ohe=ohe*Rad*3600.0;
    }

    // keep the main loop running
    loop2();
  }
}

void TGeoAlignH::autoModel(int n) {

  num=n; // how many stars?

  lat=90.0/Rad; // 90 deg. latitude for Alt/Azm
  cosLat=cos(lat);
  sinLat=sin(lat);

  best_dist   =3600.0*180.0;
  best_deo    =0.0;
  best_pd     =0.0;
  best_pz     =0.0;
  best_pe     =0.0;
  best_tf     =0.0;
  best_ff     =0.0;
  best_df     =0.0;
  best_ode    =0.0;
  best_ohe    =0.0;

  // figure out the average Az offset as a starting point
  ohe=0;
  for (l=0; l<num; l++) {
    z1=actual[l].azm-mount[l].azm;
    if (z1>PI)  z1=z1-PI*2.0;
    if (z1<-PI) z1=z1+PI*2.0;
    ohe=ohe+z1;
  }
  ohe=ohe/num; best_ohe=round(ohe*Rad*3600.0); best_ohw=best_ohe;

  // these don't apply for Alt/Az
  Ff=0; Df=0;

  // only search for cone error if > 2 stars
  int Do=0; if (num>2) Do=1;
  
  // search, this can handle about 9 degrees of polar misalignment, and 4 degrees of cone error
  //              DoPdPzPeTfFf Df OdOh
  do_search(16384,0 ,0,1,1,0, 0, 0,1,1);
  do_search( 8192,Do,0,1,1,0, 0, 0,1,1);
  do_search( 4096,Do,0,1,1,0, 0, 0,1,1);
  do_search( 2048,Do,0,1,1,0, 0, 0,1,1);
  do_search( 1024,Do,0,1,1,0, 0, 0,1,1);
  do_search(  512,Do,0,1,1,0, 0, 0,1,1);
#ifdef HAL_SLOW_PROCESSOR
  //              DoPdPzPeTfFf Df OdOh
  do_search(  256,Do,0,1,1,0, 0, 0,1,1);
  do_search(  128,Do,0,1,1,0, 0, 0,1,1);
#else
  if (num>4) {
    //              DoPdPzPeTfFf Df OdOh
    do_search(  256,Do,1,1,1,0,Ff,Df,1,1);
    do_search(  128,Do,1,1,1,1,Ff,Df,1,1);
    do_search(   64,Do,1,1,1,1,Ff,Df,1,1);
#ifdef HAL_FAST_PROCESSOR
    do_search(   32,Do,1,1,1,1,Ff,Df,1,1);
    do_search(   16,Do,1,1,1,1,Ff,Df,1,1);
#endif
  } else {
    do_search(  256,Do,0,1,1,0, 0, 0,1,1);
    do_search(  128,Do,0,1,1,0, 0, 0,1,1);
    do_search(   64,Do,0,1,1,0, 0, 0,1,1);
    do_search(   32,Do,0,1,1,0, 0, 0,1,1);
#ifdef HAL_FAST_PROCESSOR
    do_search(   16,Do,0,1,1,0, 0, 0,1,1);
#endif
  }
#endif

  // geometric corrections
  doCor=best_deo/3600.0;
  pdCor=best_pd/3600.0;
  azmCor=best_pz/3600.0;
  altCor=best_pe/3600.0;

  tfCor=best_tf/3600.0;
#if defined(MOUNT_TYPE_FORK) || defined(MOUNT_TYPE_FORK_ALT) || defined(MOUNT_TYPE_ALTAZM)
  dfCor=best_ff/3600.0;
#else
  dfCor=best_df/3600.0;
#endif

  ax1Cor=best_ohw/3600.0;
  ax2Cor=best_odw/3600.0;

  geo_ready=true;
}

void TGeoAlignH::horToInstr(double Alt, double Azm, double *Alt1, double *Azm1, int PierSide) {
  double p=1.0; if (PierSide==PierSideWest) p=-1.0;
  
  double cosLat=cos(90.0/Rad); double sinLat=sin(90.0/Rad);
  
  if (Alt>90.0) Alt=90.0;
  if (Alt<-90.0) Alt=-90.0;

  // breaks-down near the Zenith (limited to >1' from Zenith)
  if (abs(Alt)<89.98333333) {

    // initial rough guess at instrument HA,Dec
    double z=Azm/Rad;
    double a=Alt/Rad;
    
    for (int pass=0; pass<3; pass++) {
      double sinAlt=sin(a);
      double cosAlt=cos(a);
      double sinAzm=sin(z);
      double cosAzm=cos(z);

      // ------------------------------------------------------------
      // misalignment due to tube/optics not being perp. to Alt axis
      // negative numbers are further (down) from the Zenith, swing to the
      // horizon and the effect on Alt is 0. At the Nadir it
      // becomes an (up) offset.  Unchanged with meridian flips.
      // expressed as a correction to the Zenith axis misalignment
      double DOh=doCor*(1.0/cosAlt)*p;
  
      // ------------------------------------------------------------
      // misalignment due to Alt axis being perp. to Azm axis
      double PDh=-pdCor*(sinAlt/cosAlt)*p;
  
  #if defined (MOUNT_TYPE_FORK) || defined(MOUNT_TYPE_FORK_ALT) || defined(MOUNT_TYPE_ALTAZM)
      // Fork flex
      double DFd=dfCor*cosAzm;
  #else
      // Axis flex
      double DFd=-dfCor*(cosLat*cosAzm+sinLat*(sinAlt/cosAlt));
  #endif
  
      // Tube flex
      double TFh=tfCor*(cosLat*sinAzm*(1.0/cosAlt));
      double TFd=tfCor*(cosLat*cosAzm-sinLat*cosAlt);
  
      // polar misalignment
      double z1=-azmCor*cosAzm*(sinAlt/cosAlt) + altCor*sinAzm*(sinAlt/cosAlt);
      double a1=+azmCor*sinAzm                 + altCor*cosAzm;
      *Azm1=Azm + (z1+PDh+DOh+TFh);
      *Alt1=Alt + (a1+DFd+TFd);

      // improved guess at instrument Alt,Azm
      z=*Azm1/Rad;
      a=*Alt1/Rad;
    }
  } else {
    // just ignore the the correction if right on the pole
    *Azm1=Azm;
    *Alt1=Alt;
  }
  
  // finally, apply the index offsets
  *Azm1=*Azm1-ax1Cor;
  *Alt1=*Alt1-ax2Cor*-p;
}

// takes the instrument equatorial coordinates and applies corrections to arrive at topocentric refracted coordinates
void TGeoAlignH::instrToHor(double Alt, double Azm, double *Alt1, double *Azm1, int PierSide) { 
  double p=1.0; if (PierSide==PierSideWest) p=-1.0;
  
  double cosLat=cos(90.0/Rad); double sinLat=sin(90.0/Rad);
  
  Azm=Azm+ax1Cor;
  Alt=Alt+ax2Cor*-p;
  
  if (Alt>90.0) Alt=90.0;
  if (Alt<-90.0) Alt=-90.0;

  // breaks-down near the Zenith (limited to >1' from Zenith)
  if (abs(Alt)<89.98333333) {
    double z=Azm/Rad;
    double a=Alt/Rad;
    double sinAlt=sin(a);
    double cosAlt=cos(a);
    double sinAzm=sin(z);
    double cosAzm=cos(z);

    // ------------------------------------------------------------
    // misalignment due to tube/optics not being perp. to Alt axis
    // negative numbers are further (S) from the Zenith, swing to the
    // horizon and the effect on Alt is 0. At the Nadir it
    // becomes a (N) offset.  Unchanged with meridian flips.
    // expressed as a correction to the Azm axis misalignment
    double DOh=doCor*(1.0/cosAlt)*p;

    // as the above offset becomes zero near the horizon, the affect
    // works on Azm instead.  meridian flips affect this in Azm
    double PDh=-pdCor*(sinAlt/cosAlt)*p;

#if defined (MOUNT_TYPE_FORK) || defined(MOUNT_TYPE_FORK_ALT)
    // Fork flex
    double DFd=dfCor*cosAzm;
#else
    // Axis flex
    double DFd=-dfCor*(cosLat*cosAzm+sinLat*(sinAlt/cosAlt));
#endif

    // Tube flex
    double TFh=tfCor*(cosLat*sinAzm*(1.0/cosAlt));
    double TFd=tfCor*(cosLat*cosAzm-sinLat*cosAlt);
   
    // ------------------------------------------------------------
    // polar misalignment
    double z1=-azmCor*cosAzm*(sinAlt/cosAlt) + altCor*sinAzm*(sinAlt/cosAlt);
    double a1=+azmCor*sinAzm                 + altCor*cosAzm;

    *Azm1=Azm - (z1+PDh+DOh+TFh);
    *Alt1=Alt - (a1+DFd+TFd);
  } else {
    // just ignore the the correction if right on the pole
    *Azm1=Azm;
    *Alt1=Alt;
  }

  while (*Azm1>360.0) *Azm1-=360.0;
  while (*Azm1<-360.0) *Azm1+=360.0;
  if (*Alt1>90.0) *Alt1=90.0;
  if (*Alt1<-90.0) *Alt1=-90.0;
}
#endif
