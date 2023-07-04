#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>
#include <EasyDDNS.h>
#include <Time.h>
#include <TimeLib.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>

#include "Configs.h"
#include "ImagesBinary.h"
#include "jsonResponses.h"
#include "MiscFunctions.h"
#include "CharBuffer.h"

unsigned long requests = 0;
unsigned long requestsInvalid = 0;

ESP8266WebServer server(80);

// Main handlers
#include "ResponseHelpers.h"
#include "PageHandlers.h"
#include "WebApiHandlers.h"
#include "ConfigHandlers.h"

// Advanced handlers
#include "WebapiHanoi.h"

void setupHandlers() {
  // Page handlers
  server.on(F("/"), pageRootHandler);
  server.on(F("/config"), pageConfigHandler);

  // Resources:
  server.on(F("/img/Favicon.png"), handleImage_Favicon);
  server.on(F("/img/WemosBoard.gif"), handleImage_WemosBoard);

  // Webapi handlers
  server.on(F("/api"), WebapiHandler);

  server.on(F("/api/hanoi"), WebapiHanoiHandle);

  server.on(F("/api/journeylog/pokemonregions/Locations.json"), WebapiPokeJourneyLog_Locations);
  server.on(F("/api/journeylog/pokemonregions/Stays.json"), WebapiPokeJourneyLog_Stays);
  server.on(F("/api/journeylog/pokemonregions/Areas.json"), WebapiPokeJourneyLog_Areas);

  server.on(F("/api/journeylog/spainregions/Locations.json"), WebapiEsJourneyLog_Locations);
  server.on(F("/api/journeylog/spainregions/Stays.json"), WebapiEsJourneyLog_Stays);
  server.on(F("/api/journeylog/spainregions/Areas.json"), WebapiEsJourneyLog_Areas);
  server.on(F("/api/esjourneylog/Locations.json"), WebapiEsJourneyLog_Locations);
  server.on(F("/api/esjourneylog/Stays.json"), WebapiEsJourneyLog_Stays);
  server.on(F("/api/esjourneylog/Areas.json"), WebapiEsJourneyLog_Areas);

  server.on(F("/api/bankaccounts/BankAccounts.json"), WebapiBankAccounts_BankAccounts);
  server.on(F("/api/bankaccounts/BankAccountMovements.json"), WebapiBankAccounts_BankAccountMovements);

  server.on(F("/api/currencies/rates.json"), WebapiCurrencies_Rates);
  server.on(F("/api/currencies/transactions.json"), WebapiCurrencies_Transactions);

  server.on(F("/api/hitlist/Contractors.json"), WebapiHitList_Contractors);

  server.on(F("/api/pharses/mariano"), WebapiPhrases_Mariano_All);
  server.on(F("/api/pharses/mariano/random"), WebapiPhrases_Mariano_RandomOne);

  server.onNotFound(handleNotFound);

  // Config handlers

  server.on(F("/api/config/switchled"), configSwitchLedStatusHandler);
  server.on(F("/api/config/restart"), configRestartBoard);
  server.on(F("/api/config/reset"), configRestartBoard);

  server.on(F("/api/config/ledstatus"), configGetLedStatusHandler);
  server.on(F("/api/config/boarddata"), WebapiConfig_BoardData);
}

void setup(void) {
  pinMode(led, OUTPUT);

  if (enableSerial)
    Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.config(staticIp, gateway, subnet);
  WiFi.begin(ssid, password);
  Serial.println();

  // Wait for connection
  bool ledStatus = false;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
    digitalWrite(led, ledStatus ? LOW : HIGH);
    ledStatus = !ledStatus;
  }

  digitalWrite(led, HIGH);

  Serial.println();
  Serial.print(F("Connected to "));
  Serial.println(ssid);
  Serial.print(F("IP address: "));
  Serial.println(WiFi.localIP());

  if (enableOTA)
    ArduinoOTA.begin();

  if (MDNS.begin(MDNSAddress)) {
    Serial.println(F("MDNS responder started"));
  }

  setupHandlers();

  server.begin();
  Serial.println(F("HTTP server started"));

  // EasyDDNS
  if (enableDDNS) {
    EasyDDNS.service(ddnsService);
    EasyDDNS.client(ddnsUrl, ddnsToken);
    EasyDDNS.onUpdate([&](const char* oldIP, const char* newIP) {
      Serial.print(F("EasyDDNS - IP Change Detected: "));
      Serial.println(newIP);
    });
  }
}

void loop(void) {
  server.handleClient();
  if (enableDDNS)
    EasyDDNS.update(ddnsUpdateIntervalMS);
  MDNS.update();
  if (enableOTA)
    ArduinoOTA.handle();
}
