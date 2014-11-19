#pragma once
#include "SegmentCore.h"
#include "RPFileReader.h"
#include "UlteriusInterface.h"
#include "ulterius.h"
#include <QMutex>


//This class manages and receives data from several child processes.
//Each child process (UlteriusDataGrabber) streams a particular data type
//using the ulterius interface.  This data is then sent using named pipes (shared memory)
//by each child process to this class. Normally I would just use threads to do this, 
//but there is a bug in the ulterius DLL which forces us to use a different process 
//for each data type we get from ulterius.

namespace Nf {

#define MAX_FRAMES 5
  //#define MANDATE_B8 

  typedef std::deque < RPData > DatumQueue;
  typedef std::deque < GPS_Data > GpsQueue;

  //Handles (see UlteriusDataGrabber.cpp for a description of what each one does).
  typedef struct {
    HANDLE hRead;
    HANDLE hWrite;
    HANDLE hDone;
    HANDLE hPipe;
  } HandleSet;

  class RPUlteriusProcessManager;

  typedef struct {
    std::deque < HandleSet > queue;
    QMutex *lock;
    u8 done;
    RPUlteriusProcessManager *owner;
    std::map < RP_TYPE, uDataDesc > descs;
  } ThreadContext;

  class RPUlteriusProcessManager : RPReaderCollection {

  public:
    RPData GetNextRPData();

    uDataDesc GetImageDesc(RP_TYPE type);

    s32 AddData(const RPDatum *data);
    void SetFrameRate(f32 framerate);

    RPUlteriusProcessManager(const char *ip, f64 mpp, Vec2d origin, f32 framerate=11.0f);
    virtual ~RPUlteriusProcessManager();

    void EnableType(RP_TYPE type, u8 enable);
  protected:
    char m_ip[20];
    std::map < RP_TYPE, HandleSet > m_handles;
    std::map < RP_TYPE, uDataDesc > m_descs;
    DatumQueue m_queue;
    GpsQueue m_qGps;
    QMutex *m_lock;
    ThreadContext m_tContext;
    u32 m_enabledTypes;
    f32 m_msPerFrame;
    ulterius m_ulterius;
    s32 m_dataAvailable;
    Vec2d m_origin;
    f64 m_mpp;
  };
};