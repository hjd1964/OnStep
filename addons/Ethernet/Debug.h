// -----------------------------------------------------------------------------------
// Helper macros for debugging, with less typing

#if DEBUG != OFF
  #if DEBUG == REMOTE
    // Use VF or VLF for strings (supports embedded spaces)
    void debugPrint(const char s[]) { char s1[255]; strcpy(s1,s); for (unsigned int i=0; i < strlen(s1); i++) if (s1[i]==' ') s1[i]='_'; Ser.print(s1); }
    #define D(x)       { Ser.print(":EC"); Ser.print(x); Ser.print("#"); }
    #define DF(x)      { Ser.print(":EC"); debugPrint(x); Ser.print("#"); }
    #define DL(x)      { Ser.print(":EC"); Ser.print(x); Ser.print("&#"); }
    #define DLF(x)     { Ser.print(":EC"); debugPrint(x); Ser.print("&#"); }
  #else
    #define D(x)       DebugSer.print(x)
    #define DF(x)      DebugSer.print(F(x))
    #define DL(x)      DebugSer.println(x)
    #define DLF(x)     DebugSer.println(F(x))
  #endif
#else
  #define D(x)
  #define DF(x)
  #define DL(x)
  #define DLF(x)
#endif
#if DEBUG == VERBOSE || DEBUG == REMOTE
  #define V(x)       D(x)
  #define VF(x)      DF(x)
  #define VL(x)      DL(x)
  #define VLF(x)     DLF(x)
#else
  #define V(x)
  #define VF(x)
  #define VL(x)
  #define VLF(x)
#endif
