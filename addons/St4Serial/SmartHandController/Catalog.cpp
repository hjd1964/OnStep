#include <Arduino.h>
#include "Catalog.h"
#include "Config.h"
#include "math.h"

#include "constants.h"

// Do not change anything in the structs or arrays below, since they
// have to be in sync with the extractions scripts.

// Struct for Deep Space Objects (Messier, Herschel, ..etc.)
typedef struct {
  const char           Cons;
  const char           Obj_type;
  const unsigned short Obj_id;
  const signed   short Mag;
  const float          RA;
  const float          DE;
  } dso_t;

// Struct for stars
typedef struct {
  const char           Cons;
  const char           Bayer;
  const signed   short Mag;
  const float          RA;
  const float          DE;
  const char           Name[16];
  } star_t;

// Bayer designation, the Greek letter for each star within a constellation
const char* Txt_Bayer[24] = {
  "Alp",
  "Bet",
  "Gam",
  "Del",
  "Eps",
  "Zet",
  "Eta",
  "The",
  "Iot",
  "Kap",
  "Lam",
  "Mu" ,
  "Nu" ,
  "Xi" ,
  "Omi",
  "Pi" ,
  "Rho",
  "Sig",
  "Tau",
  "Ups",
  "Phi",
  "Chi",
  "Psi",
  "Ome"
};

// Type of object, in the Open NGC database. Do not change the
// order of this, since it has to match what is in Open NGC
const char * Txt_Object_Type[20] = {
  "Galaxy",
  "Open Cluster",
  "Star",
  "Double Star",
  "Other",
  "Galaxy Pair",
  "Galaxy Triplet",
  "Galaxy Group",
  "Globular Cluster",
  "Planetary Nebula",
  "Nebula",
  "Hii Region",
  "Cluster + Nebula",
  "Asterism",
  "Reflectn Nebula",
  "SuperNova Rmnnt",
  "Emission Nebula",
  "NonExist",
  "Nova",
  "Dup"
};

// Constellation abbreviation, alphabetical order
const char* Txt_Constellations[89] = {
  "And", //  0
  "Ant", //  1
  "Aps", //  2
  "Aql", //  3
  "Aqr", //  4
  "Ara", //  5
  "Ari", //  6
  "Aur", //  7
  "Boo", //  8
  "CMa", //  9
  "CMi", //  10
  "CVn", //  11
  "Cae", //  12
  "Cam", //  13
  "Cap", //  14
  "Car", //  15
  "Cas", //  16
  "Cen", //  17
  "Cep", //  18
  "Cet", //  19
  "Cha", //  20
  "Cir", //  21
  "Cnc", //  22
  "Col", //  23
  "Com", //  24
  "CrA", //  25
  "CrB", //  26
  "Crt", //  27
  "Cru", //  28
  "Crv", //  29
  "Cyg", //  30
  "Del", //  31
  "Dor", //  32
  "Dra", //  33
  "Equ", //  34
  "Eri", //  35
  "For", //  36
  "Gem", //  37
  "Gru", //  38
  "Her", //  39
  "Hor", //  40
  "Hya", //  41
  "Hyi", //  42
  "Ind", //  43
  "LMi", //  44
  "Lac", //  45
  "Leo", //  46
  "Lep", //  47
  "Lib", //  48
  "Lup", //  49
  "Lyn", //  50
  "Lyr", //  51
  "Men", //  52
  "Mic", //  53
  "Mon", //  54
  "Mus", //  55
  "Nor", //  56
  "Oct", //  57
  "Oph", //  58
  "Ori", //  59
  "Pav", //  60
  "Peg", //  61
  "Per", //  62
  "Phe", //  63
  "Pic", //  64
  "PsA", //  65
  "Psc", //  66
  "Pup", //  67
  "Pyx", //  68
  "Ret", //  69
  "Scl", //  70
  "Sco", //  71
  "Sct", //  72
  "Ser", //  73
  "Sex", //  74
  "Sge", //  75
  "Sgr", //  76
  "Tau", //  77
  "Tel", //  78
  "TrA", //  79
  "Tri", //  80
  "Tuc", //  81
  "UMa", //  82
  "UMi", //  83
  "Vel", //  84
  "Vir", //  85
  "Vol", //  86
  "Vul", //  87
  "Unknown" // 88
};

