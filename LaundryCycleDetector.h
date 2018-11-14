#ifndef LaundryCycleDetector_h
#define LaundryCycleDetector_h

#include "Arduino.h"





class LaundryCycleDetector {
  public:
    LaundryCycleDetector(long vibrationDeviationThreshold, long minVibrationTime, long minVibrationCount, long maxFinishedWaitTime, long resetTimeout, float averageAlpha, float deviationAlpha) : 
        mVibrationDeviationThreshold(vibrationDeviationThreshold), 
        mMinVibrationTime(minVibrationTime), 
        mMinVibrationCount(minVibrationCount), 
        mMaxFinishedWaitTime(maxFinishedWaitTime), 
        mResetTimeout(resetTimeout),
        mAverageLowpassAlpha(averageAlpha),
        mDeviationLowpassAlpha(deviationAlpha),
        mInitialized(false)
        { 
          resetDetector();  
        };

    void addVibrationValue(long value);
    bool shouldResetDetector();
    void resetDetector();
    bool isCycleStarted();
    bool isVibrating();
    bool isLaundryFinished();
        

  private:
    const long mVibrationDeviationThreshold;
    const long mMinVibrationTime;
    const long mMinVibrationCount;
    const long mMaxFinishedWaitTime;
    const long mResetTimeout;
    
    long mFirstVibrationMillis;
    long mLatestVibrationMillis;
    long mVibrationCount;
    long mVibrationAverage; 
    long mVibrationDeviation; 
    float mAverageLowpassAlpha;    
    float mDeviationLowpassAlpha;
    bool mInitialized;
  
    void lowpassFilter(long input, long* output, float alpha);
    long abs(long val);    
  
};




#endif
