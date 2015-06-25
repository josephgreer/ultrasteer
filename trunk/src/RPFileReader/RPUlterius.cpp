#include "RTCore.h"
#include "RPUlterius.h"

namespace Nf {

  static RPUlteriusReaderCollection *gThis = NULL;

  static bool onNewData(void* data, int type, int sz, bool cine, int frmnum)
  {
    u8 rv = true;
    if(gThis) {
      rv = gThis->AddData(data, type, sz, cine, frmnum);
    } else {
      rv = false;
    }
    return (bool)rv;
  }

  static bool paramCallback(void* paramID, int x, int y)
  {
    if(strcmp((char *)paramID, "b-image rect") == 0)
      gThis->m_imageInfoUpdated = true;
    return true;
  }

  static Squarei squarei_uDataDesc(const uDataDesc & desc)
  {
    Squarei res(Vec2i(desc.roi.ulx, desc.roi.uly), Vec2i(desc.roi.brx, desc.roi.bry));
    return res;
  }

  RPUlteriusReaderCollection::RPUlteriusReaderCollection(const char *ip, f64 mpp, Vec2d origin)
    : m_mask(0)
    , m_mpp(mpp)
    , m_origin(origin)
  {
    m_imageInfoUpdated = true;
    m_ulterius = new ulterius();

    s32 nTypes = sizeof(RP_TYPE_LIST)/sizeof(RP_TYPE_LIST[0]);
    for(s32 i=0; i<nTypes; i++) {
      m_types[(RP_TYPE)RP_TYPE_LIST[i]] = 0;
    }

    gThis = this;

    m_ulterius->connect(ip);

    m_ulterius->setSharedMemoryStatus(0);

    m_ulterius->setCallback(onNewData);
    m_ulterius->setParamCallback(paramCallback);
  }

  RPUlteriusReaderCollection::~RPUlteriusReaderCollection()
  {
    gThis = NULL;
  }

  RPData RPUlteriusReaderCollection::GetNextRPData()
  {
    if(!m_lock.tryLock(30))
      return RPData();

    if(m_mask&RPF_GPS && !m_gps.gps.valid) {
      m_lock.unlock();
      return RPData();
    }

    RPData res;
    bool fullSet = false;
    u32 mask = m_mask&(~RPF_GPS);
    if(m_frameQueue.size() > 0) {
      FrameCollectionQueue::reverse_iterator i = m_frameQueue.rbegin();
      do {
        fullSet = i->second.FullSet(mask);
        if(!fullSet)
          i++;
      } while(i!= m_frameQueue.rend() && !fullSet);

      if(i != m_frameQueue.rend()) {
        std::pair < u32, RPData > temp = *i;
        res = temp.second;
        m_frameQueue.erase(std::next(i).base());
      }
    }

    m_lock.unlock();
    res.origin = m_origin;
    res.mpp = (f32)m_mpp;
    res.gps = m_gps.gps;
    return res;
  }

  void RPUlteriusReaderCollection::EnableType(RP_TYPE type, u8 enable)
  {
    uDataDesc desc;
    m_ulterius->getDataDescriptor((uData)type, desc);
    m_descs[type] = desc;
    u32 mask = m_ulterius->getDataToAcquire();
    bool rv = m_ulterius->setDataToAcquire(mask|type);
    ASSERT(rv);
    m_mask = mask;
  }

  void RPUlteriusReaderCollection::EnableMask(u32 mask)
  {
    uDataDesc desc;
    m_ulterius->getDataDescriptor((uData)RPF_BPOST8, desc);
    m_descs[RPF_BPOST8] = desc;

    m_ulterius->getDataDescriptor((uData)RPF_BPOST32, desc);
    m_descs[RPF_BPOST32] = desc;

    m_ulterius->getDataDescriptor((uData)RPF_COLOR, desc);
    m_descs[RPF_COLOR] = desc;

    bool rv = m_ulterius->setDataToAcquire(mask);
    ASSERT(rv);
    m_mask = mask;
  }

  uDataDesc RPUlteriusReaderCollection::GetImageDesc(RP_TYPE type)
  {
    return m_descs[type];
  }

  void RPUlteriusReaderCollection::UpdateImageDescs()
  {
    for(std::map < RP_TYPE, uDataDesc >::iterator i = m_descs.begin(); i != m_descs.end(); i++) {
      m_ulterius->getDataDescriptor((uData)i->first, m_descs[i->first]);
    }
    //To be safe, get B8 data desc because we use that elsewhere
    m_ulterius->getDataDescriptor((uData)RPF_BPOST8, m_descs[RPF_BPOST8]);
  }

  void RPUlteriusReaderCollection::DeleteFramesBefore(s32 frame)
  {
    s32 num = m_frameQueue.begin()->first;
    while(num < frame) {
      std::pair < u32, RPData > el = *m_frameQueue.begin();
      el.second.Release();
      m_frameQueue.erase(m_frameQueue.begin());
      num = m_frameQueue.begin()->first;
    }
  }

