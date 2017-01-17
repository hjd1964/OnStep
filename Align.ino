// -----------------------------------------------------------------------------------
// PROGRAM FOR POINTING A TELESCOPE BY T. TAKI
//
// CONVERSION TO C++/Arduino BY Howard Dutton, 10/12/2016

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
    return;
  }

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

  geo_ready=false;
}

// remember the alignment between sessions
void TGeoAlign::readCoe() {
  doCor=EEPROM_readFloat(EE_doCor);
  pdCor=EEPROM_readFloat(EE_pdCor);
  altCor=EEPROM_readFloat(EE_altCor);
  azmCor=EEPROM_readFloat(EE_azmCor);
}

void TGeoAlign::writeCoe() {
  EEPROM_writeFloat(EE_doCor,doCor);
  EEPROM_writeFloat(EE_pdCor,pdCor);
  EEPROM_writeFloat(EE_altCor,altCor);
  EEPROM_writeFloat(EE_azmCor,azmCor);
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

  AlignStars[I-1].HA =haRange(LST()*15.0-RA);
  AlignStars[I-1].Dec=Dec;
  AlignStars[I-1].HA1=((double)(long)targetAxis1.part.m)/(double)StepsPerDegreeAxis1;
  AlignStars[I-1].Dec=((double)(long)targetAxis2.part.m)/(double)StepsPerDegreeAxis2;

  // two or more stars and finished
  if ((I>=2) && (I==N)) geo_ready=true;

  // First star:
  // Near the celestial equator (Dec=0, HA=0), telescope West of the pier if multi-star align
  if (I==1) {
    // set the indexAxis1 offset
    // set the indexAxis2 offset
    if (!syncEqu(RA,Dec)) { return false; }

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
    if (syncEqu(RA,Dec)) {
      double IH2=indexAxis1;
      double ID2=indexAxis2;

      indexAxis1  = (IH2+IH1)/2.0;                      // average offset in HA
      indexAxis1Steps = (long)(indexAxis1*(double)StepsPerDegreeAxis1);
      indexAxis2  = (ID2-ID1)/2.0;                      // new offset in Dec
      indexAxis2Steps = (long)(indexAxis2*(double)StepsPerDegreeAxis2);

      double IH3=indexAxis1;
      double ID3=indexAxis2;
    
      altCor=-(ID2+ID1)/2.0;                    // Negative when pointed below the pole
      altCor= altCor/cos(avgHA/Rad);            // correct for measurements being away from the Meridian

      // allow the altCor to be applied
      if (syncEqu(RA,Dec)) {
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
    if (syncEqu(RA,Dec)) {
      double IH2=indexAxis1;
      double ID2=indexAxis2;

      azmCor = -(ID2-ID1);                                        // offset in declination is largely due to polar align Azm error
      azmCor = azmCor/sin(haRange(LST()*15.0-newTargetRA)/Rad);   // correct for HA of measurement location

      // allow the azmCor to be applied
      if (syncEqu(RA,Dec)) {
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
    double h =HA/Rad;
    double d =Dec/Rad;
    double dSin=sin(d);
    double dCos=cos(d);
    double p=1.0; if (pierSide==PierSideWest) p=-1.0;

    // ------------------------------------------------------------
    // misalignment due to tube/optics not being perp. to Dec axis
    // negative numbers are further (S) from the NCP, swing to the
    // equator and the effect on declination is 0. At the SCP it
    // becomes a (N) offset.  Unchanged with meridian flips.
    // expressed as a correction to the Polar axis misalignment
    double DOh = doCor*(1.0/dCos)*p;

    // ------------------------------------------------------------
    // misalignment due to Dec axis being perp. to RA axis
    double PDh =-pdCor*(dSin/dCos)*p;

    // polar misalignment
    double h1=-azmCor*cos(h)*(dSin/dCos) + altCor*sin(h)*(dSin/dCos);
    double d1=+azmCor*sin(h)             + altCor*cos(h);
    *HA1 =HA +(h1+PDh+DOh);
    *Dec1=Dec+ d1;
  } else {
    // just ignore the the correction if right on the pole
    *HA1 =HA;
    *Dec1=Dec;
  }

  while (*HA1>180.0) *HA1-=360.0;
  while (*HA1<-180.0) *HA1+=360.0;
  
  // finally, apply the index offsets
  *HA1-=indexAxis1; *Dec1-=indexAxis2;
}

// takes the instrument equatorial coordinates and applies corrections to arrive at topocentric refracted coordinates
void TGeoAlign::InstrToEqu(double Lat, double HA, double Dec, double *HA1, double *Dec1) { 
  // remove the index offsets
  HA+=indexAxis1; Dec+=indexAxis2;

  while (HA>180.0) HA-=360.0;
  while (HA<-180.0) HA+=360.0;

  // breaks-down near the pole (limited to >1' from pole)
  if (abs(Dec)<89.98333333) {
    double h =HA/Rad;
    double d =Dec/Rad;
    double dSin=sin(d);
    double dCos=cos(d);
    double p=1.0; if (pierSide==PierSideWest) p=-1.0;

    // ------------------------------------------------------------
    // misalignment due to tube/optics not being perp. to Dec axis
    // negative numbers are further (S) from the NCP, swing to the
    // equator and the effect on declination is 0. At the SCP it
    // becomes a (N) offset.  Unchanged with meridian flips.
    // expressed as a correction to the Polar axis misalignment
    double DOh = doCor*(1.0/dCos)*p;

    // as the above offset becomes zero near the equator, the affect
    // works on HA instead.  meridian flips effect this in HA
    double PDh =-pdCor*(dSin/dCos)*p;

    // ------------------------------------------------------------
    // polar misalignment
    double h1=-azmCor*cos(h)*(dSin/dCos) + altCor*sin(h)*(dSin/dCos);
    double d1=+azmCor*sin(h)             + altCor*cos(h);
    *HA1 =HA -(h1+PDh+DOh);
    *Dec1=Dec- d1;
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

