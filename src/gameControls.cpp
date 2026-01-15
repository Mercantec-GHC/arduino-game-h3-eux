#include <gameControls.h>
#include <displayFunc.h>
#include <Arduino.h>
#include <wifiHq.h>


String game_devId;

void game_init(String deviceId) {
    game_devId = deviceId;
}

String CreateBody() {
    JSONVar doc;
    doc["deviceId"] = game_devId;
    String json = JSON.stringify(doc);

    return json;
}

QueueResponse game_joinQueue() {
    QueueResponse qr;

    qr.success = false;

    Serial.println("Joining Game...");

    String json = CreateBody();

    String postResponse;
    if(wifi_httpPost("/api/joinqueue", CreateBody(), postResponse)) {
        JSONVar res = JSON.parse(postResponse);

        if ((bool)res["success"]) {
            qr.success = true;
            
            String dir = (const char*) res["direction"];

            if (dir == "left") {
                qr.direction = MOVE_LEFT;
            }
            else {
                qr.direction = MOVE_RIGHT;
            }

            String state = (const char*) res["state"];

            if(state == "waiting" || state == "ready") {
                qr.state = IN_QUEUE;
            }
        }
        else {
            qr.state = DISCONNECTED;
        }
    }
    else {
        qr.state = DISCONNECTED;
    }

    return qr;
}


 HeartbeatResponse game_sendHeartbeat() {
    HeartbeatResponse hr;

    Serial.println("SENDING LOVE <3");

    String json = CreateBody();

    String postResponse;
    if(wifi_httpPost("/api/heartbeat", CreateBody(), postResponse)) {
        JSONVar res = JSON.parse(postResponse);

        if ((bool)res["success"]) {
            hr.success = true;

            String state = (const char*) res["state"];

            if(state == "waiting" || state == "ready") {
                hr.state = IN_QUEUE;
            }
            else if (state == "playing"){
                hr.state = IN_GAME;
            }
            else {
                hr.state = GAME_OVER;
            }

            hr.score = (int)res["score"];

            hr.multiplier = (double)res["multiplier"];
        }
        else {
            hr.state = DISCONNECTED;
        }
    }
    else {
        hr.state = DISCONNECTED;
    }

    return hr;
}

bool game_move(bool move) {

    Serial.println("YOU GOT THE MOVES LIKE JAGGER");

    JSONVar doc;
    doc["deviceId"] = game_devId;
    doc["IsMoving"] = move;

    String json = JSON.stringify(doc);

    String postResponse;
    if(wifi_httpPost("/api/move", json, postResponse)) {
        JSONVar res = JSON.parse(postResponse);

        if((bool)res["success"]) {
            return true;
        }
        else {
            return false;
        }
    }
    
    return false;
}