#ifndef STATE_H
#define STATE_H

#include <Arduino.h>
// ---------------------------------------------------------------------
// Live bay state
// ---------------------------------------------------------------------
extern String bayStatus;
extern float voltage, current, power, energyWh, temperature;

extern unsigned long sessionStartMs;
extern float predictedArrivalProb;
extern int predictedDurationMin;
extern int lastHourOfDay;

extern bool overloadActive;
extern int peakTariffStartHr;
extern int peakTariffEndHr;
extern float predictionThreshold;
extern String loadDecision;
extern int throttleLevel;
extern int overloadCurrentA;
extern int maxStationLoadW;
extern bool manualOverrideActive;


// ---------------------------------------------------------------------
// Timing / debounce bookkeeping
// ---------------------------------------------------------------------

#endif