// ---------------------------------------------------------------------
// Program for pointing a telescope by Toshimi Taki
//
// Original published in Sky & Telescope, February 1989, pages 194-196.
//
// Source here, with errors corrected: 
// http://wwwcdn.skyandtelescope.com/wp-content/uploads/taki.bas
//
// Background on equations is on Taki's site:
// http://www.geocities.jp/toshimi_taki/aim/aim.htm
//
// Conversion to C++/Arduino by Howard Dutton, 10/12/2016

#ifdef MOUNT_TYPE_ALTAZM
TAlign::TAlign()
{
  init();
}

TAlign::~TAlign()
{
}

// Initialize
void TAlign::init() {
  Z1=0;  // Mount error angle between horizontal axis and vertical axis
  Z2=0;  // Mount error angle between vertical axis and telescope optical axis
  Z3=0;  // Mount error angle, zero point shift (azimuth axis of rotation vs. instrument altitude angle)
  //
  t_ready=false;
}

// Status
bool TAlign::isReady() {
  return t_ready;
}

// I=1 for 1st star, I=2 for 2nd star, both must be initialized before use
// for I>2 additional stars allow for mount modeling
// N=total number of stars for this align
// B=HA, D=Dec, H=Elevation, F=Azimuth (all in degrees)
void TAlign::addStar(int I, int N, double B, double D, double H, double F) {

  AlignStars[I-1].HA=B;  AlignStars[I-1].Dec=D;
  AlignStars[I-1].Alt=H; AlignStars[I-1].Az=F;

  // FOR >2 STARS LOG THEM AND USE TO CALCULATE Z1/Z2/Z3 TERMS
  if (I==N) {
    t_ready=true;
    if (I>2) {
      // handles searching for Z3 up to +/- 10 degrees
      bestZ3(N,-10.0,10.0,2.0);       // 10x
      bestZ3(N,Z3-2.0,Z3+2.0,0.5);    // 8x
      bestZ3(N,Z3-0.5,Z3+0.5,0.062);  // 16x
//    bestZ12(N);
    }
  }
  if (I>2) return;

  // HAVE STAR, PREPARE
  X[1][I]=cos(D/Rad)*cos(B/Rad);
  X[2][I]=cos(D/Rad)*sin(B/Rad);
  X[3][I]=sin(D/Rad);
  F=F/Rad;
  H=(H+Z3)/Rad;
  t_sub1(F,H);
  Y[1][I]=Y[1][0];
  Y[2][I]=Y[2][0];
  Y[3][I]=Y[3][0];
  //
  X[1][3]=X[2][1]*X[3][2]-X[3][1]*X[2][2];
  X[2][3]=X[3][1]*X[1][2]-X[1][1]*X[3][2];
  X[3][3]=X[1][1]*X[2][2]-X[2][1]*X[1][2];
  double A=sqrt(pow(X[1][3],2.0)+pow(X[2][3],2.0)+pow(X[3][3],2.0));
  for (int I=1; I<=3; I++) {
    X[I][3]=X[I][3]/A;
  }
  //
  Y[1][3]=Y[2][1]*Y[3][2]-Y[3][1]*Y[2][2];
  Y[2][3]=Y[3][1]*Y[1][2]-Y[1][1]*Y[3][2];
  Y[3][3]=Y[1][1]*Y[2][2]-Y[2][1]*Y[1][2];
  A=sqrt(pow(Y[1][3],2.0)+pow(Y[2][3],2.0)+pow(Y[3][3],2.0));
  for (int I=1; I<=3; I++) {
    Y[I][3]=Y[I][3]/A;
  }
  //
  //
  // TRANSFORM MATRIX
  for (int I=1; I<=3; I++) {
    for (int J=1; J<=3; J++) {
      V[I][J]=X[I][J];
    }
  }
  t_deter_sub();
  double E=W;
  //
  for (int M=1; M<=3; M++) {
    for (int I=1; I<=3; I++) {
      for (int J=1; J<=3; J++) {
        V[I][J]=X[I][J];
      }
    }
    for (int N=1; N<=3; N++) {
      V[1][M]=0;
      V[2][M]=0;
      V[3][M]=0;
      V[N][M]=1;
      t_deter_sub();
      Q[M][N]=W/E;
    }
  }
  //
  for (int I=1; I<=3; I++) {
    for (int J=1; J<=3; J++) {
      R[I][J]=0;
    }
  }
  for (int I=1; I<=3; I++) {
    for (int J=1; J<=3; J++) {
      for (int L=1; L<=3; L++) {
        R[I][J]=R[I][J]+Y[I][L]*Q[L][J];
      }
    }
  }
  //
  for (int M=1; M<=3; M++) {
    for (int I=1; I<=3; I++) {
      for (int J=1; J<=3; J++) {
        V[I][J]=R[I][J];
      }
    }
    t_deter_sub();
    E=W;
    for (int N=1; N<=3; N++) {
      V[1][M]=0;
      V[2][M]=0;
      V[3][M]=0;
      V[N][M]=1;
      t_deter_sub();
      Q[M][N]=W/E;
    }
  }
}

