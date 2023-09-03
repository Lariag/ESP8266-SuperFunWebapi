#ifndef NOTREALLYPKER_CPP
#define NOTREALLYPKER_CPP

namespace NotReallyPoker {

#include "NotReallyPokerResources.h"

#define PlayerLength 16
#define TableLength 16
#define NumPlayers 40
#define PlayerInactivityInSeconds 20  // In seconds
#define NumTables 10
#define TableNull NumTables + 1

#if DEV
WebSocketsServer webSocket = WebSocketsServer(8088);
#else
WebSocketsServer webSocket = WebSocketsServer(8087);
#endif

class Player {
private:

public:
  uint8_t playerId;
  uint8_t tableIndex;
  char playerName[PlayerLength];
  int choosenCard;
  bool active;

  Player() {
  }

  void SetPlayer(uint8_t id, uint8_t table, const char* name, bool isExpectator) {
    playerId = id;
    tableIndex = table;
    strcpy(playerName, name);
    choosenCard = isExpectator ? -1 : 0;
    active = true;
  }

  void CleanPlayer() {
    active = false;
    choosenCard = 0;
    playerId = 0;
  }
};

typedef void (*PlayerFunctionPointer)(Player&);
typedef void (*PlayerJsonArrayFunctionPointer)(Player&, JsonArray*);

class Game {
private:
  Player players[NumPlayers];
  char tableNames[NumTables][TableLength];

  uint8_t GetIndexForTableName(const char* tableName, bool registerIfNotExists) {
    for (int i = 0; i < NumTables; i++) {
      if (strcasecmp(tableNames[i], tableName) == 0) {
        return i;
      }
    }

    if (registerIfNotExists) {
      for (int i = 0; i < NumTables; i++) {
        if (strlen(tableNames[i]) == 0) {
          strcpy(tableNames[i], tableName);
          return i;
        }
      }
    }

    return TableNull;
  }

public:
  Game() {
  }

  ~Game() {
  }

  int CountPlayersInSameTable(uint8_t playerId) {
    int playerIndex = GetPlayerIndex(playerId);
    if (playerIndex < 0) {
      return 0;
    }

    int cont = 0;
    for (int i = 0; i < NumPlayers; i++) {
      if (players[i].active && players[i].tableIndex == players[playerIndex].tableIndex) {
        cont++;
      }
    }
    return cont;
  }

  int GetPlayerIndex(uint8_t playerId) {
    for (int i = 0; i < NumPlayers; i++) {
      if (players[i].active && players[i].playerId == playerId) {
        return i;
      }
    }
    return -1;
  }

  bool SetPlayer(uint8_t playerId, const char* tableName, const char* playerName, bool isExpectator) {
    int playerIndex = GetPlayerIndex(playerId);
    if (playerIndex >= 0) {
      return false;
    }

    uint8_t tableIndex = GetIndexForTableName(tableName, true);
    if (tableIndex == TableNull) {
      return false;
    }

    for (int i = 0; i < NumPlayers; i++) {
      if (!players[i].active) {
        players[i].SetPlayer(playerId, tableIndex, playerName, isExpectator);
        return true;
      }
    }
    return false;
  }

  void PlayerDisconnected(uint8_t playerId) {
    int i = GetPlayerIndex(playerId);
    if (i >= 0) {
      int playersInTable = CountPlayersInSameTable(playerId);
      if (playersInTable == 1) {
        tableNames[players[i].tableIndex][0] = '\0';
      }
      players[i].CleanPlayer();
    }
  }

  bool PlayerNameExistsInTable(const char* tableName, const char* playerName) {
    uint8_t tableIndex = GetIndexForTableName(tableName, false);
    if (tableIndex == TableNull) {
      return false;
    }

    for (int i = 0; i < NumPlayers; i++) {
      if (players[i].active && players[i].tableIndex == tableIndex && strcasecmp(players[i].playerName, playerName) == 0) {
        return true;
      }
    }
    return false;
  }


  bool ChooseCard(uint8_t playerId, int card) {
    int i = GetPlayerIndex(playerId);
    if (i >= 0) {
      players[i].choosenCard = card;
      return true;
    }
    return false;
  }

  bool ResetPlayerTableCards(uint8_t playerId) {
    int playerIndex = GetPlayerIndex(playerId);
    if (playerIndex < 0) {
      return false;
    }

    for (int i = 0; i < NumPlayers; i++) {
      if (players[i].active && players[i].tableIndex == players[playerIndex].tableIndex && players[i].choosenCard > 0) {
        players[i].choosenCard = 0;
      }
    }
    return true;
  }

  int GetChoosenCardForPlayer(uint8_t playerId) {
    int i = GetPlayerIndex(playerId);
    if (i >= 0) {
      return players[i].choosenCard;
    }
    return 0;
  }

