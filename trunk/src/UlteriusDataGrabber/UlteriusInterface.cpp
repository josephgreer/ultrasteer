#include "SegmentCore.h"
#include "UlteriusInterface.h"

namespace Nf {

  static RPUlteriusReader *gThis = NULL;

  static bool onNewData(void* data, int type, int sz, bool cine, int frmnum)
  {
    u8 rv;
    if(gThis)
      rv = gThis->AddDatum(data, type, sz, cine, frmnum);
    else
      rv = false;
    return (bool)rv;
  }

  RPUlteriusReader::RPUlteriusReader(const char *ip, RP_TYPE type)
    :  m_type(type) 
  {
    m_ulterius = new ulterius();

    m_ulterius->connect(ip);
    m_ulterius->setSharedMemoryStatus(0);
    m_ulterius->setCallback(onNewData);

    bool rv = m_ulterius->setDataToAcquire(type);
    ASSERT(rv);

    gThis = this;
  }

  RPUlteriusReader::~RPUlteriusReader()
  {
    gThis = NULL;
  }

  RPDatum RPUlteriusReader::GetNextRPDatum()
  {
    if(!m_lock.tryLock(30))
      return RPDatum();

    RPDatum res;
    if(m_queue.size()) {
      res = m_queue.front();
      m_queue.pop_front();
    }

    m_lock.unlock();
    return res;
  }

  u8 RPUlteriusReader::AddDatum(void* data, int type, int sz, bool cine, int frmnum)
  {
    u32 tick = GetTickCount();
    //printf("Type %d Tick %d\n", type, tick);

    RPDatum datum;
    datum.header.type = type;
    datum.header.tick = tick;
    datum.data = (u8 *)malloc(sz);
    datum.header.nBytes = sz;

    memcpy(datum.data, data, datum.header.nBytes);

    if(!m_lock.tryLock(30)) {
      free(datum.data);
      return 0;
    }

    m_queue.push_back(datum);

    while(m_queue.size() > MAX_FRAMES) {
      RPDatum temp = m_queue.front();
      free(temp.data);
      m_queue.pop_front();
    }

    m_lock.unlock();
    return 1;
  }

};