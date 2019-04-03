#pragma once

// ----------------------------------------------------------
// Do not change anything in the structs or arrays below, since they
// have to be in sync with the extraction scripts.

// Struct for catalog header
typedef struct {
  const char           Title[10];
  const char           Prefix[8];
  const unsigned short NumObjects;
  void*                Objects;
  const char*          ObjectNames;
  bool                 isStarCatalog;
  int                  Epoch;
  int                  Index;
} catalog_t;

#pragma pack(1)

// Struct for Deep Space Objects (Messier, Herschel, ..etc.)
typedef struct {
  const char           HasName: 1;
  const char           Cons: 7;
  const char           Obj_type;
  const unsigned short Obj_id;
  const signed   short Mag;
  const float          RA;
  const float          DE;
} dso_t;

// Reduced size equal
typedef struct {
  const char           HasName: 1;
  const char           Cons: 7;
  const char           Obj_type;
  const unsigned short Obj_id;
  const char           Mag;
  const unsigned short RA;
  const signed short   DE;
} dso__t;

// Struct for stars
typedef struct {
  const char           HasName: 1;
  const char           Cons: 7;
  const char           Bayer;
  const signed   short Mag;
  const float          RA;
  const float          DE;
} star_t;

// Reduced size equal
typedef struct {
  const char           HasName: 1;
  const char           Cons: 7;
  const char           Bayer;
  const char           Mag;
  const unsigned short RA;
  const signed short   DE;
} star__t;

#pragma pack()

// Bayer designation, the Greek letter for each star within a constellation
const char* Txt_Bayer[24] = {
  "Alp","Bet","Gam","Del","Eps","Zet","Eta","The","Iot","Kap","Lam","Mu" ,"Nu","Xi","Omi","Pi","Rho","Sig","Tau","Ups","Phi","Chi","Psi","Ome"
};

// Type of object, in the Open NGC database. Do not change the
// order of this, since it has to match what is in Open NGC
const char * Txt_Object_Type[21] = {
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
