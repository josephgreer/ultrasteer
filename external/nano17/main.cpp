#include "cForceSensor.h"
#include <Windows.h>

class Timer {
  public:
    Timer() {
      reset();
    }
    /// reset() makes the timer start over counting from 0.0 seconds.
    void reset() {
      unsigned __int64 pf;
      QueryPerformanceFrequency( (LARGE_INTEGER *)&pf );
      freq_ = 1.0 / (double)pf;
      QueryPerformanceCounter( (LARGE_INTEGER *)&baseTime_ );
    }
    /// seconds() returns the number of seconds (to very high resolution)
    /// elapsed since the timer was last created or reset().
    double seconds() {
      unsigned __int64 val;
      QueryPerformanceCounter( (LARGE_INTEGER *)&val );
      return (val - baseTime_) * freq_;
    }
    /// seconds() returns the number of milliseconds (to very high resolution)
    /// elapsed since the timer was last created or reset().
    double milliseconds() {
      return seconds() * 1000.0;
    }
  private:
    double freq_;
    unsigned __int64 baseTime_;
};

Timer t;
double StartTime = 0;
double CurrentTime = 0;

int main()
{
	double ForceData[3];

	cForceSensor g_ForceSensor;
	g_ForceSensor.Set_Calibration_File_Loc("C:/Joey/ultrasteer/src/Nano17/FT11287.cal"); 
	int retVal = g_ForceSensor.Initialize_Force_Sensor("Dev1/ai0:5");

	for (;;)
	{
		StartTime = t.seconds();
		int i = g_ForceSensor.AcquireFTData();
		g_ForceSensor.GetForceReading(ForceData);
		CurrentTime = t.seconds();
    Sleep(100);
		//printf("Time: %f\r", CurrentTime-StartTime);
		printf("Force 1: %f\t%f\t%f\tTime = %f\n", ForceData[0], ForceData[1], ForceData[2], CurrentTime-StartTime);
	}

	return 0;
}