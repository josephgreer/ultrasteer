#pragma once
#include "SegmentCore.h"
#include "RPFileReader.h"
#include "ulterius.h"
#include <QMutex>
#include <deque>

namespace Nf {
#define MAX_FRAMES 5

  struct RPDatumHeader {
    u32 type;       // RPData type i.e. RPF_BPOST32
    u32 tick;       // tick in ms datum was received
    u32 nBytes;     // number of bytes

    RPDatumHeader() 
    {
      nBytes = 0;
      tick = 0;
      type = 0;
    }
  };

  struct RPDatum {
    RPDatumHeader header;
    u8 *data;
  };

  typedef std::deque < RPDatum > RPDataQueue;

	class RPUlteriusReader  {

	public:
		RPDatum GetNextRPDatum();

		u8 AddDatum(void* data, int type, int sz, bool cine, int frmnum);

    RPUlteriusReader(const char *ip, RP_TYPE type);
		virtual ~RPUlteriusReader();

    uDataDesc GetDesc(RP_TYPE type);

	protected:
    RP_TYPE m_type;
    uDataDesc m_desc;
		ulterius* m_ulterius;
		RPDataQueue m_queue;
	  QMutex m_lock;
    std::map<RP_TYPE, uDataDesc> m_descs;
	};

};