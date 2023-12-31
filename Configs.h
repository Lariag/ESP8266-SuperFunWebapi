#ifndef STASSID
#define STASSID "Your WiFi SSID"
#define STAPSK "Your WiFi password"
#endif

#define DEV 1
// Set a single board (used for images).
#define IsWemosBoard 0
#define IsAZDeliveryBoard 1

const int led = LED_BUILTIN;

#if DEV
const bool enableSerial = true;
const bool enableDDNS = false;
const bool enableOTA = true;
const bool enableOTAPassword = false;
const char PROGMEM passwordOTA[] = {"1234"};
#else
const bool enableSerial = false;
const bool enableDDNS = true;
const bool enableOTA = true;
const bool enableOTAPassword = true;
const char PROGMEM passwordOTA[] = {"1234"};
#endif

const char PROGMEM ddnsToken[] = { "Your token" };
const char PROGMEM ddnsService[] = { "Your DDNS service" };
const char PROGMEM ddnsUrl[] = { "Your DDNS URL" };
const unsigned long ddnsUpdateIntervalMS = 1800000;

const char* ssid = STASSID;
const char* password = STAPSK;

#if DEV
const char PROGMEM MDNSAddress[] = { "arduinowebapi-dev" };
const IPAddress staticIp(192, 168, 1, 199);
#else
const char PROGMEM MDNSAddress[] = { "arduinowebapi" };
const IPAddress staticIp(192, 168, 1, 200);
#endif

const IPAddress gateway(192, 168, 1, 1);
const IPAddress subnet(255, 255, 255, 0);