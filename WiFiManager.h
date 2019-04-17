/**************************************************************
   WiFiManager is a library for the ESP8266/Arduino platform
   (https://github.com/esp8266/Arduino) to enable easy
   configuration and reconfiguration of WiFi credentials using a Captive Portal
   inspired by:
   http://www.esp8266.com/viewtopic.php?f=29&t=2520
   https://github.com/chriscook8/esp-arduino-apboot
   https://github.com/esp8266/Arduino/tree/master/libraries/DNSServer/examples/CaptivePortalAdvanced
   Built by AlexT https://github.com/tzapu
   Licensed under MIT license
 **************************************************************/

#ifndef WiFiManager_h
#define WiFiManager_h

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#else
#include <WiFi.h>
#include <WebServer.h>
#endif
#include <DNSServer.h>
#include <memory>

#if defined(ESP8266)
extern "C"
{
#include "user_interface.h"
}
#define ESP_getChipId() (ESP.getChipId())
#else
#include <esp_wifi.h>
#define ESP_getChipId() ((uint32_t)ESP.getEfuseMac())
#endif

//include the LED Display!
#include "LED_Display_Wrapper.h"

const char HTTP_HEAD[] PROGMEM = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>{v}</title>";
const char HTTP_STYLE[] PROGMEM = "<style>.c{text-align: center;} div,input{padding:5px;font-size:1em;} input{width:95%;} body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;} .q{float: right;width: 64px;text-align: right;} .l{background: url(\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAMAAABEpIrGAAAALVBMVEX///8EBwfBwsLw8PAzNjaCg4NTVVUjJiZDRUUUFxdiZGSho6OSk5Pg4eFydHTCjaf3AAAAZElEQVQ4je2NSw7AIAhEBamKn97/uMXEGBvozkWb9C2Zx4xzWykBhFAeYp9gkLyZE0zIMno9n4g19hmdY39scwqVkOXaxph0ZCXQcqxSpgQpONa59wkRDOL93eAXvimwlbPbwwVAegLS1HGfZAAAAABJRU5ErkJggg==\") no-repeat left center;background-size: 1em;} .tickertape{width:100%;height:50px;background-repeat:no-repeat;background-size:contain;background-position:center;background-image:url(\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAogAAABXAQMAAAC9cCYqAAAABlBMVEX///8AAABVwtN+AAAAAWJLR0QAiAUdSAAAAAlwSFlzAAAZ1gAAGdYBGNHK7QAAAAd0SU1FB+MEERAZCb/b53oAAAP3SURBVFhH7VcxbuMwEKRA4NgEVpvCsL6Q0oVh3VPyBJcuDEdBCpf5koIrUt4XZKRIKyONDhCUm9klZVl2HAN3TRCx0JiiOeLucjlLY4Y2eGDwwOCBwQNf2APl3N2XM2PKeXxfrGDITDAqadPSRMUqtkVFTGxujC2WQA5WE8d+HlCI8D6q7uJf1VqxbPBmnRBtRcba2LJJNiWxTjeFMY64xWBUpzH724BCRMb6Lnmp3o0FPldkfBcUxqgxrmrS510tuMES3K5JH7e1THwEY7zzuBUiTCJTYHztMLoDRjD/adLfZAQ+vpGxOWR8+4zxaI1kCmsUxt4aW0YMwPyGfkyIxsD61o+0HtbS6tJbDT96q1NaDT/e0Z8xUayOqnn8VDIy8/ihZKzXghprRmSVPCDWRIeZtqiBnFmljDV+K+ZCxEm3Y2vya+A1cIoX187kN8bgX8YQpz+ijHgDxArym1GU3XLsyrKfLa4sV5YJESd9jcZFsynCV3sM1uONGLTgY8QHPQBXeSw8+rmxtz7mnzQiARkZ/zlBSaSED8svR5VxgqViaIExIbNmX8CQjcw+zsQ+NCbN8NC+Z8Qk7qC2Jb4jaGWSZ5Qs1L7mUI8Rf47JHDBQnmTcZ+FpxgOmDxmPrL54jT2r28gc+7EXmY/8GPwZrHb0NNqJWPPE1UHFXqxNiROXgx4D4/fE4EfFkDPFLKbvomIa01dRsRREm/Bh8+XEZvTfYsKJUDNBjYj+URFhk2bLNRSLSOUi1skGX0DTnCmgWFAwUS5MdFsolycy7Q7ni30WNjjL2YdyUQ38Gd4y7nCWvyBnusqlX0Tm+x8HWeiqd2UKjF5nAmO8g3J1GakzZxntp4xQro8Yw7HRO3v2VotOn7Iauu1gdatcYY0nIxMhMg8SkZVEpBMZOR8ZGffE06wKkZm3kQlnuBOvhu8UUxcxVMV0RIxyRTQ9w/PFyGY4u/PFmApmM6APzBeGn/+w9rPKFZSMSiVqdaBcyELvd9YVbTujXPsTl0rldPaxcuHlGeXqamFPFURbuowdffFjusyLlavL2NfAM4z/Y40X+ZG1xcV+PK9coUphrDWe+yolKBdeHsS6E/Zv9NMr1thBpWD2TFF2MyuAaYzzxVcAnfpRfLpXfp49DmeRNK9Ya9y51uiysg+5opX9pvB9lbWUD4l7pzqBcqUg8Iwc5E1Jbx+4c4XbB3Jmf/sgQ79+9NUel9XePrgbAmP3znV4Q/J1ZIfxKGeoXK8+PU4yHt3iehXpZYztTfNSq7snRXvTPBEZKFY3Mqgt2sioHz+NjN89M3ff7p5QN+ruQY1ztHs0Izu7Jy/HINBYD23wwOCBwQODBwYPiAf+AhI0HijdY85HAAAAAElFTkSuQmCC\")}</style>";
const char HTTP_SCRIPT[] PROGMEM = "<script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}</script>";
const char HTTP_HEAD_END[] PROGMEM = "</head><body><div style='text-align:left;display:inline-block;min-width:260px;'>";
const char HTTP_PORTAL_OPTIONS[] PROGMEM = "<form action=\"/wifi\" method=\"get\"><button>Configure WiFi</button></form><br/><form action=\"/0wifi\" method=\"get\"><button>Configure WiFi (Manually)</button></form><br/>";
//<form action=\"/i\" method=\"get\"><button>Info</button></form><br/><form action=\"/r\" method=\"post\"><button>Reset</button></form>";
const char HTTP_ITEM[] PROGMEM = "<div><a href='#p' onclick='c(this)'>{v}</a>&nbsp;<span class='q {i}'>{r}%</span></div>";
const char HTTP_FORM_START[] PROGMEM = "<form method='get' action='wifisave'><input id='s' name='s' length=32 placeholder='SSID'><br/><input id='p' name='p' length=64 type='password' placeholder='password'><br/>";
const char HTTP_FORM_PARAM[] PROGMEM = "<br/><input id='{i}' name='{n}' length={l} placeholder='{p}' value='{v}' {c}>";
const char HTTP_FORM_END[] PROGMEM = "<br/><button type='submit'>save</button></form>";
const char HTTP_SCAN_LINK[] PROGMEM = "<br/><div class=\"c\"><a href=\"/wifi\">Scan</a></div>";
const char HTTP_SAVED[] PROGMEM = "<div>Credentials Saved<br />Trying to connect to network.<br />If it fails reconnect to AP to try again</div>";
const char HTTP_END[] PROGMEM = "</div></body></html>";

