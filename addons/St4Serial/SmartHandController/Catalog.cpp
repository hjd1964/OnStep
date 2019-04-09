#include <Arduino.h>
#include <math.h>
#include "Config.h"
#include "Catalog.h"
#include "CatalogTypes.h"

// --------------------------------------------------------------------------------
// Data for different catalogs, each is a collection of certain celestial objects
// ** In this section is everything you need touch to add a new "catalog.h"      **
// ** file and use it within the SHC.                                            **

// Simply include the catalog you want here.  There are also _x, _c and _vc suffixes for some catalogs which...
// mean expanded, compact, and very-compact.  These options effect available catalogs, features, and accuracy.
  #include "catalogs/stars.h"
  #include "catalogs/messier_c.h"
  #include "catalogs/caldwell_c.h"
  #include "catalogs/herschel_c.h"
  #include "catalogs/collinder_vc.h"
  #include "catalogs/ngc_select_c.h"
  #include "catalogs/ic_select_c.h"

// And uncomment the matching line below:
catalog_t catalog[] = {
// Note: Alignment always uses the first catalog!
// Title         Prefix   Num records   Catalog data  Catalog name string  Catalog subId string  Type                Epoch
  {"Stars",       "Star ", NUM_STARS,    Cat_Stars,    Cat_Stars_Names,     Cat_Stars_SubId,      Cat_Stars_Type,     2000, 0},
  {"Messier",     "M",     NUM_MESSIER,  Cat_Messier,  Cat_Messier_Names,   Cat_Messier_SubId,    Cat_Messier_Type,   2000, 0},
  {"Caldwell",    "C",     NUM_CALDWELL, Cat_Caldwell, Cat_Caldwell_Names,  Cat_Caldwell_SubId,   Cat_Caldwell_Type,  2000, 0},
  {"Herschel400", "N",     NUM_HERSCHEL, Cat_Herschel, Cat_Herschel_Names,  Cat_Herschel_SubId,   Cat_Herschel_Type,  2000, 0},
  {"Collinder",   "Cr",    NUM_COLLINDER,Cat_Collinder,Cat_Collinder_Names, Cat_Collinder_SubId,  Cat_Collinder_Type, 2000, 0},
//{"NGC",         "N",     NUM_NGC,      Cat_NGC,      Cat_NGC_Names,       Cat_NGC_SubId,        Cat_NGC_Type,       2000, 0},
//{"IC",          "I",     NUM_IC,       Cat_IC,       Cat_IC_Names,        Cat_IC_SubId,         Cat_IC_Type,        2000, 0},
  {"Select NGC",  "N",     NUM_NGC,      Cat_NGC,      Cat_NGC_Names,       Cat_NGC_SubId,        Cat_NGC_Type,       2000, 0},
  {"Select IC",   "I",     NUM_IC,       Cat_IC,       Cat_IC_Names,        Cat_IC_SubId,         Cat_IC_Type,        2000, 0},
  {"",            "",                 0,         NULL,               NULL,                NULL,   CAT_NONE,              0, 0}
};

// --------------------------------------------------------------------------------
// Catalog Manager

// initialization
void CatMgr::setLat(double lat) {
  _lat=lat;
  if (lat<9999) {
    _cosLat=cos(lat/Rad);
    _sinLat=sin(lat/Rad);
  }
}

// Set Local Sidereal Time, and number of milliseconds
void CatMgr::setLstT0(double lstT0) {
  _lstT0=lstT0;
  _lstMillisT0=millis();
}

bool CatMgr::isInitialized() {
  return ((_lat<9999) && (_lstT0!=0));
}

// Get Local Sidereal Time, converted from hours to degrees
double CatMgr::lstDegs() {
  return (lstHours()*15.0);
}

// Get Local Sidereal Time, in hours, and adjust for time inside the menus
double CatMgr::lstHours() {
  double msSinceT0=(unsigned long)(millis()-_lstMillisT0);
  // Convert from Solar to Sidereal
  double siderealSecondsSinceT0=(msSinceT0/1000.0)*1.00277778;
  return _lstT0+siderealSecondsSinceT0/3600.0;
}

// number of catalogs available
int CatMgr::numCatalogs() {
  for (int i=0; i<MaxCatalogs; i++) {
    if (catalog[i].NumObjects==0) return i;
  }
  return 32;
}

gen_star_t*      _genStarCatalog     = NULL;
gen_star_comp_t* _genStarCompCatalog = NULL;
ext_star_t*      _extStarCatalog     = NULL;
dso_t*           _dsoCatalog         = NULL;
dso_comp_t*      _dsoCompCatalog     = NULL;
dso_vcomp_t*     _dsoVCompCatalog    = NULL;

