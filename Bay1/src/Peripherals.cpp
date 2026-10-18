#include <Arduino.h>
#include <DHT.h>
#include "State.h"
#include "Peripherals.h"
#include "config.h"


DHT   dht(DHT_PIN , DHT_TYPE);

unsigned long lastSampleMs = 0;

// Rolling mean of the last 5 samples - this is the recentAvgCurrent
// feature both Edge AI models are trained on.
float currentHistory[5] = {0, 0, 0, 0, 0};
int   currentHistoryIdx = 0;

float mapFloat(long x, long inMin, long inMax, float outMin, float outMax) {
    return (x - inMin) * (outMax - outMin) / (float)(inMax - inMin) + outMin;
}


void sample_sensors(void)
{
    int raw_current = analogRead(CURRENT_PIN); // 0 to 4095
    int raw_voltage = analogRead(VOLTAGE_PIN); // 0 to 4095

    voltage = mapFloat(raw_voltage, 0, 4095, 0 ,250);

    if(bayStatus == "CHARGING")
    {
        current = mapFloat(raw_current, 0, 4095, 0, 32); 
    }
    else
    {
        current = 0;
    }

    // read current and 5 values array
      
    power = voltage * current;

    // feed the rolling-average window
    currentHistory[currentHistoryIdx] = current;
    currentHistoryIdx = (currentHistoryIdx + 1) % 5;




    // Read temperature in CELSIUS. readTemperature(bool S, bool force):
    // the first argument is "return Fahrenheit", NOT a pin number.
    float t = dht.readTemperature();
    if(!(isnan(t))) temperature = t;

    // Energy delivered this session (Wh), integrated over real elapsed time
    unsigned long nowMs = millis();
    if(bayStatus == "CHARGING" && lastSampleMs != 0)
    {
        energyWh += power * ((nowMs - lastSampleMs) / 3600000.0f);
    }
    lastSampleMs = nowMs;

    
   


}

float recentAvgCurrent(void)
{
    float sum = 0;
    for(int i = 0; i < 5; i++)
    {
        sum += currentHistory[i];
    }
    return sum / 5.0f;
}

bool plugin_flag_once = 1;
bool plugout_flag_once = 1;

void plug_status(void)
{
    bool pluginReading = digitalRead(BTN_PLUGIN);
   // detect the sw is pressed
   if(pluginReading == LOW && plugin_flag_once)
   {
    sessionStartMs = millis();
    // plug in switch is pressed
    plugin_flag_once = 0;
   // change bay_status FREE to charging
   if(bayStatus == "FREE" )
   {
        bayStatus = "CHARGING";
        energyWh = 0.0;                 // new session, new energy total
        Serial.printf("%s plug in detected - now CHARGING\n", BAY_ID);
        digitalWrite(RELAY_PIN, HIGH);
   }
   //update leds

   }
   
   if(pluginReading == HIGH)
   {
    plugin_flag_once = 1;
   }
   

   // plug out switch is pressed
   bool plugoutReading = digitalRead(BTN_PLUGOUT);
   // detect the sw is pressed
   if(plugoutReading == LOW && plugout_flag_once)
   {
    // plug in switch is pressed
    plugout_flag_once = 0;
   // change bay_status FREE to charging
   if(bayStatus == "CHARGING" )
   {
        bayStatus = "FREE";
        manualOverrideActive = false;   // release any operator override
        throttleLevel = 100;
        loadDecision = "IDLE";
        Serial.printf("%s plug out detected - now FREE\n", BAY_ID);
   }
   //update leds

   }
   
   if(plugoutReading == HIGH)
   {
    plugout_flag_once = 1;
   }

   // change bay_status  charging to FREE
   //update leds

}

void update_led_status(void)
{
    // if baystatus is charging turn on green led
    if(bayStatus == "FREE")
    {
        digitalWrite(LED_GREEN, HIGH);
        digitalWrite(LED_YELLOW, LOW);

    }
    else
    {
        // if baystatus is free turn on yellow led
        digitalWrite(LED_GREEN, LOW);
        digitalWrite(LED_YELLOW, HIGH);

    }
    
}