#include <ArduinoJson.h>
#include "rpc.h"
#include "network.h"
#include "State.h"
#include "config.h"

// ---------------------------------------------------------------------
// SRS 8.5: RPC command handling
// ---------------------------------------------------------------------
void handleRpc(String requestId, char* payload) {

    JsonDocument doc;

    if (deserializeJson(doc, payload)) {
        Serial.println("RPC JSON parsing failed.");
        return;
    }

    String method = doc["method"] | "";

    JsonDocument response;

    // ---------------------------------------------------------------
    // Relay ON / OFF
    // ThingsBoard sends:
    // setRelayState(true)
    // setRelayState(false)
    // ---------------------------------------------------------------
    if (method == "setRelayState") {

        bool relayState = doc["params"] | false;

        if (relayState) {
            digitalWrite(RELAY_PIN, HIGH);
        }
        else {
            digitalWrite(RELAY_PIN, LOW);
        }

        manualOverrideActive = true;
        throttleLevel = relayState ? 100 : 0;
        loadDecision = relayState ? "ALLOW" : "MANUAL_OFF";

        response["success"] = true;
        response["relayState"] = relayState;
        response["throttleLevel"] = throttleLevel;

        Serial.print(">> RPC setRelayState(");
        Serial.print(relayState);
        Serial.println(") - manual override engaged.");
    }

    // ---------------------------------------------------------------
    // Get current relay state
    // ThingsBoard widget sends:
    // getState
    // ---------------------------------------------------------------
    else if (method == "getState") {

        bool relayState = digitalRead(RELAY_PIN);

        response["relayState"] = relayState;
        response["success"] = true;

        Serial.print(">> RPC getState -> ");
        Serial.println(relayState ? "ON" : "OFF");
    }

    // ---------------------------------------------------------------
    // Set throttle
    // ---------------------------------------------------------------
    else if (method == "setThrottle") {

        int level = doc["params"]["level"] | 100;

        level = constrain(level, 0, 100);

        manualOverrideActive = true;
        throttleLevel = level;
        loadDecision = "MANUAL_THROTTLE";

        response["success"] = true;
        response["throttleLevel"] = throttleLevel;

        Serial.print(">> RPC setThrottle(");
        Serial.print(level);
        Serial.println(") - manual override engaged.");
    }

    // ---------------------------------------------------------------
    // Hand control back to the optimizer
    // ---------------------------------------------------------------
    else if (method == "setAutoMode") {

        manualOverrideActive = false;
        throttleLevel = 100;
        loadDecision = "ALLOW";

        response["success"] = true;
        response["manualOverrideActive"] = false;

        Serial.println(">> RPC setAutoMode - back under automatic control.");
    }

    // ---------------------------------------------------------------
    // Unknown RPC
    // ---------------------------------------------------------------
    else {

        response["success"] = false;
        response["error"] = "unknown method";

        Serial.print(">> Unknown RPC method: ");
        Serial.println(method);
    }

    // ---------------------------------------------------------------
    // Send response back to ThingsBoard
    // ---------------------------------------------------------------
    char buffer[256];

    serializeJson(response, buffer);

    mqtt.publish(
        ("v1/devices/me/rpc/response/" + requestId).c_str(),
        buffer
    );
}