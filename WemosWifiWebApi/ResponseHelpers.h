// Helper functions

String formatJson(String json) {
  json.replace(F("'"), F("\""));
  return json;
}

void sendJsonResponse(String json) {
  requests++;
  server.send(200, F("application/json"), json);
}

void sendJsonResponseError(int errorCode, String errorMessage) {
  requests++;
  server.send(errorCode, F("application/json"), String(F("{\"Error\" : \"")) + errorMessage + String(F("\"}")));
}

void sendJsonResponse_P(const char* json) {
  requests++;
  server.send_P(200, "application/json", json);
}

void sendCSSResponse_P(const char* css) {
  requests++;
  server.send_P(200, "text/css", css);
}
void sendJSResponse_P(const char* css) {
  requests++;
  server.send_P(200, "text/javascript", css);
}

void sendHtmlResponse(String html) {
  requests++;
  server.send(200, F("text/html"), html);
}

void sendHtmlResponse_P(const char* html) {
  requests++;
  server.send(200, "text/html", html);
}

enum resourceType {
  png = 1,
  gif = 2,
  json = 3,
  ico = 4
};

void sendResourceRequest(bool useCache, enum resourceType resType, const char* data, unsigned long dataSize) {
  requests++;
  if (useCache)
    server.sendHeader(F("Cache-Control"), F("public, max-age=86400"));

  switch (resType) {
    case png:
      server.send_P(200, "image/png", data, dataSize);
      break;
    case gif:
      server.send_P(200, "image/gif", data, dataSize);
      break;
    case ico:
      server.send_P(200, "image/x-icon", data, dataSize);
      break;
    case json:
      server.send_P(200, "application/json", data, dataSize);
      break;
  }
}