// handle catalog selection (0..n)
void CatMgr::select(int number) {
  _genStarCatalog     =NULL;
  _genStarCompCatalog =NULL;
  _extStarCatalog     =NULL;
  _dsoCatalog         =NULL;
  _dsoCompCatalog     =NULL;
  _dsoVCompCatalog    =NULL;
  if ((number<0) || (number>=numCatalogs())) number=-1; // invalid catalog?
  _selected=number;
  if (_selected>=0) {
    if (catalog[_selected].CatalogType==CAT_GEN_STAR)      _genStarCatalog    =(gen_star_t*)catalog[_selected].Objects; else
    if (catalog[_selected].CatalogType==CAT_GEN_STAR_COMP) _genStarCompCatalog=(gen_star_comp_t*)catalog[_selected].Objects; else
    if (catalog[_selected].CatalogType==CAT_EXT_STAR)      _extStarCatalog    =(ext_star_t*)catalog[_selected].Objects; else
    if (catalog[_selected].CatalogType==CAT_DSO)           _dsoCatalog        =(dso_t*)catalog[_selected].Objects; else
    if (catalog[_selected].CatalogType==CAT_DSO_COMP)      _dsoCompCatalog    =(dso_comp_t*)catalog[_selected].Objects; else
    if (catalog[_selected].CatalogType==CAT_DSO_VCOMP)     _dsoVCompCatalog   =(dso_vcomp_t*)catalog[_selected].Objects; else _selected=-1;
  }
}

//  Get active catalog type
CAT_TYPES CatMgr::catalogType()  {
  return catalog[_selected].CatalogType;
}

// Star catalog?
bool CatMgr::isStarCatalog() {
  return (catalogType()==CAT_GEN_STAR) || (catalogType()==CAT_GEN_STAR_COMP) || (catalogType()==CAT_EXT_STAR);
}

// Dso catalog?
bool CatMgr::isDsoCatalog()  {
  return (catalogType()==CAT_DSO) || (catalogType()==CAT_DSO_COMP) || (catalogType()==CAT_DSO_VCOMP);
}

// Get active catalog title
const char* CatMgr::catalogTitle() {
  if (_selected<0) return "";
  return catalog[_selected].Title;
}

// Get active catalog prefix
const char* CatMgr::catalogPrefix() {
  if (_selected<0) return "";
  return catalog[_selected].Prefix;
}

// catalog filtering
void CatMgr::filtersClear() {
  _fm=FM_NONE;
}

void CatMgr::filterAdd(int fm) {
  _fm|=fm;
}

void CatMgr::filterAdd(int fm, int param) {
  _fm|=fm;
  if (fm&FM_CONSTELLATION) _fm_con=param;
  if (fm&FM_OBJ_TYPE) _fm_obj_type=param;
}

// select catalog record
bool CatMgr::setIndex(int index) {
  if (_selected<0) return false;
  catalog[_selected].Index=index;
  decIndex();
  return incIndex();
}

int CatMgr::getIndex() {
  return catalog[_selected].Index;
}

int CatMgr::getMaxIndex() {
  return catalog[_selected].NumObjects-1;
}

bool CatMgr::incIndex() {
  int i=getMaxIndex()+1;
  do {
    i--;
    catalog[_selected].Index++;
    if (catalog[_selected].Index>getMaxIndex()) catalog[_selected].Index=0;
  } while (isFiltered() && (i>0));
  if (isFiltered()) return false; else return true;
}

bool CatMgr::decIndex() {
  int i=getMaxIndex()+1;
  do {
    i--;
    catalog[_selected].Index--;
    if (catalog[_selected].Index<0) catalog[_selected].Index=getMaxIndex();
  } while (isFiltered() && (i>0));
  if (isFiltered()) return false; else return true;
}

// get catalog contents

// RA, converted from hours to degrees
double CatMgr::ra() {
  return rah()*15.0;
}

// RA in hours
double CatMgr::rah() {
  if (_selected<0) return 0;
  if (catalogType()==CAT_GEN_STAR)      return _genStarCatalog[catalog[_selected].Index].RA; else
  if (catalogType()==CAT_GEN_STAR_COMP) return _genStarCompCatalog[catalog[_selected].Index].RA/2730.6666666666666; else
  if (catalogType()==CAT_EXT_STAR)      return _extStarCatalog[catalog[_selected].Index].RA; else
  if (catalogType()==CAT_DSO)           return _dsoCatalog[catalog[_selected].Index].RA; else
  if (catalogType()==CAT_DSO_COMP)      return _dsoCompCatalog[catalog[_selected].Index].RA/2730.6666666666666; else
  if (catalogType()==CAT_DSO_VCOMP)     return _dsoVCompCatalog[catalog[_selected].Index].RA/2730.6666666666666; else return 0;
}