//
// CONVERT EQUATORIAL --> TELESCOPE
// B=HA, D=Dec, H=Elevation, F=Azimuth (all in degrees)
void TAlign::EquToInstr(double B, double D, double *H, double *F) {
  X[1][1]=cos(D/Rad)*cos(B/Rad);
  X[2][1]=cos(D/Rad)*sin(B/Rad);
  X[3][1]=sin(D/Rad);
  Y[1][1]=0;
  Y[2][1]=0;
  Y[3][1]=0;
  for (int I=1; I<=3; I++) {
    for (int J=1; J<=3; J++) {
      Y[I][1]=Y[I][1]+R[I][J]*X[J][1];
    }
  }
  t_angle_sub(F,H);
  *F=*F/Rad;
  *H=*H/Rad;
  t_sub2(*F,*H);
  t_angle_sub(F,H);
  *H=*H-Z3;
}

//
// CONVERT TELESCOPE --> EQUATORIAL
// H=Elevation, F=Azimuth, B=HA, D=Dec (all in degrees)
void TAlign::InstrToEqu(double H, double F, double *B, double *D) {
  F=F/Rad;
  H=(H+Z3)/Rad;
  t_sub1(F,H);
  X[1][1]=Y[1][0];
  X[2][1]=Y[2][0];
  X[3][1]=Y[3][0];
  Y[1][1]=0;
  Y[2][1]=0;
  Y[3][1]=0;
  for (int I=1; I<=3; I++) {
    for (int J=1; J<=3; J++) {
      Y[I][1]=Y[I][1]+Q[I][J]*X[J][1];
    }
  }
  t_angle_sub(&F,&H);
  F=F-round(F/360.0)*360.0;  // was INT()
  *B=F;
  *D=H;
}

//
// DETERMINANT SUBROUTINE
void TAlign::t_deter_sub() {
  W=  V[1][1]*V[2][2]*V[3][3]+V[1][2]*V[2][3]*V[3][1];
  W=W+V[1][3]*V[3][2]*V[2][1];
  W=W-V[1][3]*V[2][2]*V[3][1]-V[1][1]*V[3][2]*V[2][3];
  W=W-V[1][2]*V[2][1]*V[3][3];
}

//
// ANGLE SUBROUTINE
void TAlign::t_angle_sub(double *F, double *H) {
  double C=sqrt(Y[1][1]*Y[1][1]+Y[2][1]*Y[2][1]);
  if ((C==0) && (Y[3][1]>0)) *H=90.0;
  if ((C==0) && (Y[3][1]<0)) *H=-90.0;
  if (C!=0) *H=atan(Y[3][1]/C)*Rad;
  //
  if (C==0) *F=1000.0;
  if ((C!=0) && (Y[1][1]==0) && (Y[2][1]>0)) *F=90.0;
  if ((C!=0) && (Y[1][1]==0) && (Y[2][1]<0)) *F=270.0;
  if (Y[1][1]>0) *F=atan(Y[2][1]/Y[1][1])*Rad;
  if (Y[1][1]<0) *F=atan(Y[2][1]/Y[1][1])*Rad+180.0;
  *F=*F-round(*F/360.0)*360.0; // was INT()
}

//
// SUBROUTINE
void TAlign::t_sub1(double F, double H) {
  Y[1][0]=cos(F)*cos(H)-sin(F)*(Z2/Rad);
  Y[1][0]=Y[1][0]+sin(F)*sin(H)*(Z1/Rad);
  Y[2][0]=sin(F)*cos(H)+cos(F)*(Z2/Rad);
  Y[2][0]=Y[2][0]-cos(F)*sin(H)*(Z1/Rad);
  Y[3][0]=sin(H);
}

//
// SUBROUTINE
void TAlign::t_sub2(double F, double H) {
  Y[1][1]=cos(F)*cos(H)+sin(F)*(Z2/Rad);
  Y[1][1]=Y[1][1]-sin(F)*sin(H)*(Z1/Rad);
  Y[2][1]=sin(F)*cos(H)-cos(F)*(Z2/Rad);
  Y[2][1]=Y[2][1]+cos(F)*sin(H)*(Z1/Rad);
  Y[3][1]=sin(H);
}

//
// BESTZ3
// nrange to range is the search area in degrees
// incr is the increment distance across that range in +/- degrees
void TAlign::bestZ3(int N, double nrange, double range, double incr) {
  double HA1,Dec1;
  double HA2,Dec2;
  double BestZ3=180.0;
  double BestDist=180.0;

  // search
  for (Z3=nrange; Z3<=range; Z3+=incr) {
    // for each star
    for (int J=0; J<N; J++) {
      InstrToEqu(AlignStars[J].Alt,AlignStars[J].Az,&HA1,&Dec1); 

      double Dist=0;
      for (int K=0; K<N; K++) {
        if (J!=K) {
          // Star J to Star K dist catalog vs. aligned

          // catalog
          double Dist1=angDist(AlignStars[J].HA,AlignStars[J].Dec,AlignStars[K].HA,AlignStars[K].Dec);

          // aligned
          InstrToEqu(AlignStars[K].Alt,AlignStars[K].Az,&HA2,&Dec2);
          double Dist2=angDist(HA1,Dec1,HA2,Dec2);

          Dist+=abs(Dist1-Dist2);
        }
      }
      Dist=Dist/N; // average distance for this set
      if (Dist<BestDist) { BestZ3=Z3; BestDist=Dist; }
    }
  }

  Z3=BestZ3;
}
#endif