// Catalog names. 
const char * Txt_Catalog[] = {
  "Star ",
  "M ",
  "N ",
  "None "
};

// Data for different catalogs, each is a collection of certain celestial objects
// These are machine generated using scripts from Open NGC and KStars' Star data
// Do NOT edit manually. Rerun the scripts instead. 
#include "messier.h"
#include "herschel.h"
#include "stars.h"

// ----------------------------------------------------------
// Catalog Manager

// initialization
void CatMgr::setLat(double lat) {
  _lat=lat;
  if (lat<9999) {
    _cosLat=cos(lat/Rad);
    _sinLat=sin(lat/Rad);
  }
}

void CatMgr::setLstT0(double lstT0) {
  _lstT0=lstT0;
  _lstMillisT0=millis();
}

bool CatMgr::isInitialized() {
  return ((_lat<9999) && (_lstT0!=0));
}

// time
double CatMgr::lstDegs() {
  return (lstHours()*15.0);
}

double CatMgr::lstHours() {
  double msSinceT0=(unsigned long)(millis()-_lstMillisT0);
  double siderealSecondsSinceT0=(msSinceT0/1000.0)*1.00277778;
  return _lstT0+siderealSecondsSinceT0/3600.0;
}

// handle catalog selection
void CatMgr::select(Catalog cat) {
  _cat=cat;
  if (_cat==STAR) _selected=0; else
  if (_cat==MESSIER) _selected=1; else
  if (_cat==HERSCHEL) _selected=2; else { _selected=0; _cat=CAT_NONE; }
}

Catalog CatMgr::getCat() {
  return _cat;
}

const char* CatMgr::catalogStr() {
  return Txt_Catalog[_selected];
}

// catalog filtering
void CatMgr::filter(FilterMode fm) {
  _fm=fm;
}

bool CatMgr::isFiltered() {
  double ar,ad;
  if (!isInitialized()) return false;
  switch (_fm) {
    case FM_NONE:
      return false;
      break;
    case FM_ABOVE_HORIZON:
      return alt()<0.0;
      break;
    case FM_ALIGN_ALL_SKY:
      // minimum 10 degrees altitude (to limit unlikely align stars and to minimize refraction effects)
      if (alt()<10.0) return true;
      // minimum 5 degrees from the pole (for accuracy)
      if (_lat>=0.0) {
        if (dec()>85.0) return true;
      } else {
        if (dec()<-85.0) return true;
      }
      return false;
      break;
    case FM_ALIGN_3STAR_1:
      // minimum 10 degrees altitude (to limit unlikely align stars and to minimize refraction effects)
      if (alt()<10.0) return true;
      // maximum 30 degrees from align location
      ar = HAToRA(-1.0 * 15.0);
      if (_lat >= 0.0) ad = 10.0; else ad = -10.0;
      if (DistFromEqu(ar,ad)>30.0) return true;
      // on our side of meridian
      if (ha()>0) return true;
      return false;
      break;
    case FM_ALIGN_3STAR_2:
      // minimum 10 degrees altitude (to limit unlikely align stars and to minimize refraction effects)
      if (alt()<10.0) return true;
      // maximum 30 degrees from align location
      ar = HAToRA(+1.0 * 15.0);
      if (_lat >= 0.0) ad = 10.0; else ad = -10.0;
      if (DistFromEqu(ar,ad)>30.0) return true;
      // on our side of meridian
      if (ha()<0) return true;
      return false;
    break;
    case FM_ALIGN_3STAR_3:
      // minimum 10 degrees altitude (to limit unlikely align stars and to minimize refraction effects)
      if (alt()<10.0) return true;
      // maximum 30 degrees from align location
      ar = HAToRA(+6.0 * 15.0);
      if (_lat >= 0.0) ad = 45.0; else ad = -45.0;
      if (DistFromEqu(ar,ad)>30.0) return true;
      return false;
      break;
  }
}

// select catalog record
void CatMgr::setIndex(int index) {
  _idx[_selected]=index;
  decIndex();
  incIndex();
}