#define WIFI_MANAGER_MAX_PARAMS 10

class WiFiManagerParameter
{
public:
  WiFiManagerParameter(const char *custom);
  WiFiManagerParameter(const char *id, const char *placeholder, const char *defaultValue, int length);
  WiFiManagerParameter(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom);

  const char *getID();
  const char *getValue();
  const char *getPlaceholder();
  int getValueLength();
  const char *getCustomHTML();

private:
  const char *_id;
  const char *_placeholder;
  char *_value;
  int _length;
  const char *_customHTML;

  void init(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom);

  friend class WiFiManager;
};

class WiFiManager
{
public:
  WiFiManager();

  boolean autoConnect();
  boolean autoConnect(char const *apName, char const *apPassword = NULL);

  //if you want to always start the config portal, without trying to connect first
  boolean startConfigPortal();
  boolean startConfigPortal(char const *apName, char const *apPassword = NULL);

  // get the AP name of the config portal, so it can be used in the callback
  String getConfigPortalSSID();
  String getSSID();
  String getPassword();
  void resetSettings();

  //sets timeout before webserver loop ends and exits even if there has been no setup.
  //useful for devices that failed to connect at some point and got stuck in a webserver loop
  //in seconds setConfigPortalTimeout is a new name for setTimeout
  void setConfigPortalTimeout(unsigned long seconds);
  void setTimeout(unsigned long seconds);