#ifndef ALIGN_GOTOASSIST_ON
// -----------------------------------------------------------------------------------
// GEOMETRIC ALIGN FOR EQUATORIAL MOUNTS
//

TGeoAlign::TGeoAlign()
{
  init();
}

TGeoAlign::~TGeoAlign()
{
}

// Initialize
void TGeoAlign::init() {
  avgDec=0.0;
  avgHA =0.0;
  
  altCor=0;  // for geometric coordinate correction/align, - is below the pole, + above (Z3)
  azmCor=0;  // - is right of the pole, + is left
  doCor =0;  // declination/optics orthogonal correction (Z2)
  pdCor =0;  // declination/polar orthogonal correction  (Z1)
  dfCor =0;  // fork or declination axis flex
  tfCor =0;  // tube flex

  geo_ready=false;
}

// remember the alignment between sessions
void TGeoAlign::readCoe() {
  dfCor=nv.readFloat(EE_dfCor);  // dfCor is ffCor for fork mounts
  tfCor=nv.readFloat(EE_tfCor);
  doCor=nv.readFloat(EE_doCor);
  pdCor=nv.readFloat(EE_pdCor);
  altCor=nv.readFloat(EE_altCor);
  azmCor=nv.readFloat(EE_azmCor);
}

void TGeoAlign::writeCoe() {
  nv.writeFloat(EE_dfCor,dfCor);  // dfCor is ffCor for fork mounts
  nv.writeFloat(EE_tfCor,tfCor);
  nv.writeFloat(EE_doCor,doCor);
  nv.writeFloat(EE_pdCor,pdCor);
  nv.writeFloat(EE_altCor,altCor);
  nv.writeFloat(EE_azmCor,azmCor);
}

// Status
bool TGeoAlign::isReady() {
  return geo_ready;
}

/*
Alignment Logic:
Near the celestial equator (Dec=0, HA=0)...
the azmCor term is 0 in Dec
the altCor term is 1 in Dec
the doCor  term is 1 in HA
the pdCor  term is 0 in HA

Near HA=6 and Dec=45...
the azmCor term is 1 in Dec
the altCor term is 0 in Dec
the doCor  term is 0 in HA
the pdCor  term is 1 in HA
*/

// I=1 for 1st star, I=2 for 2nd star, I=3 for 3rd star
// N=total number of stars for this align (1 to 3)
// RA, Dec (all in degrees)
bool TGeoAlign::addStar(int I, int N, double RA, double Dec) {

  cli();
  AlignStars[I-1].HA =haRange(LST()*15.0-RA);
  AlignStars[I-1].Dec=Dec;
  AlignStars[I-1].HA1=((double)(long)targetAxis1.part.m)/(double)StepsPerDegreeAxis1;
  AlignStars[I-1].Dec1=((double)(long)targetAxis2.part.m)/(double)StepsPerDegreeAxis2;
  sei();

  // two or more stars and finished
  if ((I>=2) && (I==N)) geo_ready=true;

  // First star:
  // Near the celestial equator (Dec=0, HA=0), telescope West of the pier if multi-star align
  if (I==1) {
    // set the indexAxis1 offset
    // set the indexAxis2 offset
    if (syncEqu(RA,Dec)!=0) { return false; }

    avgDec=Dec;
    avgHA =haRange(LST()*15.0-RA);
  } else 
  // Second star:
  // Near the celestial equator (Dec=0, HA=0), telescope East of the pier
  if (I==2) {
    double IH1=indexAxis1;
    double ID1=-indexAxis2;

    avgDec=(avgDec+Dec)/2.0;
    avgHA =(-avgHA+haRange(LST()*15.0-RA))/2.0; // last HA is negative because we were on the other side of the meridian
    if (syncEqu(RA,Dec)==0) {
      double IH2=indexAxis1;
      double ID2=indexAxis2;

      indexAxis1  = (IH2+IH1)/2.0;              // average offset in HA
      indexAxis1Steps = (long)(indexAxis1*(double)StepsPerDegreeAxis1);
      indexAxis2  = (ID2-ID1)/2.0;              // new offset in Dec
      indexAxis2Steps = (long)(indexAxis2*(double)StepsPerDegreeAxis2);

      double IH3=indexAxis1;
      double ID3=indexAxis2;
    
      altCor=-(ID2+ID1)/2.0;                    // Negative when pointed below the pole
      altCor= altCor/cos(avgHA/Rad);            // correct for measurements being away from the Meridian

      // allow the altCor to be applied
      if (syncEqu(RA,Dec)==0) {
        IH2=indexAxis1;
        ID2=indexAxis2;

        doCor =-(IH2-IH1)/2.0;                  // the difference of these two values should be a decent approximation of the optical axis to Dec axis error (aka cone error)
        doCor = doCor*cos(avgDec/Rad);          // correct for measurement being away from the Celestial Equator

        indexAxis1      = IH3;
        indexAxis1Steps = (long)(indexAxis1*(double)StepsPerDegreeAxis1);
        indexAxis2      = ID3;
        indexAxis2Steps = (long)(indexAxis2*(double)StepsPerDegreeAxis2);
      } else return false;
    } else return false;
  } else 
  // Third star:
  // Near (Dec=45, HA=6), telescope East of the pier
  if (I==3) {
    double IH1=indexAxis1;
    double ID1=indexAxis2;
    if (syncEqu(RA,Dec)==0) {
      double IH2=indexAxis1;
      double ID2=indexAxis2;

      azmCor = -(ID2-ID1);                                        // offset in declination is largely due to polar align Azm error
      azmCor = azmCor/sin(haRange(LST()*15.0-newTargetRA)/Rad);   // correct for HA of measurement location

      // allow the azmCor to be applied
      if (syncEqu(RA,Dec)==0) {
        IH2=indexAxis1;
        ID2=indexAxis2;
        // only apply Dec axis flexture term on GEMs
        #ifdef MOUNT_TYPE_GEM
        pdCor = (IH2-IH1);                      // the Dec axis to RA axis perp. error should be the only major source of error left affecting the HA
        pdCor = pdCor/tan(newTargetDec/Rad);    // correct for Dec of measurement location
        #else
        pdCor = 0.0;
        #endif

        indexAxis1=IH1;
        indexAxis1Steps = (long)(indexAxis1*(double)StepsPerDegreeAxis1);
        indexAxis2=ID1;
        indexAxis2Steps = (long)(indexAxis2*(double)StepsPerDegreeAxis2);
      } else return false;
    } else return false;
  } else return false;
  return true;
}

