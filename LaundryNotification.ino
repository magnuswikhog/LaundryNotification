#include <Wire.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "LaundryCycleDetector.h"
#include "Config.h"
#include "UserSettings.h"




//#define BLYNK_PRINT Serial                // Comment this out to disable prints and save space




void setConfig(bool debug, Config& config){
  //                                              DEBUG           PRODUCTION
  // ---------------------------------------------------------------------------------------------
  config.NOTIFICATION_TIMEOUT           = debug ? 100000        : 2073600000;     // 48 hours
  config.VIBRATION_DEVIATION_THRESHOLD  = debug ? 300           : 300;            // 
  config.DELAY                          = debug ? 100           : 100;            // 100 ms
  config.MIN_VIBRATION_TIME             = debug ? 3000          : 1200000;        // 20 minutes
  config.MIN_VIBRATION_COUNT            = debug ? 10            : 100;            // 
  config.MAX_VIBRATION_WAIT             = debug ? 3000          : 300000;         // 5 minutes
  config.RESET_TIMEOUT                  = debug ? 14400000      : 18000000;       // 5 hours
  config.AVERAGE_ALPHA                  = debug ? 0.8           : 0.8;            //
  config.DEVIATION_ALPHA                = debug ? 0.8           : 0.8;            //
}




#define WIFI_STATUS_LED 4
#define LAUNDRY_STATUS_LED 5
#define DEBUG_INPUT_PIN 12




Config config;
const uint8_t MPU=0x68; 
const size_t readBits = 12;
int16_t accX,accY,accZ,tmp,gyX,gyY,gyZ;
LaundryCycleDetector* detector;
long laundryFinishedTimestamp;
BlynkTimer blynkTimer;






void setup(){
  // Init acc module
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B); 
  Wire.write(0);    
  Wire.endTransmission(true);

  pinMode(LAUNDRY_STATUS_LED, OUTPUT);
  pinMode(WIFI_STATUS_LED, OUTPUT);
  pinMode(DEBUG_INPUT_PIN, INPUT_PULLUP);
  digitalWrite(LAUNDRY_STATUS_LED, HIGH); // HIGH = Builtin LED off
  digitalWrite(WIFI_STATUS_LED, LOW); // LOW = GPIO LED off

  bool useDebugConfig = digitalRead(DEBUG_INPUT_PIN) == LOW;
  setConfig( useDebugConfig, config );
  if( useDebugConfig ){
    for(int i=0; i<10; i++){
      digitalWrite(WIFI_STATUS_LED, HIGH);
      digitalWrite(LAUNDRY_STATUS_LED, LOW);
      delay(100);
      digitalWrite(WIFI_STATUS_LED, LOW);
      digitalWrite(LAUNDRY_STATUS_LED, HIGH);
      delay(100);
    }
  }
  
  detector = new LaundryCycleDetector( 
      config.VIBRATION_DEVIATION_THRESHOLD,
      config.MIN_VIBRATION_TIME,
      config.MIN_VIBRATION_COUNT,
      config.MAX_VIBRATION_WAIT,
      config.RESET_TIMEOUT,
      config.AVERAGE_ALPHA,
      config.DEVIATION_ALPHA
  );

  

  Blynk.begin(BLYNK_AUTH, WIFI_SSID, WIFI_PASSWORD);
  blynkTimer.setInterval(1000L, blynkTimerEvent);  
}


void loop(){
  Blynk.run();
  blynkTimer.run();
  

  if( laundryFinishedTimestamp == 0 ){
    // Read from acc module
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);  
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, readBits, true);    
    accX=Wire.read()<<8|Wire.read();    
    accY=Wire.read()<<8|Wire.read();  
    accZ=Wire.read()<<8|Wire.read();  
    gyX=Wire.read()<<8|Wire.read();  
    gyY=Wire.read()<<8|Wire.read();  
    gyZ=Wire.read()<<8|Wire.read();  
    
    detector->addVibrationValue( accX );
  
    if( detector->isCycleStarted() ){
      digitalWrite(LAUNDRY_STATUS_LED, detector->isVibrating() ? LOW : HIGH );
    }

    if( detector->isLaundryFinished() ){
      digitalWrite(LAUNDRY_STATUS_LED, LOW); // Turn on LED
      laundryFinishedTimestamp = millis();
    }  
  }

  
  delay(config.DELAY);
}



void notifyLaundryFinished(){
  digitalWrite(LAUNDRY_STATUS_LED, HIGH); // Turn off LED
  Blynk.notify("Excuse me, your laundry is finished!");  
}



void blynkTimerEvent(){
    digitalWrite( WIFI_STATUS_LED, Blynk.connected() ? HIGH : LOW );

    /* If laundry is finished, send notification if connected. If Blynk connection hasn't been available for the set 
       timeout, reset the detector and don't try to send the notification any longer. */
    if( laundryFinishedTimestamp > 0 ){
      if( Blynk.connected() || millis() - laundryFinishedTimestamp > config.NOTIFICATION_TIMEOUT ){
        detector->resetDetector();
        laundryFinishedTimestamp = 0;
        notifyLaundryFinished();
      }
    }
}
