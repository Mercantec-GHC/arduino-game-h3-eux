#include <gameControls.h>
#include <displayFunc.h>
#include <Arduino.h>
#include <wifiHq.h>


String game_devId;

void game_init(String deviceId) {
    game_devId = deviceId;
}

QueueResponse game_joinQueue() {
    QueueResponse qr;

    Serial.print("Joining Game...");

    JSONVar doc;
    doc["deviceId"] = game_devId;
    String json = JSON.stringify(doc);

    String postResponse;
    if(wifi_httpPost("/joinqueue", json, postResponse)) {
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

            if(state == "inQueue") {
                qr.state = IN_QUEUE;
            }
            else {
                //missing other states womp womp :'(
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

    Serial.print("SENDING LOVE <3");

    JSONVar doc;
    doc["deviceId"] = game_devId;
    String json = JSON.stringify(doc);

    String postResponse;
    if(wifi_httpPost("/heartbeat", json, postResponse)) {
        JSONVar res = JSON.parse(postResponse);

        if ((bool)res["success"]) {
            hr.success = true;

            String state = (const char*) res["state"];

            if(state == "inQueue") {
                hr.state = IN_QUEUE;
            }
            else {
                //missing other states womp womp :'(
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