  void RPUlteriusReaderCollection::PurgeExcessFrames()
  {
    while(m_frameQueue.size() > MAX_FRAMES) {
      std::pair < u32, RPData > el = *m_frameQueue.begin();
      el.second.Release();
      m_frameQueue.erase(m_frameQueue.begin());
      //printf("Dropping Frame\n");
    }
  }

  u8 RPUlteriusReaderCollection::AddData(void* data, int type, int sz, bool cine, int frmnum)
  {
    u32 tick = GetTickCount();
    RP_TYPE tp = (RP_TYPE)type;
    uDataDesc desc = GetImageDesc(tp);


    CvSize size = cvSize(desc.w, desc.h);
    RPData rp;
    switch(tp) {
        case RPF_BPOST8: 
          {
            IplImage *im = cvCreateImage(size/*cvSize(desc.roi.brx-desc.roi.ulx, desc.roi.bry-desc.roi.uly)*/,
              IPL_DEPTH_8U, 1);
            rp.b8 = im;

            u8 *psrc = (u8 *)data;//+(desc.w/**desc.roi.uly+desc.roi.ulx*/)*(desc.ss>>3);
            u8 *pdst = (u8 *)im->imageData;
            for(s32 r=0; r<im->height; r++) 
              memcpy(pdst+r*im->widthStep, psrc+r*desc.w*(desc.ss>>3), im->width);
          }
          break;
        case RPF_BPOST32:
        case RPF_COLOR:
          {
            IplImage *im = cvCreateImage(size/*cvSize(desc.roi.brx-desc.roi.ulx, desc.roi.bry-desc.roi.uly)*/,
              IPL_DEPTH_8U, 4);
            rp.color = im;

            u8 *psrc = (u8 *)data;//+(desc.w/**desc.roi.uly+desc.roi.ulx*/)*(desc.ss>>3);
            u8 *pdst = (u8 *)im->imageData;
            for(s32 r=0; r<im->height; r++) 
              memcpy(pdst+r*im->widthStep, psrc+r*desc.w*(desc.ss>>3), im->width*(desc.ss>>3));
          }
          break;
        case RPF_GPS:
          {
            u8 *raw = (u8 *)data;
            s32 idx = 0;
            rp.gps.tick = tick;
            rp.gps.pos.x = *(double *)&raw[idx];  idx += sizeof(double);
            rp.gps.pos.y = *(double *)&raw[idx];   idx += sizeof(double);
            rp.gps.pos.z = *(double *)&raw[idx];   idx += sizeof(double);

            rp.gps.posaer.x = *(double *)&raw[idx];  idx += sizeof(double);
            rp.gps.posaer.y = *(double *)&raw[idx];   idx += sizeof(double);
            rp.gps.posaer.z = *(double *)&raw[idx];   idx += sizeof(double);

            double m[9];
            for(int i=0; i<9; i++) {
              m[i] = *(double *)&raw[idx];   idx += sizeof(double);
            }
            double me[16] = {m[0], m[1], m[2], 0, 
              m[3], m[4], m[5], 0, 
              m[6], m[7], m[8], 0, 
              0,    0,    0,    1};
            cv::Mat(4,4,CV_64F,me).copyTo(rp.gps.pose);
            rp.gps.pose = rp.gps.pose.t();

            for(int i=0; i<5; i++) {
              rp.gps.offset[i] = *(double *)&raw[idx];  idx += sizeof(double);
            }

            rp.gps.quality = *(unsigned short *)&raw[idx];  idx+= sizeof(unsigned short);

            rp.gps.valid = 1;
          }
          break;
    }

    if(!m_lock.tryLock(20)) {
      rp.Release();
      return 0;
    }
    if(m_imageInfoUpdated) {
      UpdateImageDescs();
      m_imageInfoUpdated = false;
    }
    if((RP_TYPE)type == RPF_GPS) {
      ////if it's gps just replace the last sample with gps
      //if(m_frameQueue.size())
      //  frmnum = m_frameQueue.rbegin()->first;
      //else
      //  frmnum = 0;
      m_gps = rp;
      m_lock.unlock();
      return 1;
    }
    RPData rpp = m_frameQueue[frmnum];
    switch(tp) {
        case RPF_BPOST8: 
          {
            rpp.b8 = rp.b8;
          }
          break;
        case RPF_BPOST32:
        case RPF_COLOR:
          {
            rpp.color = rp.color;
          }
          break;
        case RPF_GPS:
          {
            rpp.gps = rp.gps;
          }
          break;

    }

    rpp.roi = squarei_uDataDesc(m_descs[RPF_BPOST8]);

    m_frameQueue[frmnum] = rpp;

    //DeleteFramesBefore(frmnum);
    PurgeExcessFrames();

    m_lock.unlock();
    return 1;
  }

};