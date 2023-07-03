String GetArduinoInfoAsHTML() {
  uint32_t currentSeconds = now();
  int hrs = currentSeconds / 3600;
  int minutes = currentSeconds % 3600 / 60;
  uint8_t ledStatus = digitalRead(led);

  String info = "<br/><div style='font-size:.7em'>Hosted on a <a target='_blank' style='color:#eeeeee' href='https://www.luisllamas.es/wemos-d1-mini-una-genial-placa-de-desarrollo-con-esp8266/'>Wemos D1 Mini</a> Arduino board.</br>"
                " - Time since startup: "
                + String(hrs) + "h " + String(minutes) + "m " + String(currentSeconds % 60) + "s | Requests since startup: " + String(requests) + " (+" + String(requestsInvalid) + " invalid) | Led: " + (ledStatus == LOW ? String("🔵") : String("🔴")) + " | Free Memory: " + String(availableMemory()) + " bytes | WiFi Signal: " + String(WiFi.RSSI()) + "dBm -</div>";
  return info;
}

String GetHTMLHeader() {
  return F("<!DOCTYPE html>"
           "<html>"
           "<head>"
           "<meta charset='UTF-8'>"
           "<title>Lara's Super Fun WebApi</title>"
           "<style>a {color:#fca503} a:visited {color:#fca503} a:hover {color:#fcba03} .mainmenu {font-size:.8em}</style>"
           "<link rel='icon' type='image/png' sizes='32x32' href='/img/Favicon.png' />"
           "</head>"
           "<body style='background-color:#222; color:#eee; font-family:Arial; font-size:1em'>"
           "<h1>Welcome to Lara's Super Fun WebApi!</h1>"
           "<div class='mainmenu'> | <a href='/'>Main page</a> | <a href='/config'>Config page</a> | </div>");
}

String GetHTMLFooter() {
  return F("<br/><img src='/img/WemosBoard.gif' style='border-radius: 0 20px; width:300px' />"
           "</body>");
}

String GetNiceHTMLTemplace(String content) {
  return GetHTMLHeader() + content + GetArduinoInfoAsHTML() + GetHTMLFooter();
}