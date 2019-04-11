#pragma once

// maximum number of catalogs supported
#define MaxCatalogs 64

// ----------------------------------------------------------
// Do not change anything in the structs or arrays below, since they
// have to be in sync with the extraction scripts.

// Struct for catalog header
typedef struct {
  const char           Title[16];
  const char           Prefix[8];
  const unsigned short NumObjects;
  const void*          Objects;
  const char*          ObjectNames;
  const char*          ObjectSubIds;
  CAT_TYPES            CatalogType;
  int                  Epoch;
  int                  Index;
} catalog_t;

#pragma pack(1)

// Struct for Deep Space Objects (Messier, Herschel, ..etc.)
typedef struct {
  const unsigned char  Has_name: 1;
  const unsigned char  Cons: 7;
  const unsigned char  Obj_type: 7;
  const unsigned char  Has_subId: 1;
  const unsigned short Obj_id;
  const signed   short Mag;
  const float          RA;
  const float          DE;
} dso_t; // 14 bytes/rec

typedef struct {
  const unsigned char  Has_name: 1;
  const unsigned char  Cons: 7;
  const unsigned char  Obj_type: 7;
  const unsigned char  Has_subId: 1;
  const unsigned short Obj_id;
  const unsigned char  Mag;
  const unsigned short RA;
  const signed   short DE;
} dso_comp_t; // 9 bytes/rec (as above except except Mag/RA/Dec use smaller data types)

typedef struct {
  const unsigned char  Has_name: 1;
  const unsigned char  Cons: 7;
  const unsigned char  Obj_type: 7;
  const unsigned char  Has_subId: 1;
  const unsigned char  Mag;
  const unsigned short RA;
  const signed   short DE;
} dso_vcomp_t; // 7 bytes/rec (as above except Obj_id is replaced by the record count)

// Struct for general star catalog
typedef struct {
  const unsigned char  Has_name: 1;
  const unsigned char  Cons: 7;
  const unsigned char  BayerFlam: 8;
  const unsigned char  Has_subId: 1;
  const signed   short Mag;
  const float          RA;
  const float          DE;
} gen_star_t; // 13 bytes/rec

// Struct for general star catalog compact
typedef struct {
  const unsigned char  Has_name: 1;
  const unsigned char  Cons: 7;
  const unsigned char  BayerFlam: 7;
  const unsigned char  Has_subId: 1;
  const unsigned char  Mag;
  const unsigned short RA;
  const signed   short DE;
} gen_star_comp_t; // 7 bytes/rec (as above except except Mag/RA/Dec use smaller data types)

// Struct for double star catalog
typedef struct {
  const unsigned long  Has_name: 1;
  const unsigned long  Cons: 7;
  const unsigned long  BayerFlam: 8;
  const unsigned long  Has_subId: 1;

  const unsigned long  Sep: 14;      // separation of components 0.0 to 999.8" (0 to 9998), 999.9 (9999) = Unknown
  const unsigned int   PA: 9;        // position angle in degrees
  const signed short   Mag2;

  const signed short   Mag;
  const float          RA;
  const float          DE;
} dbl_star_t; // 18 bytes per record

// Struct for variable star catalog
typedef struct {
  const unsigned long  Has_name: 1;
  const unsigned long  Cons: 7;
  const unsigned long  BayerFlam: 8;
  const unsigned long  Has_subId: 1;

  const unsigned long  Period: 15;   // Period 0.00 to 9.99 days (0 to 999) period 10.0 to 3276.6 days (10000 to 32766), 32767 = Unknown
  const signed short   Mag2;

  const signed short   Mag;
  const float          RA;
  const float          DE;
} var_star_t; // 14 bytes per record

#pragma pack()

// Bayer designation, the Greek letter for each star within a constellation
const char* Txt_Bayer[25] = {
  "Alp","Bet","Gam","Del","Eps","Zet","Eta","The","Iot","Kap","Lam","Mu","Nu","Xi","Omi","Pi","Rho","Sig","Tau","Ups","Phi","Chi","Psi","Ome","?"
};

// Type of object, in the Open NGC database. Do not change the
// order of this, since it has to match what is in Open NGC
const char* Txt_Object_Type[21] = {
  "Galaxy",        "Open Cluster",   "Star",           "Double Star", "Other",      "Galaxy Pair",      "Galaxy Triplet",
  "Galaxy Group",  "Globular Clstr", "Planetary Nbla", "Nebula",      "Hii Region", "Cluster + Nebula", "Asterism",
  "Reflectn Nbla", "SuperNov Rmnt",  "Emission Nbla",  "Non Existant","Nova",       "Duplicate",        "Dark Nebula"
};

// Constellation abbreviation, alphabetical order
const char* Txt_Constellations[89] = {
  "And","Ant","Aps","Aql","Aqr","Ara","Ari","Aur","Boo","CMa","CMi","CVn","Cae","Cam","Cap","Car","Cas","Cen","Cep","Cet","Cha","Cir",
  "Cnc","Col","Com","CrA","CrB","Crt","Cru","Crv","Cyg","Del","Dor","Dra","Equ","Eri","For","Gem","Gru","Her","Hor","Hya","Hyi","Ind",
  "LMi","Lac","Leo","Lep","Lib","Lup","Lyn","Lyr","Men","Mic","Mon","Mus","Nor","Oct","Oph","Ori","Pav","Peg","Per","Phe","Pic","PsA",
  "Psc","Pup","Pyx","Ret","Scl","Sco","Sct","Ser","Sex","Sge","Sgr","Tau","Tel","TrA","Tri","Tuc","UMa","UMi","Vel","Vir","Vol","Vul"," ---"
};
