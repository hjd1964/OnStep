#include <Arduino.h>

const double Rad=57.29577951;

const unsigned int FM_NONE           = 0;
const unsigned int FM_ABOVE_HORIZON  = 1;
const unsigned int FM_ALIGN_ALL_SKY  = 2;
const unsigned int FM_CONSTELLATION  = 4;
const unsigned int FM_OBJ_TYPE       = 8;
const unsigned int FM_BY_MAG         = 16;
const unsigned int FM_NEARBY         = 32;
const unsigned int FM_DBL_MIN_SEP    = 64;
const unsigned int FM_DBL_MAX_SEP    = 128;
const unsigned int FM_VAR_MAX_PER    = 256;

enum CAT_TYPES {CAT_NONE, CAT_GEN_STAR, CAT_GEN_STAR_VCOMP, CAT_DBL_STAR, CAT_DBL_STAR_COMP, CAT_VAR_STAR, CAT_VAR_STAR_COMP, CAT_DSO, CAT_DSO_COMP, CAT_DSO_VCOMP};

class CatMgr {
  public:
// initialization
    void        setLat(double lat);
    void        setLstT0(double lstT0);
    void        setLastTeleEqu(double RA, double Dec);
    bool        isInitialized();

// time
    double      lstDegs();
    double      lstHours();

// catalog selection
    int         numCatalogs();
    void        select(int cat);
    CAT_TYPES   catalogType();

    bool        hasDblStarCatalog();
    bool        hasVarStarCatalog();
    bool        isStarCatalog();
    bool        isDblStarCatalog();
    bool        isVarStarCatalog();
    bool        isDsoCatalog();
    const char* catalogTitle();
    const char* catalogSubMenu();
    const char* catalogPrefix();
    bool        hasPrimaryIdInPrefix();

// catalog filtering
    void        filtersClear();
    void        filterAdd(int fm);
    void        filterAdd(int fm, int param);
    bool        hasActiveFilter();

// select catalog record
    bool        setIndex(long index);
    long        getIndex();
    long        getMaxIndex();
    bool        incIndex();
    bool        decIndex();

// get catalog contents
    int         epoch();

    double      ra();
    double      rah();
    double      ha();
    void        raHMS(uint8_t &h, uint8_t &m, uint8_t &s);
    double      dec();
    void        decDMS(short &d, uint8_t &m, uint8_t &s);
    double      alt();
    void        altDMS(short &d, uint8_t &m, uint8_t &s);
    double      azm();
    void        azmDMS(short &d, uint8_t &m, uint8_t &s);

    void        topocentricToObservedPlace(float *RA, float *Dec);

    float       period();
    int         positionAngle();
    float       separation();

    float       magnitude();
    float       magnitude2();
    
    byte        constellation();
    const char* constellationStr();
    const char* constellationCodeToStr(int code);

    byte        objectType();
    const char* objectTypeStr();
    const char* objectTypeCodeToStr(int code);

    long        objectName();
    const char* objectNameStr();

    long        primaryId();
    long        subId();
    const char* subIdStr();

    int         bayerFlam();
    const char* bayerFlamStr();

private:
    double _lat=-10000;
    double _cosLat=0;
    double _sinLat=0;
    double _lstT0=0;
    double _lastTeleRA=0;
    double _lastTeleDec=0;
    unsigned long _lstMillisT0=0;
    
    int _fm=FM_NONE;
    int _fm_con=0;
    int _fm_obj_type=0;
    double _fm_mag_limit=100.0;
    double _fm_nearby_dist=10000.0;
    double _fm_dbl_min=0.0;
    double _fm_dbl_max=100000.0;
    double _fm_var_max=100000.0;
    
    int _selected=0;

    bool isFiltered();

    const char* getElementFromString(const char *data, long elementNum);
    double DistFromEqu(double RA, double Dec);
    double HAToRA(double ha);
    void EquToHor(double RA, double Dec, double *Alt, double *Azm);
    void EquToAlt(double RA, double Dec, double *Alt);
    void HorToEqu(double Alt, double Azm, double *RA, double *Dec);
    double TrueRefrac(double Alt, double Pressure=1010.0, double Temperature=10.0);

    double cot(double n);
};

extern CatMgr cat_mgr;
