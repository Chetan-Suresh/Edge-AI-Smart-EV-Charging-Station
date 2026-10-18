#include <Arduino.h>
#include <WiFi.h>
#include "State.h"
#include "config.h"
#include "Peripherals.h"
#include "network.h"
#include "telemetry.h"
#include "model.h"
#include "edge_ai.h"
#include "optimization.h"



void setup()
{

    Serial.begin(115200);
    dht.begin();  // initialise sesnor
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
    pinMode(BTN_PLUGIN, INPUT_PULLUP);
    pinMode(BTN_PLUGOUT, INPUT_PULLUP);
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(LED_GREEN , OUTPUT);
    pinMode(LED_YELLOW, OUTPUT);
    pinMode(LED_RED, OUTPUT);

    connectWiFi(); 
    mqtt.setServer(MQTT_SERVER, MQTT_PORT);
    // PubSubClient defaults to a 256-byte buffer; a CHARGING telemetry
    // packet is ~262 bytes, so publish() would silently drop it.
    mqtt.setBufferSize(512);
    mqtt.setCallback(mqttCallback);
    connectMQTT(); 
   
}

unsigned long now;
unsigned long last_print;

void loop()
{

    ensureConnectivity();   // reconnect Wi-Fi / MQTT if the link dropped
    mqtt.loop();

    // sense -> predict -> decide -> report, every 5 s
    now = millis();
    if((now - last_print) > 5000)
    {
        last_print = now;
        sample_sensors();
        runEdgeAIInference();
        runOptimization();
        publishTelemetry();
    }
    plug_status();
    applyRelayDutyCycle();
    updateLeds();
}