  bool AreAllCardsChosen(char* tableName) {
    uint8_t tableIndex = GetIndexForTableName(tableName, false);
    if (tableIndex == TableNull) {
      return false;
    }

    bool allCardsChoosen = true;
    for (int i = 0; i < NumPlayers; i++) {
      if (players[i].tableIndex == tableIndex) {
        if (players[i].choosenCard == 0) {
          allCardsChoosen = false;
          break;
        }
      }
    }
    return allCardsChoosen;
  }

  bool ForEachPlayerInSameTable(uint8_t playerId, bool includePlayer, PlayerFunctionPointer f) {
    int playerIndex = GetPlayerIndex(playerId);
    if (playerIndex < 0) {
      return false;
    }

    for (int i = 0; i < NumPlayers; i++) {
      if (players[i].active && (players[i].playerId != playerId || includePlayer) && players[i].tableIndex == players[playerIndex].tableIndex) {
        f(players[i]);
      }
    }
  }

  bool ForEachPlayerInSameTable(uint8_t playerId, bool includePlayer, JsonArray* jArray, PlayerJsonArrayFunctionPointer f) {
    int playerIndex = GetPlayerIndex(playerId);
    if (playerIndex < 0) {
      return false;
    }

    for (int i = 0; i < NumPlayers; i++) {
      if (players[i].active && (players[i].playerId != playerId || includePlayer) && players[i].tableIndex == players[playerIndex].tableIndex) {
        f(players[i], jArray);
      }
    }
  }
};

Game PokerGame;

const unsigned int pokerCharBufferSize = 1000;
char pokerCharBuffer[pokerCharBufferSize] = { "\0" };

void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      {
        Serial.printf("[%u] Disconnected!\n", num);

        // If player is in a table, tell the other talbe players that this one disconnected.
        if (PokerGame.GetPlayerIndex(num) >= 0) {
          StaticJsonDocument<60> doc;
          doc["disconnectedId"] = num;
          serializeJson(doc, pokerCharBuffer);

          PokerGame.ForEachPlayerInSameTable(num, false, [&](Player& player) {
            webSocket.sendTXT(player.playerId, pokerCharBuffer);
          });

          PokerGame.PlayerDisconnected(num);
        }
      }
      break;

    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

