#ifndef CFORCESENSOR_H
#define CFORCESENSOR_H

#include "cATIForceSensor.h"
#include <string>

class cForceSensor
{
public:
    cForceSensor(void);
    ~cForceSensor(void);
	int Initialize_Force_Sensor(std::string a_Device);
    int Zero_Force_Sensor(void);
    int Stop_Force_Sensor(void);

    void Set_Calibration_File_Loc(std::string a_Location);

    int AcquireFTData();
    void GetForceReading(double *a_readBuffer);
    void GetTorqueReading(double *a_readBuffer);
    std::string GetForceUnit(void);
    std::string GetTorqueUnit(void);
    
protected:

private:
    double m_Frequency;
    int m_AveragingSize;
    cATIForceSensor* FTSensor;
    double m_FTData[6];
    std::string m_CalibrationFileLocation;
    
};

#endif // CFORCESENSOR_H
