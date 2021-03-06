#pragma once
#include "RTCore.h"
#include "RPFileReader.h"
#include "ulterius.h"
#include <QMutex>

namespace Nf {
#define MAX_FRAMES 5
#define MAX_GPS 20
//#define MANDATE_B8 

	typedef std::map < u32, RPData > FrameCollectionQueue;

	class RPUlteriusReaderCollection : RPReaderCollection {

	public:
		virtual RPData GetNextRPData();

		uDataDesc GetImageDesc(RP_TYPE type);
		virtual u8 AddData(void* data, int type, int sz, bool cine, int frmnum, bool doLock = true);

		RPUlteriusReaderCollection(const char *ip, Vec2d mpp, Vec2d origin);
		virtual ~RPUlteriusReaderCollection();

    RPData AssembleRPData();
    void DeleteFramesBefore(s32 frame);
    void PurgeExcessFrames();
		void EnableType(RP_TYPE type, u8 enable);
		void EnableMask(u32 mask);
		bool m_imageInfoUpdated;

		void UpdateImageDescs();

    virtual void SetFrameInformation(Vec2d mpp, Vec2d origin);

	protected:
    RPData m_gps;
    RPData m_gps2;
		std::map < RP_TYPE, u8 > m_types;
		std::map < RP_TYPE, uDataDesc > m_descs;
		ulterius* m_ulterius;
		FrameCollectionQueue m_frameQueue;
		QMutex m_lock;
		Vec2d m_mpp;
    Vec2d m_origin;
    u32 m_mask;
	};
  
  class RPCallbackReceiver
  {
  public:
    virtual void Callback(const RPData *rp) = 0;
  };

  class RPUlteriusReaderCollectionPush : public RPUlteriusReaderCollection 
  {
  protected:
    RPCallbackReceiver *m_cb;

public:
		RPUlteriusReaderCollectionPush(const char *ip, Vec2d mpp, Vec2d origin);
    virtual ~RPUlteriusReaderCollectionPush();
		virtual u8 AddData(void* data, int type, int sz, bool cine, int frmnum, bool doLock = true);
		virtual RPData GetNextRPData();

    void SetRPCallbackReceiver(RPCallbackReceiver *cb);
  };
};