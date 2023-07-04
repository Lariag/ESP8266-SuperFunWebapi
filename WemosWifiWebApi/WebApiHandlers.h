#include "sys/string.h"

// Handlers

void WebapiBankAccounts_BankAccounts() {
  sendJsonResponse_P(json_BankAccounts_BankAccounts);
}

void WebapiBankAccounts_BankAccountMovements() {
  sendJsonResponse_P(json_BankAccounts_BankAccountMovements);
}

void WebapiPokeJourneyLog_Locations() {
  sendJsonResponse_P(json_JourneyLog_Poke_Locations);
}

void WebapiPokeJourneyLog_Stays() {
  sendJsonResponse_P(json_JourneyLog_Poke_Stays);
}

void WebapiPokeJourneyLog_Areas() {
  sendJsonResponse_P(json_JourneyLog_Poke_Areas);
}

void WebapiEsJourneyLog_Locations() {
  sendJsonResponse_P(json_JourneyLog_Es_Locations);
}

void WebapiEsJourneyLog_Stays() {
  sendJsonResponse_P(json_JourneyLog_Es_Stays);
}

void WebapiEsJourneyLog_Areas() {
  sendJsonResponse_P(json_JourneyLog_Es_Areas);
}

void WebapiCurrencies_Transactions() {
  sendJsonResponse_P(json_Currencies_Transactions);
}

void WebapiCurrencies_Rates() {
  sendJsonResponse_P(json_Currencies_Rates);
}

void WebapiHitList_Contractors() {
  sendJsonResponse_P(json_HitList_Contractors);
}

void WebapiPhrases_Mariano_All(){
  sendJsonResponse_P(json_Phrases_Mariano);
}

void WebapiPhrases_Mariano_RandomOne(){
  sendJsonResponse_P(json_Phrases_Mariano_Separated[random(json_Phrases_MarianoSeparated_Count)]);
}

void WebapiHandler() {
  server.sendHeader("Location", String("/"), true);
  server.send(302, "text/plain", "");
}


void handleNotFoundPlainText() {
  requestsInvalid++;

  String message = "Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void handleNotFound() {
  if (server.uri().indexOf("/api/") != -1) {
    handleNotFoundPlainText();
  } else {
    requestsInvalid++;

    String message =
      "<h2>Not Found</h2><ul>"
      "<li>URI: "
      + server.uri() + "</li>"
                       "<li>Method: "
      + (server.method() == HTTP_GET ? "GET" : "POST") + "</li>"
                                                         "<li>Arguments: "
      + String(server.args()) + "<ol>";

    for (uint8_t i = 0; i < server.args(); i++) {
      message += "<li>" + server.argName(i) + ": " + server.arg(i) + "</li>";
    }

    if (server.hasHeader("User-Agent")) {
      message += "<li>User-Agent: " + server.header("User-Agent") + "</li>";
    }

    message += "</ol></li></ul>";

    server.send(404, "text/html", GetNiceHTMLTemplace(message));
  }
}