// takes the topocentric refracted coordinates and applies corrections to arrive at instrument equatorial coordinates 
void TGeoAlign::EquToInstr(double Lat, double HA, double Dec, double *HA1, double *Dec1) {
  if (Dec>90.0) Dec=90.0;
  if (Dec<-90.0) Dec=-90.0;

  // breaks-down near the pole (limited to >1' from pole)
  if (abs(Dec)<89.98333333) {

    // initial rough guess at instrument HA,Dec
    double h=HA/Rad;
    double d=Dec/Rad;
    
    for (int pass=0; pass<3; pass++) {
      double sinDec=sin(d);
      double cosDec=cos(d);
      double sinHA =sin(h);
      double cosHA =cos(h);
      double p=1.0; if (pierSide==PierSideWest) p=-1.0;
  
      // ------------------------------------------------------------
      // misalignment due to tube/optics not being perp. to Dec axis
      // negative numbers are further (S) from the NCP, swing to the
      // equator and the effect on declination is 0. At the SCP it
      // becomes a (N) offset.  Unchanged with meridian flips.
      // expressed as a correction to the Polar axis misalignment
      double DOh=doCor*(1.0/cosDec)*p;
  
      // ------------------------------------------------------------
      // misalignment due to Dec axis being perp. to RA axis
      double PDh=-pdCor*(sinDec/cosDec)*p;
  
  #if defined (MOUNT_TYPE_FORK) || defined(MOUNT_TYPE_FORK_ALT)
      // Fork flex
      double DFd=dfCor*cosHA;
  #else
      // Axis flex
      double DFd=-dfCor*(cosLat*cosHA+sinLat*(sinDec/cosDec));
  #endif
  
      // Tube flex
      double TFh=tfCor*(cosLat*sinHA*(1.0/cosDec));
      double TFd=tfCor*(cosLat*cosHA-sinLat*cosDec);
  
      // polar misalignment
      double h1=-azmCor*cosHA*(sinDec/cosDec) + altCor*sinHA*(sinDec/cosDec);
      double d1=+azmCor*sinHA                 + altCor*cosHA;
      *HA1 =HA +(h1+PDh+DOh+TFh);
      *Dec1=Dec+(d1+DFd+TFd);

      // improved guess at instrument HA,Dec
      h=*HA1/Rad;
      d=*Dec1/Rad;
    }
  } else {
    // just ignore the the correction if right on the pole
    *HA1 =HA;
    *Dec1=Dec;
  }

  while (*HA1>180.0) *HA1-=360.0;
  while (*HA1<-180.0) *HA1+=360.0;
  
#ifndef MOUNT_TYPE_ALTAZM
  // switch to under the pole coordinates
  if ((Lat>=0) && ((abs(*HA1)>(double)UnderPoleLimit*15.0))) {
    *HA1 =*HA1-180.0; while (*HA1<-180.0) *HA1=*HA1+360.0;
    *Dec1=(90.0-*Dec1)+90.0;
  }
  if ((Lat<0) && ((abs(*HA1)>(double)UnderPoleLimit*15.0) )) {
    *HA1 =*HA1-180.0; while (*HA1<-180.0) *HA1=*HA1+360.0;
    *Dec1=(-90.0-*Dec1)-90.0;
  }
#endif

// finally, apply the index offsets
  *HA1-=indexAxis1; *Dec1-=indexAxis2;
}

