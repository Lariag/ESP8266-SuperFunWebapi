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
           "<link rel='icon' type='image/png' sizes='32x32' href='/img/Favicon.png' />"
           "<link rel='stylesheet' href='common.css' type='text/css' />"
           "</head>"
           "<body class='bodyClass' style='background-color:#222;'>"
           "<div class='pageHeader'>"
           "<h1 class='rainbow-gradient-text'>Welcome to Lara's Super Fun WebAPI!</h1>"
           "<div class='mainmenu'>"
#if DEV
           " | <a>&#60;DEV&#62;</a> "
#endif
           " | <a href='/'>Main page</a> | <a href='/config'>Config page</a> | <a href='https://github.com/Lariag/ESP8266-SuperFunWebapi' target='_blank'>View on GitHub</a> |</div></div>"
           "<div class='pageContainer'>");
}

String GetHTMLFooter() {
  return F("<br/><img src='/img/WemosBoard.gif' style='border-radius: 0 20px; width:300px' />"
           "</div></body>");
}

String GetNiceHTMLTemplace(String content) {
  return GetHTMLHeader() + content + GetArduinoInfoAsHTML() + GetHTMLFooter();
}


void pageRootHandler() {
  sendHtmlResponse(GetNiceHTMLTemplace(F(
    "<br/>Available APIs:<br/><ul>"

    "<li>Food Recipes:<ul>"
    "  <li><a target='_blank' href='/api/foodrecipes/Food.json'>/api/foodrecipes/Food.json</a></li>"
    "  <li><a target='_blank' href='/api/foodrecipes/Recipes.json'>/api/foodrecipes/Recipes.json</a></li>"
    "  <li><a target='_blank' href='/api/foodrecipes/CookingPrice.json'>/api/foodrecipes/CookingPrice.json</a></li>"
    "</ul></li>"

    "<li>Real State:<ul>"
    "  <li><a target='_blank' href='/api/realstate/Houses.json'>/api/realstate/Houses.json</a></li>"
    "  <li><a target='_blank' href='/api/realstate/Additions.json'>/api/realstate/Additions.json</a></li>"
    "  <li><a target='_blank' href='/api/realstate/Neighborhoods.json'>/api/realstate/Neighborhoods.json</a></li>"
    "</ul></li>"

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

const PROGMEM char pageCSS[] = {
           ".bodyClass {background-color:#222; margin:0; background-size: 3em; background-image: url('/img/bgTriangles.png'); background-repeat: repeat; color:#eee; font-family:'Arial'; font-size:1em}\n"
           ".pageHeader { padding: 8px; margin: -8px 0; background-color: #272727 }"
           ".pageContainer {margin: 8px;}"
           "a {color:#fca503}\n"
           "a:visited {color:#fca503}\n"
           "a:hover {color:#fcba03;}\n"
           ".mainmenu {font-size:.8em}\n"
           "ul {margin-bottom: 10px}\n"
           "ul li ul li {font-size:.9em}\n"
           "ul li ul li ul li {font-size:.8em}\n"
           ".rainbow-element-background { animation: rainbowBackground 5s linear infinite;}"
           "@keyframes rainbowBackground {0% { background: #ff000057; /* Red */ } 12.5% { background: #ff660057; /* Orange */ } 25% { background: #ffff0057; /* Yellow */ } 37.5% { background: #00ff0057; /* Green */ } 	50% { background: #00ffff57; /* Cyan */ } 62.5% { background: #0000ff57; /* Blue */ } 75% { background: #8a2be257; /* Indigo */ } 87.5% { background: #9400d357; /* Violet */ } 100% { background: #ff000057; /* Back to Red */ } }"
           ".rainbow-gradient-text:hover { background: repeating-linear-gradient(45deg, #ff0000, #ff6600, #ffff00, #00ff00, #00ffff, #0000ff, #8a2be2, #9400d3, #ff0000, #ff6600, #ffff00, #00ff00); background-size: 300px 100%; background-origin: border-box; background-clip: content-box; animation: rainbowGradient 10s linear infinite; color: transparent; background-clip: text; }"
           "@keyframes rainbowGradient { 0% { background-position: 0% 0%; } 100% { background-position: 100% 100%; } }"
           };

void handleCSS(){
  sendCSSResponse_P(pageCSS);
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
}

void handleImage_WemosBoard() {
  sendResourceRequest(true, gif, ImageWemosBoardBinary, ImageWemosBoardBinarySize);
}
void handleImage_BackgroundTiled() {
  sendResourceRequest(true, png, ImageTiledBackgroundBinary, ImageTiledBackgroundSize);
}