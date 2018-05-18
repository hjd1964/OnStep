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

#pragma once

byte alignNumStars = 0;
byte alignThisStar = 0;

#ifdef MOUNT_TYPE_ALTAZM
typedef struct {
  double HA;
  double Dec;
  double Alt;
  double Az;
} align_coord_t;

class TAlign
{
  public:
    TAlign();
    ~TAlign();
    void init();
    bool isReady();
    void addStar(int I, int N, double B, double D, double H, double F);
    void EquToInstr(double B, double D, double *H, double *F);
    void InstrToEqu(double H, double F, double *B, double *D);

  private:
    double Z1;
    double Z2;
    double Z3;
    boolean t_ready;
    double W;
    double Q[4][4];
    double V[4][4];
    double R[4][4];
    double X[4][4];
    double Y[4][4];

    align_coord_t AlignStars[10];

    void t_deter_sub();
    void t_angle_sub(double *F, double *H);
    void t_sub1(double F, double H);
    void t_sub2(double F, double H);

    void bestZ3(int I, double nrange, double range, double incr);
};

TAlign Align;
#endif

#ifndef ALIGN_GOTOASSIST_ON
// -----------------------------------------------------------------------------------
// GEOMETRIC ALIGN FOR EQUATORIAL MOUNTS
//

typedef struct {
  double HA;
  double Dec;
  double HA1;
  double Dec1;
} align_coord2_t;

class TGeoAlign
{
  public:
    double altCor;
    double azmCor;
    double doCor;
    double pdCor;
    double dfCor;
    double tfCor;

    TGeoAlign();
    ~TGeoAlign();
    void init();
    void readCoe();
    void writeCoe();
    bool isReady();
    bool addStar(int I, int N, double RA, double Dec);
    void EquToInstr(double Lat, double HA, double Dec, double *HA1, double *Dec1);
    void InstrToEqu(double Lat, double HA, double Dec, double *HA1, double *Dec1);

  private:
    boolean geo_ready;
    double avgDec;
    double avgHA;

    align_coord2_t AlignStars[4];
};

TGeoAlign GeoAlign;

#else
// -----------------------------------------------------------------------------------
// GEOMETRIC ALIGN FOR EQUATORIAL MOUNTS2
//

typedef struct {
  double ha;
  double dec;
  int side;
} align_coord2_t;

class TGeoAlign
{
  public:
    double altCor;
    double azmCor;
    double doCor;
    double pdCor;
    double dfCor;
    double tfCor;

    TGeoAlign();
    ~TGeoAlign();
    void init();
    void readCoe();
    void writeCoe();
    bool isReady();
    bool addStar(int I, int N, double RA, double Dec);
    void EquToInstr(double Lat, double HA, double Dec, double *HA1, double *Dec1);
    void InstrToEqu(double Lat, double HA, double Dec, double *HA1, double *Dec1);

  private:
    boolean geo_ready;
    double avgDec;
    double avgHA;

    long num,l;
    long Ff,Df;
    double best_deo, best_pd, best_pz, best_pe, best_ohw, best_odw, best_ohe, best_ode, best_tf, best_df, best_ff;
    double h1,d1;
    double avg_ha,avg_dec;
    double dist,sumd,rms;
    double best_dist;
    double ohe,ode,ohw,odw,dh;
    double sd,sh,sum1;
    double max_dist;

    align_coord2_t mount[9];
    align_coord2_t actual[9];
    align_coord2_t delta[9];

    void correct(double ha, double dec, double pierSide, double sf, double _deo, double _pd, double _pz, double _pe, double _da, double _ff, double _tf, double *h1, double *d1);
    void do_search(double sf, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9);
    void autoModel(int n);
};

TGeoAlign GeoAlign;
#endif
