#ifndef STASSID
#define STASSID "Your WiF iSSID"
#define STAPSK "Your WiFi password"
#endif

const int led = LED_BUILTIN;
const bool enableSerial = true;
const bool enableDDNS = false;
const bool enableOTA = true;

const char PROGMEM ddnsToken[] = { "Your token" };
const char PROGMEM ddnsService[] = { "Your DDNS service" };
const char PROGMEM ddnsUrl[] = { "Your DDNS URL" };
const unsigned long ddnsUpdateIntervalMS = 1800000;

const char* ssid = STASSID;
const char* password = STAPSK;

const char PROGMEM MDNSAddress[] = { "arduinowebapi" };
const IPAddress staticIp(192, 168, 1, 185);
const IPAddress gateway(192, 168, 1, 1);
const IPAddress subnet(255, 255, 255, 0);