// takes the instrument equatorial coordinates and applies corrections to arrive at topocentric refracted coordinates
void TGeoAlign::InstrToEqu(double Lat, double HA, double Dec, double *HA1, double *Dec1) { 
  // remove the index offsets
  HA+=indexAxis1; Dec+=indexAxis2;

#ifndef MOUNT_TYPE_ALTAZM
  // switch from under the pole coordinates
  if (Dec>90.0) { Dec=(90.0-Dec)+90; HA=HA-180.0; }
  if (Dec<-90.0) { Dec=(-90.0-Dec)-90.0; HA=HA-180.0; }
#endif

  while (HA>180.0) HA-=360.0;
  while (HA<-180.0) HA+=360.0;

  // breaks-down near the pole (limited to >1' from pole)
  if (abs(Dec)<89.98333333) {
    double h =HA/Rad;
    double d =Dec/Rad;
    double sinDec=sin(d);
    double cosDec=cos(d);
    double sinHA =sin(h);
    double cosHA =cos(h);
    double p=1.0; if (pierSide==PierSideWest) p=-1.0;

    // ------------------------------------------------------------
    // misalignment due to tube/optics not being perp. to Dec axis
    // negative numbers are further (S) from the NCP, swing to the
    // equator and the effect on declination is 0. At the SCP it
    // becomes a (N) offset.  Unchanged with meridian flips.
    // expressed as a correction to the Polar axis misalignment
    double DOh=doCor*(1.0/cosDec)*p;

    // as the above offset becomes zero near the equator, the affect
    // works on HA instead.  meridian flips affect this in HA
    double PDh=-pdCor*(sinDec/cosDec)*p;

#if defined (MOUNT_TYPE_FORK) || defined(MOUNT_TYPE_FORK_ALT)
    // Fork flex
    double DFd=dfCor*cosHA;
#else
    // Axis flex
    double DFd=-dfCor*(cosLat*cosHA+sinLat*(sinDec/cosDec));
#endif

    // Tube flex
    double TFh=tfCor*(cosLat*sinHA*(1.0/cosDec));
    double TFd=tfCor*(cosLat*cosHA-sinLat*cosDec);
   
    // ------------------------------------------------------------
    // polar misalignment
    double h1=-azmCor*cosHA*(sinDec/cosDec) + altCor*sinHA*(sinDec/cosDec);
    double d1=+azmCor*sinHA                 + altCor*cosHA;

    *HA1 =HA -(h1+PDh+DOh+TFh);
    *Dec1=Dec-(d1+DFd+TFd);
  } else {
    // just ignore the the correction if right on the pole
    *HA1=HA;
    *Dec1=Dec;
  }

  while (*HA1>180.0) *HA1-=360.0;
  while (*HA1<-180.0) *HA1+=360.0;
  if (*Dec1>90.0) *Dec1=90.0;
  if (*Dec1<-90.0) *Dec1=-90.0;
}

#else

// -----------------------------------------------------------------------------------
// GEOMETRIC ALIGN FOR EQUATORIAL MOUNTS, GOTO ASSIST BASED
//

#define GOTO_ASSIST_DEBUG_OFF

TGeoAlign::TGeoAlign()
{
  init();
}

TGeoAlign::~TGeoAlign()
{
}

// Initialize
void TGeoAlign::init() {
  avgDec=0.0;
  avgHA =0.0;
  
  altCor=0;  // for geometric coordinate correction/align, - is below the pole, + above (Z3)
  azmCor=0;  // - is right of the pole, + is left
  doCor =0;  // declination/optics orthogonal correction (Z2)
  pdCor =0;  // declination/polar orthogonal correction  (Z1)
  dfCor =0;  // fork or declination axis flex
  tfCor =0;  // tube flex

  geo_ready=false;
}

// remember the alignment between sessions
void TGeoAlign::readCoe() {
  dfCor=nv.readFloat(EE_dfCor);  // dfCor is ffCor for fork mounts
  tfCor=nv.readFloat(EE_tfCor);
  doCor=nv.readFloat(EE_doCor);
  pdCor=nv.readFloat(EE_pdCor);
  altCor=nv.readFloat(EE_altCor);
  azmCor=nv.readFloat(EE_azmCor);
}

void TGeoAlign::writeCoe() {
  nv.writeFloat(EE_dfCor,dfCor);  // dfCor is ffCor for fork mounts
  nv.writeFloat(EE_tfCor,tfCor);
  nv.writeFloat(EE_doCor,doCor);
  nv.writeFloat(EE_pdCor,pdCor);
  nv.writeFloat(EE_altCor,altCor);
  nv.writeFloat(EE_azmCor,azmCor);
}

// Status
bool TGeoAlign::isReady() {
  return geo_ready;
}

/*
Alignment Logic:
Near the celestial equator (Dec=0, HA=0)...
the azmCor term is 0 in Dec
the altCor term is 1 in Dec
the doCor  term is 1 in HA
the pdCor  term is 0 in HA

Near HA=6 and Dec=45...
the azmCor term is 1 in Dec
the altCor term is 0 in Dec
the doCor  term is 0 in HA
the pdCor  term is 1 in HA
*/

