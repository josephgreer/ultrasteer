#pragma once

#ifdef EXPORT_PORTA
    #define portaL __declspec(dllexport)
#elif defined IMPORT_PORTA
    #define portaL __declspec(dllimport)
#else
    #define portaL
#endif

#include "porta_def.h"

#ifdef __cplusplus
extern "C" {
#endif

portaL int portaInit(int cineSize, const char* firmwarePath, const char* settingsPath, const char* licensePath, const char* lutPath,
    int usm, int pci, int hv, int ddr,
    int channels);
portaL void portaShutdown();
portaL int portaIsConnected();
portaL int portaGetSystemId(char* sysid, unsigned int sz);

portaL int portaSignalBnc();
portaL int portaPollBnc();
portaL int portaAckBnc();

portaL int portaSelectProbe(int id);
portaL int portaGetCurrentProbeID();
portaL int portaGetProbeID(int connector);
portaL int portaActivateProbeConnector(int connector);
portaL int portaGetProbeName(char* name, int sz, int id);
portaL int portaGetProbeInfo(probeInfo& nfo);

portaL int portaTestElectronicComponent(int id);
portaL void portaSetPowerTestParams(int tmout, int numQuickLevels, int* levels);
portaL int portaReadPowerValues(double* pv, double* nv, double* pav, double* nav, int& tmout, double& tol);

portaL int portaRunImage();
portaL int portaStopImage();
portaL int portaIsImaging();

portaL int portaSetWatchdog(int timeout);
portaL int portaSetCustomDMA(int enable, int physicaladdress, int size);
portaL void portaSetSleepDelay(int delay);

portaL int portaInitMode(imagingMode mode);
portaL imagingMode portaGetCurrentMode();

portaL void portaSetDisplayCallback(int index, PORTA_IMG_CALLBACK fn, void* prm);
portaL void portaSetRawDataCallback(PORTA_DATA_CALLBACK fn, void* prm);
portaL void portaSetPreRunCallback(PORTA_PRERUN_CALLBACK fn, void* prm);

portaL int portaGetFrameCount(unsigned int index);
portaL int portaGetFrameSize(int index);
portaL const unsigned char* portaGetFrameAddress(int index, int frame);
portaL unsigned char* portaGetCineStart(unsigned int index);
portaL void portaClearCine();

portaL int portaLoadMasterPreset();
portaL int portaLoadPreset(const char* path);
portaL int portaSavePreset(const char* path, int overwrite);
portaL int portaFindMasterPreset(char* path, int sz, int probe);
portaL int portaIsMasterPreset(const char* path);
portaL int portaGetPresetProbeID(const char* path, int& probeid1, int& probeid2, int& probeid3);

portaL int portaGetFrameRate();
portaL int portaGetDataFrameRate();

portaL int portaGetParam(const char* prm);
portaL int portaGetParamI(const char* prm, int& val);
portaL int portaSetParamI(const char* prm, int val);
portaL int portaGetParamS(const char* prm, char* val, int sz);
portaL int portaSetParamS(const char* prm, const char* val);
portaL int portaGetParamP(const char* prm, portaPoint& val);
portaL int portaSetParamP(const char* prm, portaPoint val);
portaL int portaGetParamR(const char* prm, portaRect& val);
portaL int portaSetParamR(const char* prm, portaRect val);
portaL int portaGetParamC(const char* prm, portaCurve& val);
portaL int portaSetParamC(const char* prm, portaCurve val);
portaL int portaGetParamGC(const char* prm, portaGainCurve& val);
portaL int portaSetParamGC(const char* prm, portaGainCurve val);
portaL int portaGetParamA(const char* prm, int* val, int* count);
portaL int portaSetParamA(const char* prm, const int* val, int count);

portaL int portaCycleParam(const char* prm, int fwd, int wrap = 0);
portaL int portaGetParamMinMax(const char* prm, int& min, int& max);
portaL int portaGetParamType(const char* prm, portaVarType& type);
portaL int portaGetListParam(char* prm, unsigned int sz, int prmNum);
portaL int portaGetNumParams();

portaL int portaSetDisplayDimensions(int index, int x, int y);
portaL int portaGetDisplayDimensions(int index, int& x, int& y);

portaL int portaGetPrescanBDimensions(int index, int& w, int& h);
portaL int portaGetBwImage(int index, unsigned char*& img, int useChroma);
portaL int portaGetBwImagePrescan(int index, unsigned char*& img);
portaL int portaGetColorImage(int index, unsigned char*& img);
portaL int portaGetColorImageOnly(int index, unsigned char*& img);
portaL int portaGetColorData(int index, unsigned char*& img, int velocity, int prescan, int copy);
portaL int portaGetColorVV(int index, unsigned char*& img);
portaL int portaGetDisplayFrameCount(int index);
portaL int portaProcessCineImage(int index, int frame);

portaL int portaImportColorMap(int index, const unsigned int* lut);

portaL int portaGetMicronsPerPixel(int index, int& mx, int& my);
portaL int portaGetPixelCoordinates(int index, int line, int sample, int& xOut, int& yOut, int bColor, int addAngle = 0);
portaL int portaGetUltrasoundCoordinates(int index, int x, int y, int& xOut, int& yOut, int bColor, int addAngle = 0);

portaL int portaGetHorizontalArcRect(int index, portaRect& arcRect, int color, int top, int sampleOverride = 0);
portaL int portaGetROI(int index, int* xOut, int* yOut);
portaL int portaGetColorBox(int index, int* xOut, int* yOut);
portaL int portaGetNewColorBox(int index, int x, int y, portaRect& colorBox, int center);
portaL int portaGetLinePosition(int index);

portaL double portaGoToPosition(double angle);
portaL double portaStepMotor(int cw, int steps = 8);
portaL void portaSetMotorHomeParams(int framesOffCenter);
portaL void portaSetMotorActive(int run);

#ifdef __cplusplus
}
#endif