// HA in degrees
double CatMgr::ha() {
  if (!isInitialized()) return 0;
  double h=(lstDegs()-ra());
  while (h>180.0) h-=360.0;
  while (h<-180.0) h+=360.0;
  return h;
}

// Get RA of an object, in hours, as HH:MM:SS
void CatMgr::raHMS(uint8_t& h, uint8_t& m, uint8_t& s) {
  double f=rah();
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
  if (catalogType()==CAT_GEN_STAR)      return _genStarCatalog[catalog[_selected].Index].DE; else
  if (catalogType()==CAT_GEN_STAR_COMP) return _genStarCompCatalog[catalog[_selected].Index].DE/364.07777777777777; else
  if (catalogType()==CAT_EXT_STAR)      return _extStarCatalog[catalog[_selected].Index].DE; else
  if (catalogType()==CAT_DSO)           return _dsoCatalog[catalog[_selected].Index].DE; else
  if (catalogType()==CAT_DSO_COMP)      return _dsoCompCatalog[catalog[_selected].Index].DE/364.07777777777777; else
  if (catalogType()==CAT_DSO_VCOMP)     return _dsoVCompCatalog[catalog[_selected].Index].DE/364.07777777777777; else return 0;
}

// Declination as DD:MM:SS
void CatMgr::decDMS(short& d, uint8_t& m, uint8_t& s) {
  double f=dec();
  double d1, m1, s1;

  d1=floor(f);
  m1=(f-d1)*60;
  s1=(m1-floor(m1))*60.0;

  d = (int)d1;
  if (f<0) d *= -1;

  m = (int)m1;
  s = (int)s1;
}