        StaticJsonDocument<20> doc;
        doc["yourId"] = num;
        serializeJson(doc, pokerCharBuffer);
        webSocket.sendTXT(num, pokerCharBuffer);
      }
      break;
    case WStype_TEXT:
      {
        Serial.printf("Payload: [%u] get Text: %s\n", num, payload);
        StaticJsonDocument<256> doc;
        deserializeJson(doc, payload);

        if (doc.containsKey(F("action"))) {

          int action = doc["action"];

          int errorCode = 0;
          bool sendResponseAtTheEnd = true;

          switch (action) {
            case 1:  // Login.
              if (doc.containsKey(F("table")) && doc.containsKey(F("player")) && doc.containsKey(F("expectator"))) {

                const char* table = doc[F("table")];
                const char* player = doc[F("player")];

                if (strlen(table) < TableLength && strlen(player) < PlayerLength) {
                  const bool expectator = doc[F("expectator")];

                  if (PokerGame.PlayerNameExistsInTable(table, player)) {
                    errorCode = 1;  // Player name is already present in the table.
                  } else {
                    if (PokerGame.SetPlayer(num, table, player, expectator)) {
                      // Send the current table players to the newly logged-in player, and "new player connected" message to the other players.
                      {
                        StaticJsonDocument<150> docNewPlayer;
                        docNewPlayer["a"] = action;
                        docNewPlayer["id"] = num;
                        docNewPlayer["n"] = player;
                        if (expectator) {
                          docNewPlayer["c"] = -1;
                        }
                        serializeJson(docNewPlayer, pokerCharBuffer);
                      }

                      int playerCount = PokerGame.CountPlayersInSameTable(num);
                      int estimatedMem = JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(playerCount * 3) + 15 * playerCount + 50;
                      DynamicJsonDocument docResponse(estimatedMem);
                      docResponse["a"] = action;
                      JsonArray playersInTable = docResponse.createNestedArray(F("tp"));

                      PokerGame.ForEachPlayerInSameTable(num, true, &playersInTable, [&](Player& player, JsonArray* jArray) {
                        Serial.printf("   Adding player %s (%u) at tableIndex %u\n", player.playerName, player.playerId, player.tableIndex);

                        JsonObject playerObj = jArray->createNestedObject();
                        playerObj["id"] = player.playerId;
                        playerObj["n"] = player.playerName;
                        if (player.choosenCard != 0) {
                          playerObj["c"] = player.choosenCard;
                        }

                        webSocket.sendTXT(player.playerId, pokerCharBuffer);
                      });

                      serializeJson(docResponse, pokerCharBuffer);
                      Serial.printf("Response action 1 for player %i: %s, mem: %i (estimated %i)\n", num, pokerCharBuffer, docResponse.memoryUsage(), estimatedMem);
                      webSocket.sendTXT(num, pokerCharBuffer);
                      sendResponseAtTheEnd = false;
                    } else {
                      errorCode = 2;  // Player already in another table or all slots are full.
                    }
                  }
                } else {
                  errorCode = 105;  // Failed data validation.
                }
              } else {
                errorCode = 104;  // Failed json validation.
              }
              break;
            case 2:  // Choose card and send the confirmation response.
              if (doc.containsKey(F("cardNumber"))) {
                int cardNumber = doc[F("cardNumber")];
                int oldCardNumber = PokerGame.GetChoosenCardForPlayer(num);

                if (PokerGame.ChooseCard(num, cardNumber)) {
                  StaticJsonDocument<100> docResponse;
                  docResponse[F("a")] = action;
                  docResponse[F("id")] = num;
                  docResponse[F("c")] = cardNumber;
                  serializeJson(docResponse, pokerCharBuffer);

                  webSocket.sendTXT(num, pokerCharBuffer);
                  Serial.printf("Response action 2 for sender: %s\n", pokerCharBuffer);

                  if (oldCardNumber != cardNumber && (oldCardNumber == 0 || cardNumber == 0)) {
                    PokerGame.ForEachPlayerInSameTable(num, false, [&](Player& player) {
                      webSocket.sendTXT(player.playerId, pokerCharBuffer);
                      Serial.printf("Response action 2 for player %i: %s\n", player.playerId, pokerCharBuffer);
                    });
                  }

                  sendResponseAtTheEnd = false;
                } else {
                  errorCode = 30;  // Error choosing card for player.
                }
              } else {
                errorCode = 104;  // Failed json validation.
              }
              break;

            case 3:  // Reveal cards
              {
                int playerCount = PokerGame.CountPlayersInSameTable(num);
                int estimatedMem = JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(playerCount * 3) + 50;
                DynamicJsonDocument docResponse(estimatedMem);
                docResponse["a"] = action;
                JsonArray playersCardsInTable = docResponse.createNestedArray(F("tc"));

                PokerGame.ForEachPlayerInSameTable(num, true, &playersCardsInTable, [&](Player& player, JsonArray* jArray) {
                  if (player.choosenCard >= 0) {
                    JsonObject playerObj = jArray->createNestedObject();
                    playerObj["id"] = player.playerId;
                    playerObj["c"] = player.choosenCard;
                  }
                });

                serializeJson(docResponse, pokerCharBuffer);
                Serial.printf("Response action 3 for player %i: %s, mem: %i (estimated %i)\n", num, pokerCharBuffer, docResponse.memoryUsage(), estimatedMem);
                PokerGame.ForEachPlayerInSameTable(num, true, [&](Player& player) {
                  webSocket.sendTXT(player.playerId, pokerCharBuffer);
                });

                sendResponseAtTheEnd = false;
              }
              break;

            case 4:  // Restart game
              {
                if (PokerGame.ResetPlayerTableCards(num)) {

                  DynamicJsonDocument docResponse(50);
                  docResponse["a"] = action;
                  serializeJson(docResponse, pokerCharBuffer);
                  PokerGame.ForEachPlayerInSameTable(num, true, [&](Player& player) {
                    webSocket.sendTXT(player.playerId, pokerCharBuffer);
                  });

                  sendResponseAtTheEnd = false;
                } else {
                  errorCode = 31;  // Error resetting players cards.
                }
              }
              break;
          }

          if (sendResponseAtTheEnd || errorCode > 0) {
            StaticJsonDocument<20> docResponseAtEnd;
            if (errorCode > 0) {
              docResponseAtEnd[F("e")] = errorCode;
            } else {
              docResponseAtEnd[F("a")] = action;
            }
            serializeJson(docResponseAtEnd, pokerCharBuffer);
            webSocket.sendTXT(num, pokerCharBuffer);
          }
        }
      }
      break;
  }
}

void HandleNotReallyPoker_css() {
  sendCSSResponse_P(notreallypoker_css);
}
void HandleNotReallyPoker_js() {
  sendJSResponse_P(notreallypoker_js);
}
void HandleNotReallyPoker_Html() {
  sendHtmlResponse(notreallypoker_html);
}
void HandleNotreallypoker_Logo() {
  sendResourceRequest(true, png, notreallypoker_Logo_BinaryImage, notreallypoker_Logo_BinarySize);
}
void HandleNotreallypoker_Card() {
  sendResourceRequest(true, gif, notreallypoker_Card_BinaryImage, notreallypoker_Card_BinarySize);
}

void StartWebSocket() {
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.println("WebSocket server started.");
}

void ProcessWebSocket() {
  webSocket.loop();
}
}

#endif