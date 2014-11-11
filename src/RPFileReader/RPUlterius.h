#pragma once
#include "SegmentCore.h"
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
		RPData GetNextRPData();

		uDataDesc GetImageDesc(RP_TYPE type);
		u8 AddData(void* data, int type, int sz, bool cine, int frmnum);

		RPUlteriusReaderCollection(const char *ip);
		virtual ~RPUlteriusReaderCollection();

		void EnableType(RP_TYPE type, u8 enable);
		void EnableMask(u32 mask);
		bool m_imageInfoUpdated;

		void UpdateImageDescs();

	protected:
		std::map < RP_TYPE, u8 > m_types;
		std::map < RP_TYPE, uDataDesc > m_descs;
		ulterius* m_ulterius;
		FrameCollectionQueue m_frameQueue;
		QMutex m_lock;
		s32 m_mpp;

	};
};