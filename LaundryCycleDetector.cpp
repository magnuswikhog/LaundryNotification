#include "Arduino.h"
#include "LaundryCycleDetector.h"



void LaundryCycleDetector::addVibrationValue(long value){
  if( !mInitialized ){
      mVibrationAverage = value;
      mVibrationDeviation = 0;
      mInitialized = true;
      return;
  }
  
  
  lowpassFilter( value, &mVibrationAverage, mAverageLowpassAlpha );
  lowpassFilter( abs(value-mVibrationAverage), &mVibrationDeviation, mDeviationLowpassAlpha );


  if( mVibrationDeviation > mVibrationDeviationThreshold ){
    mVibrationCount++;
    
    if( mFirstVibrationMillis == 0 )
      mFirstVibrationMillis = millis();
      
    mLatestVibrationMillis = millis();
  }


  if( shouldResetDetector() )
    resetDetector();  
}






bool LaundryCycleDetector::shouldResetDetector(){
  return mFirstVibrationMillis > 0
      && (long) millis() - mFirstVibrationMillis > mResetTimeout;
}



void LaundryCycleDetector::resetDetector(){
  mVibrationCount = 0;
  mFirstVibrationMillis = 0;
  mLatestVibrationMillis = 0;
}



bool LaundryCycleDetector::isLaundryFinished(){
  return mFirstVibrationMillis > 0 
      && mLatestVibrationMillis > 0 
      && mVibrationCount > mMinVibrationCount
      && mLatestVibrationMillis - mFirstVibrationMillis > mMinVibrationTime
      && (long) millis() - mLatestVibrationMillis > mMaxFinishedWaitTime;
}


bool LaundryCycleDetector::isCycleStarted(){
  return mFirstVibrationMillis > 0 
      && mLatestVibrationMillis > 0 
      && mVibrationCount > 0;
}


bool LaundryCycleDetector::isVibrating(){
  return mVibrationDeviation > mVibrationDeviationThreshold;
}




void LaundryCycleDetector::lowpassFilter(long input, long* output, float alpha){
  *output = alpha*(*output) + (1-alpha)*input;
}




long LaundryCycleDetector::abs(long val){
  return val < 0 ? -val : val;
}