// I=1 for 1st star, I=2 for 2nd star, I=3 for 3rd star
// N=total number of stars for this align (1 to 3)
// RA, Dec (all in degrees)
bool TGeoAlign::addStar(int I, int N, double RA, double Dec) {

  // First star:
  // Near the celestial equator (Dec=0, HA=0), telescope West of the pier if multi-star align
  if ((I==1) && (N==1)) {
    // set the indexAxis1/2 offset
    if (syncEqu(RA,Dec)!=0) { return false; }
  }

  mount[I-1].ha=(((double)(long)(targetAxis1.part.m+indexAxis1Steps))/(double)StepsPerDegreeAxis1)/Rad;
  mount[I-1].dec=(((double)(long)(targetAxis2.part.m+indexAxis2Steps))/(double)StepsPerDegreeAxis2)/Rad;
  actual[I-1].ha=haRange(LST()*15.0-RA)/Rad;
  actual[I-1].dec=Dec/Rad;
  if (pierSide==PierSideWest) { actual[I-1].side=-1; mount[I-1].side=-1; } else
  if (pierSide==PierSideEast) { actual[I-1].side=1; mount[I-1].side=1; } else { actual[I-1].side=0; mount[I-1].side=0; }

#ifdef GOTO_ASSIST_DEBUG_ON
  char s[80];
  double f;
  D((char)(I-1+'0'));
  D(" Mount h");
  f=mount[I-1].ha*Rad/15.0; if (f<0) f=f+24.0; doubleToHms(s,&f); D(s);
  D(",d");
  f=mount[I-1].dec*Rad; doubleToDms(s,&f,false,true); D(s);
  D("   Actual h");
  f=actual[I-1].ha*Rad/15.0; if (f<0) f=f+24.0; doubleToHms(s,&f); D(s);
  D(",d");
  f=actual[I-1].dec*Rad; doubleToDms(s,&f,false,true); DL(s);
#endif

  // two or more stars and finished
  if ((I>=2) && (I==N)) {
#ifdef GOTO_ASSIST_DEBUG_ON
    DL("");
#endif
    autoModel(N);
 //   if (syncEqu(RA,Dec)!=0) { return false; }
    geo_ready=true;
  }

  return true;
}

// returns the correction to be added to the requested RA,Dec to yield the actual RA,Dec that we will arrive at
void TGeoAlign::correct(double ha, double dec, double pierSide, double sf, double _deo, double _pd, double _pz, double _pe, double _da, double _ff, double _tf, double *h1, double *d1) {
  double DO1,DOh;
  double PD,PDh;
  double PZ,PA;
  double DF,DFd,TF,FF,FFd,TFh,TFd;
  double lat;

  lat=latitude/Rad;

// ------------------------------------------------------------
// A. Misalignment due to tube/optics not being perp. to Dec axis
// negative numbers are further (S) from the NCP, swing to the
// equator and the effect on declination is 0. At the SCP it
// becomes a (N) offset.  Unchanged with meridian flips.
  DO1 =((_deo*sf)/3600.0)/Rad;
// works on HA.  meridian flips effect this in HA
  DOh = DO1*(1.0/cos(dec))*pierSide;

// ------------------------------------------------------------
// B. Misalignment, Declination axis relative to Polar axis
// expressed as a correction to where the Polar axis is pointing
// negative numbers are further (S) from the NCP, swing to the
// equator and the effect on declination is 0.
// At the SCP it is, again, a (S) offset
  PD  =((_pd*sf)/3600.0)/Rad;
// works on HA.
  PDh = -PD*tan(dec)*pierSide;

// ------------------------------------------------------------
// Misalignment, relative to NCP
// negative numbers are east of the pole
// C. polar left-right misalignment
  PZ  =((_pz*sf)/3600.0)/Rad;
// D. negative numbers are below the pole
// polar below-above misalignment
  PA  =((_pe*sf)/3600.0)/Rad;

// ------------------------------------------------------------
// Axis flex
  DF  =((_da*sf)/3600.0)/Rad;
  DFd =-DF*(cos(lat)*cos(ha)+sin(lat)*tan(dec));

// ------------------------------------------------------------
// Fork flex
  FF  =((_ff*sf)/3600.0)/Rad;
  FFd =FF*cos(ha);

// ------------------------------------------------------------
// Optical axis sag
  TF  =((_tf*sf)/3600.0)/Rad;

  TFh =TF*(cos(lat)*sin(ha)*(1.0/cos(dec)));
  TFd =TF*(cos(lat)*cos(ha)-sin(lat)*cos(dec));

// ------------------------------------------------------------
  *h1  =(-PZ*cos(ha)*tan(dec) + PA*sin(ha)*tan(dec) + DOh +  PDh +       TFh);
  *d1  =(+PZ*sin(ha)          + PA*cos(ha)                +  DFd + FFd + TFd);
}

void TGeoAlign::do_search(double sf, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9)
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

  for (_deo=_deo_m; _deo<=_deo_p; _deo++)
  for (_pd=_pd_m; _pd<=_pd_p; _pd++)
  for (_pz=_pz_m; _pz<=_pz_p; _pz++)
  for (_pe=_pe_m; _pe<=_pe_p; _pe++)
  for (_df=_df_m; _df<=_df_p; _df++)
  for (_ff=_ff_m; _ff<=_ff_p; _ff++)
  for (_tf=_tf_m; _tf<=_tf_p; _tf++)
  for (_ohe=_oh_m; _ohe<=_oh_p; _ohe++)
  for (_ode=_od_m; _ode<=_od_p; _ode++) {
    ode=((((double)_ode)*sf)/(3600.0*Rad));
    odw=-ode;
    ohe=((((double)_ohe)*sf)/(3600.0*Rad));
    ohw=ohe;

     // check the combinations for all samples
    for (l=1; l<=num; l++) {
      correct(mount[l].ha,mount[l].dec,mount[l].side,sf,_deo,_pd,_pz,_pe,_df,_ff,_tf,&h1,&d1);

      if (mount[l].side==-1) // west of the mount
      {
        h1=h1+ohw;
        d1=d1+odw;
      } else
      if (mount[l].side==1) // east of the mount, default (fork mounts)
      {
        h1=h1+ohe;
        d1=d1+ode;
      } else
      {
        h1=0; d1=0;
      }

      delta[l].ha=actual[l].ha-(mount[l].ha+h1);
      if (delta[l].ha>PI) delta[l].ha=delta[l].ha-PI*2.0;
      if (delta[l].ha<-PI) delta[l].ha=delta[l].ha+PI*2.0;
      delta[l].dec=actual[l].dec-(mount[l].dec+d1);
      delta[l].side=mount[l].side;
    }

    // calculate the standard deviations
    sum1=0.0; for (l=1; l<=num; l++) sum1=sum1+sq(delta[l].ha*cos(actual[l].dec)); sh=sqrt(sum1/(num-1));
    sum1=0.0; for (l=1; l<=num; l++) sum1=sum1+sq(delta[l].dec); sd=sqrt(sum1/(num-1));
    max_dist=sqrt(sq(sh)+sq(sd));

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
  }
}

