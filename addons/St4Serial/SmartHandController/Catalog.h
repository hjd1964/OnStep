#pragma once
#include <Arduino.h>

const double Rad=57.29577951;

enum Catalog { STAR, MESSIER, HERSCHEL, CAT_NONE };
enum FilterMode { FM_NONE, FM_ABOVE_HORIZON, FM_ALIGN_ALL_SKY, FM_ALIGN_3STAR_1, FM_ALIGN_3STAR_2, FM_ALIGN_3STAR_3 };

extern const char *constellation_txt[];
extern const char *catalog_txt[];

//Herschel
extern const char *Herschel_info_txt[];
extern const unsigned short Herschel_NGC[400];
extern const byte Herschel_info[400];
extern const unsigned short Herschel_ra[400];
extern const short Herschel_dec[400];
extern const byte Herschel_obj[400];
extern const byte Hershel_constellation[400];
extern const byte Hershel_dMag[400];

//Messier
extern const unsigned short Messier_ra[110];
extern const short Messier_dec[110];
extern const byte Messier_obj[110];
extern const byte Messier_constellation[110];
extern const byte Messier_dMag[110];

//Star
extern const byte Star_letter[292];
extern const byte Star_constellation[292];
extern const unsigned short Star_ra[292];
extern const short Star_dec[292];

void getcatdms(const short& v, short& v1, uint8_t& v2);
void getcatdf(const short& v, float& v1);
void getcathms(const unsigned short& v, uint8_t& v1, uint8_t& v2, uint8_t& v3);
void getcathf(const unsigned short& v, float& v1);

class CatMgr {
  public:
// initialization
    void setLat(double lat);
    void setLstT0(double lstT0);
  
    void select(Catalog cat);
    Catalog getCat();
    const char* catalogStr();

    bool canFilter();
    void filter(FilterMode fm);

    void setIndex(int index);
    int getIndex();
    int getMaxIndex();
    void incIndex();
    void decIndex();
    
    double ra();
    double ha();
    void   raHMS(uint8_t& v1, uint8_t& v2, uint8_t& v3);
    double dec();
    void   decDM(short& v1, uint8_t& v2);
    int    epoch();
    double alt();
    double azm();
    double magnitude();
    byte   constellation();
    const char* constellationStr();
    byte   objectType();
    const char* objectInfoStr();
    int    primaryId();

    void EquToHor(double RA, double Dec, double *Alt, double *Azm);
    void HorToEqu(double Alt, double Azm, double *RA, double *Dec);
    double TrueRefrac(double Alt, double Pressure=1010.0, double Temperature=10.0);
    
private:
    double _lat=-10000;
    double _cosLat=0;
    double _sinLat=0;
    double _lstT0=0;
    Catalog _cat=CAT_NONE;
    FilterMode _fm=FM_NONE;
    int _selected=0;
    int _idx[4]={0,0,0,0};
    int _maxIdx[4]={292-1,110-1,400-1,0-1};

    bool isFiltered();
    double DistFromEqu(double RA, double Dec);
    double HAToRA(double ha);
    double cot(double n);
};

extern CatMgr cat_mgr;

