// Solve function from: https://www.geeksforgeeks.org/c-program-for-tower-of-hanoi-2/
void solveHanoiTower_addMove(char from_rod, char to_rod, JsonArray* movesDoc) {
  JsonObject moveObj = movesDoc->createNestedObject();
  moveObj["From"] = String(from_rod);
  moveObj["To"] = String(to_rod);
}

int solveHanoiTower(int n, char from_rod, char to_rod, char aux_rod, JsonArray* movesDoc) {
  if (n == 1) {
    solveHanoiTower_addMove(from_rod, to_rod, movesDoc);
    // Serial.println("Move disk 1 from rod " + String(from_rod) + " to rod " + String(to_rod));
    return 1;
  }

  int moves = 0;
  moves += solveHanoiTower(n - 1, from_rod, aux_rod, to_rod, movesDoc);

  solveHanoiTower_addMove(from_rod, to_rod, movesDoc);
  // Serial.println("Move disk " + String(n) + " from rod " + String(from_rod) + " to rod " + String(to_rod));

  moves += solveHanoiTower(n - 1, aux_rod, to_rod, from_rod, movesDoc);
  return moves + 1;
}

void sendJsonResponseObject(String message) {
}

void WebapiHanoiHandle() {
  DynamicJsonDocument doc(4096);

  if (server.args() != 1 || server.argName(0) != "discs" || server.arg(0).toInt() < 2 || server.arg(0).toInt() > 5) {
    // Error 400.
    sendJsonResponseError(418, F("There must be only one argument received and it must be named 'discs', indicating the number of discs to move (min 2, max 5)."));
    return;
  }

  int discs = server.arg(0).toInt();

  //server.
  doc["Discs"] = server.arg(0);
  doc["TotalMoves"] = 0;
  JsonArray moves = doc.createNestedArray("Moves");
  doc["TotalMoves"] = solveHanoiTower(discs, 'A', 'C', 'B', &moves);

  serializeJson(doc, charBuffer);
  // serializeJson(doc, Serial);

  sendJsonResponse(charBuffer);
}