void TGeoAlign::autoModel(int n) {
  num=n; // how many stars?

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

  // figure out the average HA offset as a starting point
  ohe=0;
  for (l=1; l<=num; l++) {
    h1=actual[l].ha-mount[l].ha;
    if (h1>PI)  h1=h1-PI*2.0;
    if (h1<-PI) h1=h1+PI*2.0;
    ohe=ohe+h1;
  }
  ohe=ohe/num; best_ohe=round(ohe*Rad*3600.0); best_ohw=best_ohe;

#if defined(MOUNT_TYPE_FORK) || defined(MOUNT_TYPE_FORK_ALT)
  Ff=1; Df=0;
#else
  Ff=0; Df=1;
#endif

#ifdef HAL_SLOW_PROCESSOR
  // search, this can handle about 4.5 degrees of polar misalignment, and 1 degree of cone error
  //             DoPdPzPeTfFfDfOdOh
  do_search( 8192,0,0,1,1,0,0,0,0,0);
  do_search( 4096,0,0,1,1,0,0,0,0,0);
  do_search( 2048,1,0,1,1,0,0,0,0,0);
  do_search( 1024,1,0,1,1,0,0,0,0,0);
  do_search(  512,1,0,1,1,0,0,0,0,0);
  do_search(  256,1,0,1,1,0,0,0,0,0);
  do_search(  128,1,1,1,1,0,0,0,0,0);
#else
  // search, this can handle about 9 degrees of polar misalignment, and 2 degrees of cone error, 6' of FF/DF and TF
  if (num>4) {
    do_search(16384,0,0,1,1,0, 0, 0,0,0);
    do_search( 8192,0,0,1,1,0, 0, 0,0,0);
    do_search( 4096,1,0,1,1,0, 0, 0,0,0);
    do_search( 2048,1,0,1,1,0, 0, 0,0,0);
    do_search( 1024,1,0,1,1,0, 0, 0,0,0);
    do_search(  512,1,0,1,1,1, 0, 0,0,0);
    do_search(  256,1,0,1,1,1,Ff,Df,0,0);
    do_search(  128,1,1,1,1,1,Ff,Df,0,0);
  } else {
    do_search(16384,0,0,1,1,0,0,0,0,0);
    do_search( 8192,0,0,1,1,0,0,0,0,0);
    do_search( 4096,1,0,1,1,0,0,0,0,0);
    do_search( 2048,1,0,1,1,0,0,0,0,0);
    do_search( 1024,1,0,1,1,0,0,0,0,0);
    do_search(  512,1,0,1,1,0,0,0,0,0);
    do_search(  256,1,0,1,1,0,0,0,0,0);
    do_search(  128,1,1,1,1,0,0,0,0,0);
  }
#endif

  // geometric corrections
  altCor=best_pe/3600.0;
  azmCor=best_pz/3600.0;
  doCor=best_deo/3600.0;
  pdCor=best_pd/3600.0;
#if defined(MOUNT_TYPE_FORK) || defined(MOUNT_TYPE_FORK_ALT)
  dfCor=best_df/3600.0;
#else
  dfCor=best_ff/3600.0;
#endif
  tfCor=best_tf/3600.0;

#ifdef GOTO_ASSIST_DEBUG_ON
  DL("");
  char s[80];
  D("Model Error  =");
  dtostrf(best_dist*Rad*60.0,6,2,s); DL(s);
#endif

  // offset corrections: doesn't matter, a sync will override this
  indexAxis1=best_ohw/3600.0;
  indexAxis1Steps=(long)(indexAxis1*((double)StepsPerDegreeAxis1));
  if (pierSide==PierSideWest) indexAxis2=best_odw/3600.0;
  if (pierSide==PierSideEast) indexAxis2=best_ode/3600.0;
  indexAxis2Steps=(long)(indexAxis2*((double)StepsPerDegreeAxis2));
}

