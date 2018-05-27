#ifndef WIFIBLUETOOTH_H
#define WIFIBLUETOOTH_H


#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFiAP.h>
//#include "Encoders.h"
#include "MountStatus.h"
#define Product "OnEsp"
#define FirmwareDate          __DATE__
#define FirmwareTime          __TIME__
#define FirmwareVersionMajor  "1"
#define FirmwareVersionMinor  "4"
#define FirmwareVersionPatch  "d"
#define Version FirmwareVersionMajor "." FirmwareVersionMinor FirmwareVersionPatch


// -----------------------------------------------------------------------------------
// Constants

// The settings below are for initialization only, afterward they are stored and recalled from EEPROM and must
// be changed in the web interface OR with a reset (for initialization again) as described in the Config.h comments
#if SERIAL_BAUD<=28800
#define TIMEOUT_WEB 60
#define TIMEOUT_CMD 60
#else
#define TIMEOUT_WEB 15
#define TIMEOUT_CMD 30
#endif

#define DEFAULT_AJAX_RATE "5"        // normally 5 seconds between updates
#define DEFAULT_FAST_AJAX_RATE "1"   // fast update is 1 second/update
#define DEFAULT_AJAX_SHED_TIME "15"  // time before return to normal update rate

extern const char* html_headB;
extern const char* html_headerPec;
extern const char* html_headerIdx;
extern const char* html_headE;
extern const char* html_bodyB;

extern const char* html_main_cssB;
extern const char* html_main_css1;
extern const char* html_main_css2;
extern const char* html_main_css3;
extern const char* html_main_css4;
extern const char* html_main_css5;
extern const char* html_main_css6;
extern const char* html_main_css7;
extern const char* html_main_css8;
extern const char* html_main_css_control1;
extern const char* html_main_css_control2;
extern const char* html_main_css_control3;
extern const char* html_main_cssE;

extern const char* html_onstep_header1;
extern const char* html_onstep_header2;
extern const char* html_onstep_header3;
extern const char* html_onstep_header4;

extern const char* html_links1S;
extern const char* html_links1N;
extern const char* html_links2S;
extern const char* html_links2N;
extern const char* html_links3S;
extern const char* html_links3N;
extern const char* html_links4S;
extern const char* html_links4N;
extern const char* html_links5S;
extern const char* html_links5N;
extern const char* html_links6S;
extern const char* html_links6N;

// Javascript for Ajax
// be sure to define "var ajaxPage='control.txt';" etc.
extern const char* html_ajax_active;


class wifibluetooth
{
  enum Responding { R_NONE, R_ONE, R_BOOL, R_STRING };

  static MountStatus mountStatus;
  //Encoders encoders;
  static int WebTimeout; 
  static int CmdTimeout; 

#define Default_Password "password"
  static char masterPassword[40];

  static bool accessPointEnabled;
  static bool stationEnabled;
  static bool stationDhcpEnabled;

  static char wifi_sta_ssid[40];
  static char wifi_sta_pwd[40];

  static IPAddress wifi_sta_ip;
  static IPAddress wifi_sta_gw;
  static IPAddress wifi_sta_sn;

  static char wifi_ap_ssid[40];
  static char wifi_ap_pwd[40];
  static byte wifi_ap_ch;

  static IPAddress wifi_ap_ip;
  static IPAddress wifi_ap_gw;
  static IPAddress wifi_ap_sn;

  static ESP8266WebServer server;

  static WiFiServer* cmdSvr;
  static WiFiClient cmdSvrClient;
  static const char* HighSpeedCommsStr(long baud);
  static void processConfigurationGet();
  static void handleConfiguration();
  static void handleRoot();
  static void handleSettings();

  static void settingsAjax();
  static void processSettingsGet();
  static void controlAjax();
  static void processControlGet();
  static void guideAjax();
  static void handleControl();
  static void handlePec();
  static void pecAjax();
  static void processPecGet();
  static void handleWifi();
  static void handleNotFound();
  static void processWifiGet();

  static bool sendCommand(const char command[], char response[], Responding responding = R_STRING);
  static char serialRecvFlush();
  static boolean doubleToDms(char *reply, double *f, boolean fullRange, boolean signPresent);
  static boolean atoi2(char *a, int *i);
  static byte readBytesUntil2(char character, char buffer[], int length, boolean* characterFound, long timeout);
  static boolean readLX200Bytes(char* command, char* recvBuffer, long timeOutMs);
  static void cl();
  static int hexToInt(String s);

  // write int numbers into EEPROM at position i (2 bytes)
  static void EEPROM_writeInt(int i, int j);
  // read int numbers from EEPROM at position i (2 bytes)
  static int EEPROM_readInt(int i);
  // write 4 byte variable into EEPROM at position i (4 bytes)
  static void EEPROM_writeQuad(int i, byte *v);
  // read 4 byte variable from EEPROM at position i (4 bytes)
  static void EEPROM_readQuad(int i, byte *v);
  // write String into EEPROM at position i (40 bytes)
  static void EEPROM_writeString(int i, char l[]);
  // read String from EEPROM at position i (40 bytes)
  static void EEPROM_readString(int i, char l[]);
  // write 4 byte float into EEPROM at position i (4 bytes)
  static void EEPROM_writeFloat(int i, float f);
  // read 4 byte float from EEPROM at position i (4 bytes)
  static float EEPROM_readFloat(int i);
  // write 4 byte long into EEPROM at position i (4 bytes)
  static void EEPROM_writeLong(int i, long l);
  // read 4 byte long from EEPROM at position i (4 bytes)
  static long EEPROM_readLong(int i);
public:
  static void setup();
  static void update();
};
#endif