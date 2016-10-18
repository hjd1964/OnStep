// -----------------------------------------------------------------------------------
// PROGRAM FOR POINTING A TELESCOPE BY T. TAKI
//
// CONVERSION TO C++/Arduino BY Howard Dutton, 10/12/2016

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
    void bestZ12(int I);
};

TAlign Align;