int CatMgr::getIndex() {
  return _idx[_selected];
}

int CatMgr::getMaxIndex() {
  return _maxIdx[_selected];
}

void CatMgr::incIndex() {
  int i=_maxIdx[_selected]+1;
  do {
    i--;
    _idx[_selected]++;
    if (_idx[_selected]>_maxIdx[_selected]) _idx[_selected]=0;
  } while (isFiltered() && (i>0));
}

void CatMgr::decIndex() {
  int i=_maxIdx[_selected]+1;
  do {
    i--;
    _idx[_selected]--;
    if (_idx[_selected]<0) _idx[_selected]=_maxIdx[_selected];
  } while (isFiltered() && (i>0));
}

// get catalog contents

// RA in degrees
double CatMgr::ra() {
  return rah()*15.0;
}

// RA in hours
double CatMgr::rah() {
  double f;
  if (_cat==STAR)     f=Cat_Stars[_idx[_selected]].RA; else
  if (_cat==MESSIER)  f=Cat_Messier[_idx[_selected]].RA; else
  if (_cat==HERSCHEL) f=Cat_Herschel[_idx[_selected]].RA; else f=0;
  return f;
}

// HA in degrees
double CatMgr::ha() {
  if (!isInitialized()) return 0;
  double h=(lstDegs()-ra());
  while (h>180.0) h-=360.0;
  while (h<-180.0) h+=360.0;
  return h;
}

void CatMgr::raHMS(uint8_t& h, uint8_t& m, uint8_t& s) {
  double f;
  if (_cat==STAR)     f=Cat_Stars[_idx[_selected]].RA; else
  if (_cat==MESSIER)  f=Cat_Messier[_idx[_selected]].RA; else
  if (_cat==HERSCHEL) f=Cat_Herschel[_idx[_selected]].RA; else f=0;
  
  double h1,m1,s1;

  h1=floor(f);
  m1=(f-h1)*60;
  s1=(m1-floor(m1));

  s1=s1*60.0;

  h = (int)h1;
  m = (int)m1;
  s = (int)s1;
}

// Dec in degrees
double CatMgr::dec() {
  double f;
  if (_cat==STAR)     f=Cat_Stars[_idx[_selected]].DE; else
  if (_cat==MESSIER)  f=Cat_Messier[_idx[_selected]].DE; else
  if (_cat==HERSCHEL) f=Cat_Herschel[_idx[_selected]].DE; else f=0;
  return f;
}

void CatMgr::decDMS(short& d, uint8_t& m, uint8_t& s) {
  double f;
  if (_cat==STAR)     f=Cat_Stars[_idx[_selected]].DE; else
  if (_cat==MESSIER)  f=Cat_Messier[_idx[_selected]].DE; else
  if (_cat==HERSCHEL) f=Cat_Herschel[_idx[_selected]].DE; else f=0;

  double d1, m1, s1;
  d1=floor(f);
  m1=(f-d1)*60;
  s1=(m1-floor(m1))*60.0;

  d = (int)d1;
  if (f<0) {
    d *= -1;
  }

  m = (int)m1;
  s = (int)s1;
}

int CatMgr::epoch() {
  if (_cat==STAR) return 2000; else
  if (_cat==MESSIER) return 2000; else
  if (_cat==HERSCHEL) return 2000; else return 0;
}

// Alt in degrees
double CatMgr::alt() {
  double a,z;
  EquToHor(ra(),dec(),&a,&z);
  return a;
}

// Azm in degrees
double CatMgr::azm() {
  double a,z;
  EquToHor(ra(),dec(),&a,&z);
  return z;
}

double CatMgr::magnitude() {
  double m=250;
  if (_cat==STAR)     m=Cat_Stars[_idx[_selected]].Mag; else
  if (_cat==MESSIER)  m=Cat_Messier[_idx[_selected]].Mag; else
  if (_cat==HERSCHEL) m=Cat_Herschel[_idx[_selected]].Mag;
  return m/100.0;
}

byte CatMgr::constellation() {
  if (_cat==STAR)     return Cat_Stars[_idx[_selected]].Cons; else
  if (_cat==MESSIER)  return Cat_Messier[_idx[_selected]].Cons; else
  if (_cat==HERSCHEL) return Cat_Herschel[_idx[_selected]].Cons; else return 89;
}