// takes the topocentric refracted coordinates and applies corrections to arrive at instrument equatorial coordinates 
void TGeoAlign::EquToInstr(double Lat, double HA, double Dec, double *HA1, double *Dec1) {
  if (Dec>90.0) Dec=90.0;
  if (Dec<-90.0) Dec=-90.0;

  // breaks-down near the pole (limited to >1' from pole)
  if (abs(Dec)<89.98333333) {
    double h =HA/Rad;
    double d =Dec/Rad;
    double sinDec=sin(d);
    double cosDec=cos(d);
    double sinHA =sin(h);
    double cosHA =cos(h);
    double p=1.0; if (pierSide==PierSideWest) p=-1.0;

    // ------------------------------------------------------------
    // misalignment due to tube/optics not being perp. to Dec axis
    // negative numbers are further (S) from the NCP, swing to the
    // equator and the effect on declination is 0. At the SCP it
    // becomes a (N) offset.  Unchanged with meridian flips.
    // expressed as a correction to the Polar axis misalignment
    double DOh=doCor*(1.0/cosDec)*p;

    // ------------------------------------------------------------
    // misalignment due to Dec axis being perp. to RA axis
    double PDh=-pdCor*(sinDec/cosDec)*p;

#if defined (MOUNT_TYPE_FORK) || defined(MOUNT_TYPE_FORK_ALT)
    // Fork flex
    double DFd=dfCor*cosHA;
#else
    // Axis flex
    double DFd=-dfCor*(cosLat*cosHA+sinLat*(sinDec/cosDec));
#endif

    // Tube flex
    double TFh=tfCor*(cosLat*sinHA*(1.0/cosDec));
    double TFd=tfCor*(cosLat*cosHA-sinLat*cosDec);

    // polar misalignment
    double h1=-azmCor*cosHA*(sinDec/cosDec) + altCor*sinHA*(sinDec/cosDec);
    double d1=+azmCor*sinHA                 + altCor*cosHA;
    *HA1 =HA +(h1+PDh+DOh+TFh);
    *Dec1=Dec+(d1+DFd+TFd);
  } else {
    // just ignore the the correction if right on the pole
    *HA1 =HA;
    *Dec1=Dec;
  }

  while (*HA1>180.0) *HA1-=360.0;
  while (*HA1<-180.0) *HA1+=360.0;
  
#ifndef MOUNT_TYPE_ALTAZM
  // switch to under the pole coordinates
  if ((Lat>=0) && ((abs(*HA1)>(double)UnderPoleLimit*15.0))) {
    *HA1 =*HA1-180.0; while (*HA1<-180.0) *HA1=*HA1+360.0;
    *Dec1=(90.0-*Dec1)+90.0;
  }
  if ((Lat<0) && ((abs(*HA1)>(double)UnderPoleLimit*15.0) )) {
    *HA1 =*HA1-180.0; while (*HA1<-180.0) *HA1=*HA1+360.0;
    *Dec1=(-90.0-*Dec1)-90.0;
  }
#endif

// finally, apply the index offsets
  *HA1-=indexAxis1; *Dec1-=indexAxis2;
}

// takes the instrument equatorial coordinates and applies corrections to arrive at topocentric refracted coordinates
void TGeoAlign::InstrToEqu(double Lat, double HA, double Dec, double *HA1, double *Dec1) { 
  // remove the index offsets
  HA+=indexAxis1; Dec+=indexAxis2;

#ifndef MOUNT_TYPE_ALTAZM
  // switch from under the pole coordinates
  if (Dec>90.0) { Dec=(90.0-Dec)+90; HA=HA-180.0; }
  if (Dec<-90.0) { Dec=(-90.0-Dec)-90.0; HA=HA-180.0; }
#endif

  while (HA>180.0) HA-=360.0;
  while (HA<-180.0) HA+=360.0;

  // breaks-down near the pole (limited to >1' from pole)
  if (abs(Dec)<89.98333333) {
    double h =HA/Rad;
    double d =Dec/Rad;
    double sinDec=sin(d);
    double cosDec=cos(d);
    double sinHA =sin(h);
    double cosHA =cos(h);
    double p=1.0; if (pierSide==PierSideWest) p=-1.0;

    // ------------------------------------------------------------
    // misalignment due to tube/optics not being perp. to Dec axis
    // negative numbers are further (S) from the NCP, swing to the
    // equator and the effect on declination is 0. At the SCP it
    // becomes a (N) offset.  Unchanged with meridian flips.
    // expressed as a correction to the Polar axis misalignment
    double DOh=doCor*(1.0/cosDec)*p;

    // as the above offset becomes zero near the equator, the affect
    // works on HA instead.  meridian flips affect this in HA
    double PDh=-pdCor*(sinDec/cosDec)*p;

#if defined (MOUNT_TYPE_FORK) || defined(MOUNT_TYPE_FORK_ALT)
    // Fork flex
    double DFd=dfCor*cosHA;
#else
    // Axis flex
    double DFd=-dfCor*(cosLat*cosHA+sinLat*(sinDec/cosDec));
#endif

    // Tube flex
    double TFh=tfCor*(cosLat*sinHA*(1.0/cosDec));
    double TFd=tfCor*(cosLat*cosHA-sinLat*cosDec);
   
    // ------------------------------------------------------------
    // polar misalignment
    double h1=-azmCor*cosHA*(sinDec/cosDec) + altCor*sinHA*(sinDec/cosDec);
    double d1=+azmCor*sinHA                 + altCor*cosHA;

    *HA1 =HA -(h1+PDh+DOh+TFh);
    *Dec1=Dec-(d1+DFd+TFd);
  } else {
    // just ignore the the correction if right on the pole
    *HA1=HA;
    *Dec1=Dec;
  }

  while (*HA1>180.0) *HA1-=360.0;
  while (*HA1<-180.0) *HA1+=360.0;
  if (*Dec1>90.0) *Dec1=90.0;
  if (*Dec1<-90.0) *Dec1=-90.0;
}
#endif
