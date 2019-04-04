#pragma once

// maximum number of catalogs supported
#define MaxCatalogs 64

// _ON to use the compact record type for Stars or DSO's
#define USE_COMPACT_STARS_OFF
#define USE_COMPACT_DSOS_OFF

// ----------------------------------------------------------
// Do not change anything in the structs or arrays below, since they
// have to be in sync with the extraction scripts.

enum CAT_TYPES {CAT_NONE, CAT_GEN_STAR, CAT_EXT_STAR, CAT_DSO};

// Struct for catalog header
typedef struct {
  const char           Title[10];
  const char           Prefix[8];
  const unsigned short NumObjects;
  void*                Objects;
  const char*          ObjectNames;
  const char*          ObjectNotes;
  CAT_TYPES            CatalogType;
  int                  Epoch;
  int                  Index;
} catalog_t;

#pragma pack(1)

// Struct for Deep Space Objects (Messier, Herschel, ..etc.)
#ifndef USE_COMPACT_DSOS_ON
  typedef struct {
    const char           Has_name: 1;
    const char           Cons: 7;
    const char           Obj_type: 7;
    const char           Has_note: 1;
    const unsigned short Obj_id;
    const signed   short Mag;
    const float          RA;
    const float          DE;
  } dso_t;
#else
  typedef struct {
    const char           Has_name: 1;
    const char           Cons: 7;
    const char           Obj_type: 7;
    const char           Has_note: 1;
    const unsigned short Obj_id;
    const char           Mag;
    const unsigned short RA;
    const signed short   DE;
  } dso_t;
#endif

// Struct for general star catalog
#ifndef USE_COMPACT_STARS_ON
  typedef struct {
    const char           Has_name: 1;
    const char           Cons: 7;
    const char           Bayer: 5;
    const char           Has_note: 1;
    const signed short   Mag;
    const float          RA;
    const float          DE;
  } star_t;
#else
  typedef struct {
    const char           Has_name: 1;
    const char           Cons: 7;
    const char           Bayer: 5;
    const char           Has_note: 1;
    const char           Mag;
    const unsigned short RA;
    const signed short   DE;
  } star_t;
#endif

// Struct for extended star catalog
typedef struct {
  const char           Has_name: 1;
  const char           Cons: 7;
  const char           Bayer: 5;
  const char           Has_note: 1;
  const signed short   Mag;

                                     // total for this section is 8 bytes/rec
  const unsigned long  Class: 5;     // Harvard classification none,O,B,A,F,G,K,M... (0-32)
  const unsigned long  SubClass:4;   // Harvard classification 0 to 9 (0-16)
  const unsigned long  Star_type: 3; // n/a, variable, double, triple, etc. (0-7)
  const unsigned long  Sep1: 10;     // seperation component A/B 0.1 to 100.0" where 0 means n/a, 1001 means >100.0" (0-1023)
  const unsigned long  Sep1: 10;     // seperation component A/C 0.1 to 100.0" where 0 means n/a, 1001 means >100.0" (0-1023)
  const float          Period;       // in days

  const float          RA;
  const float          DE;
} xstar_t;

#pragma pack()

// Bayer designation, the Greek letter for each star within a constellation
const char* Txt_Bayer[24] = {
  "Alp","Bet","Gam","Del","Eps","Zet","Eta","The","Iot","Kap","Lam","Mu" ,"Nu","Xi","Omi","Pi","Rho","Sig","Tau","Ups","Phi","Chi","Psi","Ome"
};

// Type of object, in the Open NGC database. Do not change the
// order of this, since it has to match what is in Open NGC
const char* Txt_Object_Type[21] = {
  "Galaxy",          "Open Cluster",     "Star",             "Double Star", "Other",      "Galaxy Pair",      "Galaxy Triplet",
  "Galaxy Group",    "Globular Cluster", "Planetary Nebula", "Nebula",      "Hii Region", "Cluster + Nebula", "Asterism",
  "Reflectn Nebula", "SuperNova Rmnnt",  "Emission Nebula",  "NonExist",    "Nova",       "Duplicate",        "Dark Nebula"
};

// Constellation abbreviation, alphabetical order
const char* Txt_Constellations[89] = {
  "And","Ant","Aps","Aql","Aqr","Ara","Ari","Aur","Boo","CMa","CMi","CVn","Cae","Cam","Cap","Car","Cas","Cen","Cep","Cet","Cha","Cir",
  "Cnc","Col","Com","CrA","CrB","Crt","Cru","Crv","Cyg","Del","Dor","Dra","Equ","Eri","For","Gem","Gru","Her","Hor","Hya","Hyi","Ind",
  "LMi","Lac","Leo","Lep","Lib","Lup","Lyn","Lyr","Men","Mic","Mon","Mus","Nor","Oct","Oph","Ori","Pav","Peg","Per","Phe","Pic","PsA",
  "Psc","Pup","Pyx","Ret","Scl","Sco","Sct","Ser","Sex","Sge","Sgr","Tau","Tel","TrA","Tri","Tuc","UMa","UMi","Vel","Vir","Vol","Vul","Unknown"
};