const char* CatMgr::constellationStr() {
  return Txt_Constellations[constellation()];
}

byte CatMgr::objectType() {
  if (_cat==MESSIER)  return Cat_Messier[_idx[_selected]].Obj_type; else
  if (_cat==HERSCHEL) return Cat_Herschel[_idx[_selected]].Obj_type; else return -1;
}

const char* CatMgr::objectTypeStr() {
  if (_cat==STAR)     return "Star"; else
  if (_cat==MESSIER)  return Txt_Object_Type[objectType()]; else
  if (_cat==HERSCHEL) return Txt_Object_Type[objectType()]; else return "";
}

const char* CatMgr::objectName() {
  if (_cat==STAR)     return Cat_Stars[_idx[_selected]].Name; else
  if (_cat==MESSIER)  return "";
  if (_cat==HERSCHEL) return "";
}

int CatMgr::primaryId() {
  if (_cat==STAR)     return Cat_Stars[_idx[_selected]].Bayer + 1; else
  if (_cat==MESSIER)  return Cat_Messier[_idx[_selected]].Obj_id; else
  if (_cat==HERSCHEL) return Cat_Herschel[_idx[_selected]].Obj_id; else return -1;
}

// support functions
// convert equatorial coordinates to horizon, in degrees
void CatMgr::EquToHor(double RA, double Dec, double *Alt, double *Azm) {
  double HA=lstDegs()-RA;
  //D("RA="); D(RA);
  //D(",DE="); D(Dec);
  while (HA<0.0)    HA=HA+360.0;
  while (HA>=360.0) HA=HA-360.0;
  HA =HA/Rad;
  Dec=Dec/Rad;
  double SinAlt = (sin(Dec) * _sinLat) + (cos(Dec) * _cosLat * cos(HA));  
  *Alt   = asin(SinAlt);
  double t1=sin(HA);
  double t2=cos(HA)*_sinLat-tan(Dec)*_cosLat;
  *Azm=atan2(t1,t2)*Rad;
  *Azm=*Azm+180.0;
  *Alt = *Alt*Rad;
  //D(",Alt="); DL(*Alt);
}

// convert horizon coordinates to equatorial, in degrees
void CatMgr::HorToEqu(double Alt, double Azm, double *RA, double *Dec) { 
  while (Azm<0)      Azm=Azm+360.0;
  while (Azm>=360.0) Azm=Azm-360.0;
  Alt  = Alt/Rad;
  Azm  = Azm/Rad;
  double SinDec = (sin(Alt) * _sinLat) + (cos(Alt) * _cosLat * cos(Azm));  
  *Dec = asin(SinDec); 
  double t1=sin(Azm);
  double t2=cos(Azm)*_sinLat-tan(Alt)*_cosLat;
  double HA=atan2(t1,t2)*Rad;
  HA=HA+180.0;
  *Dec = *Dec*Rad;

  while (HA<0.0)    HA=HA+360.0;
  while (HA>=360.0) HA=HA-360.0;
  *RA=(lstDegs()-HA);
}

// returns the amount of refraction (in arcminutes) at the given true altitude (degrees), pressure (millibars), and temperature (celsius)
double CatMgr::TrueRefrac(double Alt, double Pressure, double Temperature) {
  double TPC=(Pressure/1010.0) * (283.0/(273.0+Temperature));
  double r=( ( 1.02*cot( (Alt+(10.3/(Alt+5.11)))/Rad ) ) ) * TPC;  if (r<0.0) r=0.0;
  return r;
}

double CatMgr::cot(double n) {
  return 1.0/tan(n);
}

// angular distance from current Equ coords, in degrees
double CatMgr::DistFromEqu(double RA, double Dec) {
  RA=RA/Rad; Dec=Dec/Rad;
  return acos( sin(dec()/Rad)*sin(Dec) + cos(dec()/Rad)*cos(Dec)*cos(ra()/Rad - RA))*Rad;
}

// convert an HA to RA, in degrees
double CatMgr::HAToRA(double HA) {
  return (lstDegs()-HA);
}

CatMgr cat_mgr;
