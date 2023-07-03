void configSwitchLedStatusHandler() {
  uint8_t ledStatus = digitalRead(led);
  if (ledStatus == HIGH) {
    digitalWrite(led, LOW);
  } else {
    digitalWrite(led, HIGH);
  }

  server.sendHeader("Location", String("/config"), true);
  server.send(302, "text/plain", "");
}

void configGetLedStatusHandler() {
  uint8_t ledStatus = digitalRead(led);
  sendJsonResponse(String("{\"LedStatus\" : " + String(ledStatus == HIGH ? 0 : 1) + "}"));
}

void (*resetFunc)(void) = 0;
void configRestartBoard() {
  server.sendHeader("Location", String("/"), true);
  server.send(302, "text/plain", "");
  delay(1000);
  ESP.restart();
}

void WebapiConfig_BoardData() {
  uint32_t currentSeconds = now();
  uint8_t ledStatus = digitalRead(led);
  int wifiSignal = WiFi.RSSI();
  int freeMemory = availableMemory();

  char serialized[1000];
  StaticJsonDocument<200> doc;
  doc["Board"] = F("Wemos D1 Mini");
  doc["SecondsSinceStartup"] = currentSeconds;
  doc["RequestsSinceStartup"] = requests;
  doc["InvalidRequestsSinceStartup"] = requestsInvalid;
  doc["LedStatus"] = ledStatus == HIGH ? 0 : 1;
  doc["WiFiSignal"] = wifiSignal;
  doc["FreeMemory"] = freeMemory;
  serializeJson(doc, serialized);
  sendJsonResponse(serialized);
}

void configMainHandler() {
  uint8_t ledStatus = digitalRead(led);

  sendHtmlResponse(GetNiceHTMLTemplace(
    F("<br/>Available configuration options:<br/><ul>"
      "<li><a href='/api/config/boarddata'>Get current board data</a></li>"
      "<li><a href='/api/config/switchled'>Switch led on/off</a></li>"
      "<li><a href='/api/config/ledstatus'>Get led status</a></li>"
      "<!--li><a href='/config/restart'>Restart Arduino</a></li-->"
      "</ul>")));
}