#ifndef CDAQHARDWAREINTERFACE_H
#define CDAQHARDWAREINTERFACE_H

#define CHRACTER_SIZE 2048
#define MIN_SAMPLES_PER_CHANNEL 2 // the minimum number of samples per channel that daqmx will allow us to specify
#define MAX_DATA_SIZE 10000
#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else
#define CONTINUOUS_SAMPLING

#include "NIDAQmx.h"
#include <string>

//CALLBACKS
long CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32n everyNsamplesEventType, uint32n nSamples, void *callbackData);
long CVICALLBACK DoneCallback(TaskHandle taskHandle, int32n status, void *callbackData);

class cDaqHardwareInterface
{
public:
    cDaqHardwareInterface();
    ~cDaqHardwareInterface();


    int32n ConfigSingleSampleTask(double a_SampleRate, int a_AveragingSize, std::string a_DeviceName, int a_FirstChannel,
                                 int a_NumChannels, int a_MinVoltage, int a_MaxVoltage);

    int32n StopCollection(void);
    int32n ReadSingleSample(double buffer[]);
    std::string GetErrorInfo();
    int GetMinVoltage();
    int GetMaxVoltage();
    unsigned int GetFirstChannel();
    std::string GetDeviceName();
    unsigned int GetNumChannels();
    double GetSampleFrequency();
    unsigned int GetAveragingSamples();
    std::string GetErrorCodeDescription(long errCode);
    int32n ConfigBufferTask( double a_SampleRate, int a_AveragingSize, std::string a_DeviceName, int a_FirstChannel,
                            int a_NumChannels, int a_MinVoltage, int a_MaxVoltage, int a_BufferSize );

    int32n ReadBufferedSamples( int a_NumSamples, double a_Buffer[]);
    void SetConnectionMode( int DAQConnMode );
    int GetConnectionMode();
	

private:
    TaskHandle*        	m_thDAQTask;                // the task which is used to get a data1
    std::string         m_sDeviceName;              // the name of the NI-DAQmx device which the transducer is attached to
    unsigned int		m_uiAveragingSize;          // the number of samples to average together to smooth data
    double              m_f64SamplingFrequency;     // the frequency at which to sample data, applies to both buffered and
                                                    // single-point acquisitions

    unsigned long		m_ulBufferedSize;           // the buffer size to use with buffered continuous acquisition
    unsigned int		m_uiNumChannels;            // the number of channels to sample, should always be either 6 (no software
                                                    // temp comp) or 7 (with software temp comp)

    unsigned int		m_uiFirstChannel;           // the first channel occupied by the transducer
    int					m_iMinVoltage;              // the minimum voltage
    int					m_iMaxVoltage;              // the maximum voltage
    std::string   		m_sErrorInfo;               // information about the last error
    int					m_iConnectionMode;          // connection mode of the DAQ device

	double data[MAX_DATA_SIZE];
};

#endif // CDAQHARDWAREINTERFACE_H
