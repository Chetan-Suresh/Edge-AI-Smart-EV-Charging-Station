#include <time.h>
#include "edge_ai.h"
#include "State.h"
#include "Peripherals.h"
#include "model.h" // predictArrival(), predictDuration() see ai_training/

// Same commute-hour pattern used to generate the training data
// (ai_training/generate_dataset.py). In a later phase this can be
// replaced with a table learned from real historical telemetry.

float historicalArrivalRate(int hour) {
    if (hour >= 8 && hour <= 10)   return 0.62f;
    if (hour >= 18 && hour <= 21)  return 0.58f;
    if (hour >= 0 && hour <= 5)    return 0.06f;
    if (hour >= 11 && hour <= 17)  return 0.30f;
    
    return 0.18f;
}

void runEdgeAIInference() {
    struct tm timeinfo;
    int currentHour = 12; 
    int dayOfWeek = 0;

    if (getLocalTime(&timeinfo, 100)) {
        currentHour = timeinfo.tm_hour;
        dayOfWeek = timeinfo.tm_wday;
    }
    
    lastHourOfDay = currentHour; 

   
    float bayOccupiedF = (bayStatus == "CHARGING") ? 1.0f : 0.0f;
    float sessionElapsedMin = (bayStatus == "CHARGING") ? (millis() - sessionStartMs) / 60000.0f : 0.0f;
    float histRate = historicalArrivalRate(currentHour);
    predictedArrivalProb = predictArrival(currentHour, dayOfWeek, bayOccupiedF, recentAvgCurrent(), sessionElapsedMin, histRate);
    predictedDurationMin = (bayStatus == "CHARGING")
        ? (int)predictDuration(currentHour, dayOfWeek, bayOccupiedF, recentAvgCurrent(), sessionElapsedMin, histRate) 
        : 0;
}