  //sets timeout for which to attempt connecting, useful if you get a lot of failed connects
  void setConnectTimeout(unsigned long seconds);

  void setDebugOutput(boolean debug);
  //defaults to not showing anything under 8% signal quality if called
  void setMinimumSignalQuality(int quality = 8);
  //sets a custom ip /gateway /subnet configuration
  void setAPStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn);
  //sets config for a static IP
  void setSTAStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn);
  //called when AP mode and config portal is started
  void setAPCallback(void (*func)(WiFiManager *));
  //called when settings have been changed and connection was successful
  void setSaveConfigCallback(void (*func)(void));
  //adds a custom parameter
  void addParameter(WiFiManagerParameter *p);
  //if this is set, it will exit after config, even if connection is unsuccessful.
  void setBreakAfterConfig(boolean shouldBreak);
  //if this is set, try WPS setup when starting (this will delay config portal for up to 2 mins)
  //TODO
  //if this is set, customise style
  void setCustomHeadElement(const char *element);
  //if this is true, remove duplicated Access Points - defaut true
  void setRemoveDuplicateAPs(boolean removeDuplicates);
  LED_Display_Wrapper LEDdisplay;

private:
  std::unique_ptr<DNSServer> dnsServer;
#ifdef ESP8266
  std::unique_ptr<ESP8266WebServer> server;
#else
  std::unique_ptr<WebServer> server;
#endif

  //const int     WM_DONE                 = 0;
  //const int     WM_WAIT                 = 10;

  //const String  HTTP_HEAD = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"/><title>{v}</title>";

  void setupConfigPortal();
  void startWPS();

  const char *_apName = "no-net";
  const char *_apPassword = NULL;
  String _ssid = "";
  String _pass = "";
  unsigned long _configPortalTimeout = 0;
  unsigned long _connectTimeout = 0;
  unsigned long _configPortalStart = 0;

  IPAddress _ap_static_ip;
  IPAddress _ap_static_gw;
  IPAddress _ap_static_sn;
  IPAddress _sta_static_ip;
  IPAddress _sta_static_gw;
  IPAddress _sta_static_sn;

  int _paramsCount = 0;
  int _minimumQuality = -1;
  boolean _removeDuplicateAPs = true;
  boolean _shouldBreakAfterConfig = false;
  boolean _tryWPS = false;

  const char *_customHeadElement = "";

  //String        getEEPROMString(int start, int len);
  //void          setEEPROMString(int start, int len, String string);

  int status = WL_IDLE_STATUS;
  int connectWifi(String ssid, String pass);
  uint8_t waitForConnectResult();

  void handleRoot();
  void handleWifi(boolean scan);
  void handleWifiSave();
  void handleInfo();
  void handleReset();
  void handleNotFound();
  void handle204();
  boolean captivePortal();
  boolean configPortalHasTimeout();

  // DNS server
  const byte DNS_PORT = 53;

  //helpers
  int getRSSIasQuality(int RSSI);
  boolean isIp(String str);
  String toStringIp(IPAddress ip);

  boolean connect;
  boolean _debug = true;

  void (*_apcallback)(WiFiManager *) = NULL;
  void (*_savecallback)(void) = NULL;

  WiFiManagerParameter *_params[WIFI_MANAGER_MAX_PARAMS];

  template <typename Generic>
  void DEBUG_WM(Generic text);

  template <class T>
  auto optionalIPFromString(T *obj, const char *s) -> decltype(obj->fromString(s))
  {
    return obj->fromString(s);
  }
  auto optionalIPFromString(...) -> bool
  {
    DEBUG_WM("NO fromString METHOD ON IPAddress, you need ESP8266 core 2.1.0 or newer for Custom IP configuration to work.");
    return false;
  }
};

#endif
