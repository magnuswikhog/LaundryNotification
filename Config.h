#ifndef LAUNDRYNOTIFICATION_CONFIG_H
#define LAUNDRYNOTIFICATION_CONFIG_H


struct Config{
  uint64_t NOTIFICATION_TIMEOUT;      // How many milliseconds to wait for Blynk to be connected after laundry is finished, before timing out and stop trying to send the notification
  long VIBRATION_DEVIATION_THRESHOLD; // Only when the vibration deviation (from average) exceeds this value, it's counted as "vibrating"
  int DELAY;                          // Delay time between sensor readings
  long MIN_VIBRATION_TIME;            // Minimum time between first and last vibration in order to consider laundry finished (i.e. shortest possible laundry cycle)
  long MIN_VIBRATION_COUNT;           // Minimum number of times counted as "vibrating" in order to consider laundry finished. Multiplied by DELAY it means that the machine needs to vibrate at least that many milliseconds.
  long MAX_VIBRATION_WAIT;            // Maximum time to wait after vibration has ended until laundry is considered finished
  long RESET_TIMEOUT;                 // If laundry hasn't been considered finished for this many ms since first vibration, reset and start waiting for a new laundry cycle
  float AVERAGE_ALPHA;                // Lowpass smoothing for average vibration value   
  float DEVIATION_ALPHA;              // Lowpass smoothing for vibration deviation from average
};


#endif
