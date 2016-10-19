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
// range is range the search distance +/- in degrees
// incr is the increment distance across that range in +/- degrees
void TAlign::bestZ3(int N, double nrange, double range, double incr) {
  double HA1,Dec1;
  double HA2,Dec2;
  double BestZ3=180.0;
  double BestDist=180.0;

  // +/- 10 degrees range for our search
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