// Epoch for catalog
int CatMgr::epoch() {
  if (_selected<0) return -1;
  return catalog[_selected].Epoch;
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

// apply refraction, this converts from the "Topocentric" to "Observed" place for higher accuracy, RA in hours Dec in degrees
void CatMgr::topocentricToObservedPlace(float *RA, float *Dec) {
  if (isInitialized()) {
    double Alt,Azm;
    double r=*RA*15.0;
    double d=*Dec;
    EquToHor(r,d,&Alt,&Azm);
    Alt = Alt+TrueRefrac(Alt) / 60.0;
    HorToEqu(Alt,Azm,&r,&d);
    *RA=r/15.0; *Dec=d;
  }
}

// Magnitude of an object
float CatMgr::magnitude() {
  if (_selected<0) return 99.9;
  if (catalogType()==CAT_GEN_STAR)      return _genStarCatalog[catalog[_selected].Index].Mag/100.0; else
  if (catalogType()==CAT_GEN_STAR_COMP) { int m=_genStarCompCatalog[catalog[_selected].Index].Mag; if (m==255) return 99.9; else return (m/10.0)-2.5; } else
  if (catalogType()==CAT_EXT_STAR)      return _extStarCatalog[catalog[_selected].Index].Mag/100.0; else
  if (catalogType()==CAT_DSO)           return _dsoCatalog[catalog[_selected].Index].Mag/100.0; else
  if (catalogType()==CAT_DSO_COMP)      { int m=_dsoCompCatalog[catalog[_selected].Index].Mag; if (m==255) return 99.9; else return (m/10.0)-2.5; } else
  if (catalogType()==CAT_DSO_VCOMP)     { int m=_dsoVCompCatalog[catalog[_selected].Index].Mag; if (m==255) return 99.9; else return (m/10.0)-2.5; } else return 0;
}

// Constellation number
byte CatMgr::constellation() {
  if (_selected<0) return 89;
  if (catalogType()==CAT_GEN_STAR)      return _genStarCatalog[catalog[_selected].Index].Cons; else
  if (catalogType()==CAT_GEN_STAR_COMP) return _genStarCompCatalog[catalog[_selected].Index].Cons; else
  if (catalogType()==CAT_EXT_STAR)      return _extStarCatalog[catalog[_selected].Index].Cons; else
  if (catalogType()==CAT_DSO)           return _dsoCatalog[catalog[_selected].Index].Cons; else
  if (catalogType()==CAT_DSO_COMP)      return _dsoCompCatalog[catalog[_selected].Index].Cons; else
  if (catalogType()==CAT_DSO_VCOMP)     return _dsoVCompCatalog[catalog[_selected].Index].Cons; else return 0;
}

// Constellation string
const char* CatMgr::constellationStr() {
  return Txt_Constellations[constellation()];
}

// Constellation string, from constellation number
const char* CatMgr::constellationCodeToStr(int code) {
  if ((code>=0) && (code<=87)) return Txt_Constellations[code]; else return "";
}

// Object type code
byte CatMgr::objectType() {
  if (_selected<0) return -1;
  if (catalogType()==CAT_GEN_STAR)      return 2; else
  if (catalogType()==CAT_GEN_STAR_COMP) return 2; else
  if (catalogType()==CAT_EXT_STAR)      return 2; else
  if (catalogType()==CAT_DSO)           return _dsoCatalog[catalog[_selected].Index].Obj_type; else
  if (catalogType()==CAT_DSO_COMP)      return _dsoCompCatalog[catalog[_selected].Index].Obj_type; else
  if (catalogType()==CAT_DSO_VCOMP)     return _dsoVCompCatalog[catalog[_selected].Index].Obj_type; else return -1;
}

// Object type string
const char* CatMgr::objectTypeStr() {
  int t=objectType();
  if ((t>=0) && (t<=20)) return Txt_Object_Type[t]; else return "";
}

// Object Type string, from code number
const char* CatMgr::objectTypeCodeToStr(int code) {
  if ((code>=0) && (code<=20)) return Txt_Object_Type[code]; else return "";
}

// Object name code (encoded by Has_name.)  Returns -1 if the object doesn't have a name code.
int CatMgr::objectName() {
  if (_selected<0) return -1;

  // does it have a name? if not just return
  if (catalogType()==CAT_GEN_STAR)      { if (!_genStarCatalog[catalog[_selected].Index].Has_name) return -1; } else
  if (catalogType()==CAT_GEN_STAR_COMP) { if (!_genStarCompCatalog[catalog[_selected].Index].Has_name) return -1; } else
  if (catalogType()==CAT_EXT_STAR)      { if (!_extStarCatalog[catalog[_selected].Index].Has_name) return -1; } else
  if (catalogType()==CAT_DSO)           { if (!_dsoCatalog[catalog[_selected].Index].Has_name) return -1; } else
  if (catalogType()==CAT_DSO_COMP)      { if (!_dsoCompCatalog[catalog[_selected].Index].Has_name) return -1; } else
  if (catalogType()==CAT_DSO_VCOMP)     { if (!_dsoVCompCatalog[catalog[_selected].Index].Has_name) return -1; } else return -1;

  // find the code
  int result=-1;
  int j=catalog[_selected].Index;
  if (j>getMaxIndex()) j=-1;
  if (j<0) return -1;
  if (catalogType()==CAT_GEN_STAR)      { for (int i=0; i<=j; i++) { if (_genStarCatalog[i].Has_name) result++; } } else
  if (catalogType()==CAT_GEN_STAR_COMP) { for (int i=0; i<=j; i++) { if (_genStarCompCatalog[i].Has_name) result++; } } else
  if (catalogType()==CAT_EXT_STAR)      { for (int i=0; i<=j; i++) { if (_extStarCatalog[i].Has_name) result++; } } else
  if (catalogType()==CAT_DSO)           { for (int i=0; i<=j; i++) { if (_dsoCatalog[i].Has_name) result++; } } else
  if (catalogType()==CAT_DSO_COMP)      { for (int i=0; i<=j; i++) { if (_dsoCompCatalog[i].Has_name) result++; } } else
  if (catalogType()==CAT_DSO_VCOMP)     { for (int i=0; i<=j; i++) { if (_dsoVCompCatalog[i].Has_name) result++; } } else return -1;

  return result;
}

// Object name type string
const char* CatMgr::objectNameStr() {
  if (_selected<0) return "";
  int elementNum=objectName();
  if (elementNum>=0) return getElementFromString(catalog[_selected].ObjectNames,elementNum); else return "";
}

// Object Id
unsigned int CatMgr::primaryId() {
  if (_selected<0) return -1;
  if (catalogType()==CAT_GEN_STAR)      return _genStarCatalog[catalog[_selected].Index].Bayer+1; else
  if (catalogType()==CAT_GEN_STAR_COMP) return _genStarCompCatalog[catalog[_selected].Index].Bayer+1; else
  if (catalogType()==CAT_EXT_STAR)      return _extStarCatalog[catalog[_selected].Index].Bayer+1; else
  if (catalogType()==CAT_DSO)           return _dsoCatalog[catalog[_selected].Index].Obj_id; else
  if (catalogType()==CAT_DSO_COMP)      return _dsoCompCatalog[catalog[_selected].Index].Obj_id; else
  if (catalogType()==CAT_DSO_VCOMP)     return catalog[_selected].Index+1; else return -1;
}

// Object note code (encoded by Has_note.)  Returns -1 if the object doesn't have a note code.
int CatMgr::subId() {
  if (_selected<0) return -1;

  // does it have a note? if not just return
  if (catalogType()==CAT_GEN_STAR)      { if (!_genStarCatalog[catalog[_selected].Index].Has_subId) return -1; } else
  if (catalogType()==CAT_GEN_STAR_COMP) { if (!_genStarCatalog[catalog[_selected].Index].Has_subId) return -1; } else
  if (catalogType()==CAT_EXT_STAR)      { if (!_extStarCatalog[catalog[_selected].Index].Has_subId) return -1; } else
  if (catalogType()==CAT_DSO)           { if (!_dsoCatalog[catalog[_selected].Index].Has_subId) return -1; } else
  if (catalogType()==CAT_DSO_COMP)      { if (!_dsoCompCatalog[catalog[_selected].Index].Has_subId) return -1; } else
  if (catalogType()==CAT_DSO_VCOMP)     { if (!_dsoCompCatalog[catalog[_selected].Index].Has_subId) return -1; } else return -1;

  // find the code
  int result=-1;
  int j=catalog[_selected].Index;
  if (j>getMaxIndex()) j=-1;
  if (j<0) return -1;
  if (catalogType()==CAT_GEN_STAR)      { for (int i=0; i<=j; i++) { if (_genStarCatalog[i].Has_subId) result++; } } else
  if (catalogType()==CAT_GEN_STAR_COMP) { for (int i=0; i<=j; i++) { if (_genStarCompCatalog[i].Has_subId) result++; } } else
  if (catalogType()==CAT_EXT_STAR)      { for (int i=0; i<=j; i++) { if (_extStarCatalog[i].Has_subId) result++; } } else
  if (catalogType()==CAT_DSO)           { for (int i=0; i<=j; i++) { if (_dsoCatalog[i].Has_subId) result++; } } else
  if (catalogType()==CAT_DSO_COMP)      { for (int i=0; i<=j; i++) { if (_dsoCompCatalog[i].Has_subId) result++; } } else
  if (catalogType()==CAT_DSO_VCOMP)     { for (int i=0; i<=j; i++) { if (_dsoVCompCatalog[i].Has_subId) result++; } } else return -1;

  return result;
}

// Object note string
const char* CatMgr::subIdStr() {
  if (_selected<0) return "";
  int elementNum=subId();
  if (elementNum>=0) return getElementFromString(catalog[_selected].ObjectSubIds,elementNum); else return "";
}

// support functions

// checks to see if the currently selected object is filtered (returns true if filtered out)
bool CatMgr::isFiltered() {
  if (!isInitialized()) return false;
  if (_fm==FM_NONE) return false;
  if (_fm&FM_ABOVE_HORIZON) { if (alt()<0.0) return true; }
  if (_fm&FM_ALIGN_ALL_SKY) {
    if (alt()<10.0) return true;      // minimum 10 degrees altitude
    if (abs(dec())>85.0) return true; // minimum 5 degrees from the pole (for accuracy)
  }
  if (_fm&FM_CONSTELLATION) {
    if (constellation()!=_fm_con) return true;
  }
  if (isDsoCatalog() && (_fm&FM_OBJ_TYPE)) {
    if (objectType()!=_fm_obj_type) return true;
  }
  return false;
}  

// returns elementNum 'th element from the comma delimited string where the 0th element is the first etc.
const char* CatMgr::getElementFromString(const char *data, int elementNum) {
  static char result[40] = "";

  // find the string start index
  bool found=false;
  unsigned int j=0;
  int n=elementNum;
  unsigned int len=strlen(data);
  for (unsigned int i=0; i<len; i++) {
    if (n==0) { j=i; found=true; break; }
    if (data[i]==';') { n--; }
  }

  // return the string
  if (found) {
    int k=0;
    for (unsigned int i=j; i<len; i++) {
      result[k++]=data[i];
      if (result[k-1]==';') { result[k-1]=0; break; }
      if (i==len-1) { result[k]=0; break; }
    }
    return result;
  } else return "";
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

// convert equatorial coordinates to horizon, in degrees
void CatMgr::EquToHor(double RA, double Dec, double *Alt, double *Azm) {
  double HA=lstDegs()-RA;
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

CatMgr cat_mgr;
