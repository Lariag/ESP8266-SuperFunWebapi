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
#if DEV
           "<title>&#60;DEV&#62; Lara's Super Fun WebAPI</title>"
#else
           "<title>Lara's Super Fun WebAPI</title>"
#endif
           "<style>"
           "a {color:#fca503} "
           "a:visited {color:#fca503} "
           "a:hover {color:#fcba03} "
           ".mainmenu {font-size:.8em}"
           "ul {margin-bottom: 10px}"
           "ul li ul li {font-size:.9em}"
           "ul li ul li ul li {font-size:.8em}"
           "</style>"
           "<link rel='icon' type='image/png' sizes='32x32' href='/img/Favicon.png' />"
           "</head>"
           "<body style='background-color:#222; color:#eee; font-family:Arial; font-size:1em'>"
           "<h1>Welcome to Lara's Super Fun WebAPI!</h1>"
           "<div class='mainmenu'>"
#if DEV
           " | <a>&#60;DEV&#62;</a> "
#endif
           " | <a href='/'>Main page</a> | <a href='/config'>Config page</a> | </div>");
}

String GetHTMLFooter() {
  return F("<br/><img src='/img/WemosBoard.gif' style='border-radius: 0 20px; width:300px' />"
           "</body>");
}

String GetNiceHTMLTemplace(String content) {
  return GetHTMLHeader() + content + GetArduinoInfoAsHTML() + GetHTMLFooter();
}


void pageRootHandler() {
  sendHtmlResponse(GetNiceHTMLTemplace(F(
    "<br/>Available APIs:<br/><ul>"

    "<li>HitList:<ul>"
    "  <li><a target='_blank' href='/api/hitlist/Contractors.json'>/api/hitlist/Contractors.json</a></li>"
    "</ul></li>"

    "<li>Journey Log:<ul>"
    "  <li>Pokémon Regions:<ul>"
    "    <li><a target='_blank' href='/api/journeylog/pokemonregions/Locations.json'>/api/journeylog/pokemonregions/Locations.json</a></li>"
    "    <li><a target='_blank' href='/api/journeylog/pokemonregions/Stays.json'>/api/journeylog/pokemonregions/Stays.json</a></li>"
    "    <li><a target='_blank' href='/api/journeylog/pokemonregions/Areas.json'>/api/journeylog/pokemonregions/Areas.json</a></li>"
    "  </ul></li>"
    "  <li>Spain Cities:<ul>"
    "    <li><a target='_blank' href='/api/journeylog/spainregions/Locations.json'>/api/journeylog/spainregions/Locations.json</a></li>"
    "    <li><a target='_blank' href='/api/journeylog/spainregions/Stays.json'>/api/journeylog/spainregions/Stays.json</a></li>"
    "    <li><a target='_blank' href='/api/journeylog/spainregions/Areas.json'>/api/journeylog/spainregions/Areas.json</a></li>"
    "  </ul></li>"
    "</ul></li>"

    "<li>Bank Accounts:<ul>"
    "  <li><a target='_blank' href='/api/bankaccounts/BankAccounts.json'>/api/bankaccounts/BankAccounts.json</a></li>"
    "  <li><a target='_blank' href='/api/bankaccounts/BankAccountMovements.json'>/api/bankaccounts/BankAccountMovements.json</a></li>"
    "</ul></li>"

    "<li>Currencies:<ul>"
    "  <li><a target='_blank' href='/api/currencies/rates.json'>/api/currencies/rates.json</a></li>"
    "  <li><a target='_blank' href='/api/currencies/transactions.json'>/api/currencies/transactions.json</a></li>"
    "</ul></li>"

    "<li>Pharses:<ul>"
    "  <li>Mariano Rajoy<ul>"
    "    <li><a target='_blank' href='/api/pharses/mariano'>/api/pharses/Mariano</a></li>"
    "    <li><a target='_blank' href='/api/pharses/mariano/random'>/api/pharses/Mariano/Random</a></li>"
    "  </ul></li>"
    "</ul></li>"

    "<li>Hanoi Tower Solver:<ul>"
    "  <li><a target='_blank' href='/api/hanoi?discs=3'>/api/hanoi</a><ul>"
    "    <li>Resolves the tower of hanoi problem, returning a json with the moves.</li>"
    "    <li>Parameter 'discs': Number of discs to solve. Between 2 and 5.</li>"
    "  </ul></li>"
    "</ul></li>"

    "</ul>")));
}


void pageConfigHandler() {
  uint8_t ledStatus = digitalRead(led);

  sendHtmlResponse(GetNiceHTMLTemplace(
    F("<br/>Available configuration options:<br/><ul>"
      "<li><a href='/api/config/boarddata'>Get current board data</a></li>"
      "<li><a href='/api/config/switchled'>Switch led on/off</a></li>"
      "<li><a href='/api/config/ledstatus'>Get led status</a></li>"
      "<!--li><a href='/config/restart'>Restart Arduino</a></li-->"
      "</ul>")));
}


void handleImage_Favicon() {
  sendResourceRequest(true, ico, ImageFaviconBinary, ImageFaviconBinarySize);
  //requests++;
  //server.sendHeader("Cache-Control", "public, max-age=3600");
  //server.send_P(200, F("image/png"), ImageFaviconBinary, ImageFaviconBinarySize);
}

void handleImage_WemosBoard() {
  sendResourceRequest(true, gif, ImageWemosBoardBinary, ImageWemosBoardBinarySize);
  //requests++;
  //server.sendHeader("Cache-Control", "public, max-age=3600");
  //server.send_P(200, "image/gif", ImageWemosBoardBinary, ImageWemosBoardBinarySize);
}