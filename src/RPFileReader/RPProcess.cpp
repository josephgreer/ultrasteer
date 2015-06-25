#include <windows.h>
#include "RTCore.h"
#include "RPUlterius.h"
#include "RPProcess.h"
#include "UlteriusInterface.h"

namespace Nf {

  static void releaseRPData(RPData *data)
  {
    if(data->b8)
      cvReleaseImage(&data->b8);
    if(data->color)
      cvReleaseImage(&data->color);
    if(data->sig)
      cvReleaseImage(&data->sig);
    if(data->var)
      cvReleaseImage(&data->var);
  }

  static Squarei squarei_uDataDesc(const uDataDesc & desc)
  {
    Squarei res(Vec2i(desc.roi.ulx, desc.roi.uly), Vec2i(desc.roi.brx, desc.roi.bry));
    return res;
  }

  static s32 threadProc(void *pData)
  {
    ThreadContext *ctxt = (ThreadContext *)pData;

    HANDLE handles[10];
    uDataDesc desc = ctxt->descs[RPF_BPOST32];
    s32 maxBytes = desc.w*desc.h*4+sizeof(RPDatumHeader);
    u8 *buf = (u8 *)calloc(desc.w*desc.h*4+sizeof(RPDatumHeader), 1);
    while(!ctxt->done) {
      if(!ctxt->lock->tryLock(45)) {
        continue;
      }

      if(ctxt->queue.size() == 0) {
        ctxt->lock->unlock();
        Sleep(45);
        continue;
      }

      for(s32 i=0; i<(s32)ctxt->queue.size(); i++)
        handles[i] = ctxt->queue[i].hWrite;

      bool failed = false;


      //Wait for any of write events to be signaled.
      s32 rv = WaitForMultipleObjects(ctxt->queue.size(), &handles[0], false, 10);
      if(rv-WAIT_OBJECT_0 >= 0 && rv-WAIT_OBJECT_0 < (s32)ctxt->queue.size()) {
        //One of the write events was signaled
        s32 idx = rv-WAIT_OBJECT_0;

        DWORD bytesRead;
        if(ReadFile(ctxt->queue[idx].hPipe, buf, maxBytes, &bytesRead, NULL) != TRUE) {
          NTrace("Read File failed\n");
          ASSERT(0);
        }

        RPDatum datum;
        memcpy(&datum.header, buf, sizeof(RPDatumHeader));
        datum.data = buf+sizeof(RPDatumHeader);

        //We're done reading from pipe.
        //Reset the write event and signal read event for client process
        ResetEvent(handles[idx]);
        SetEvent(ctxt->queue[idx].hRead);

        //unlock handle lock since we're done with them
        ctxt->lock->unlock();

        //Now add the data to the queue (lock is relocked in AddData())
        ctxt->owner->AddData(&datum);
      } else if(rv-WAIT_ABANDONED_0 >= 0 && rv-WAIT_ABANDONED_0 < (s32)ctxt->queue.size()) {
        NTrace("RPProcess threadProc, Abandoned event\n");
        ASSERT(0);
      } else if(rv == WAIT_FAILED) {
        NTrace("RPProcess threadProc, Wait failed on event\n");
        ASSERT(0);
      } else if(rv != WAIT_TIMEOUT){
        NTrace("RPProcess threadProc, Hmm not sure what happend waiting for event to be signaled\n");
        ASSERT(0);
      } else {
        ctxt->lock->unlock();
      }

    }

    free(buf);

    return 0;

  }

  RPUlteriusProcessManager::RPUlteriusProcessManager(const char *ip, f64 mpp, Vec2d origin, f32 framerate)
    : m_mpp(mpp)
    , m_origin(origin)
  {
    m_enabledTypes = 0;
    //11 fps by default
    m_msPerFrame = 1000.0f/framerate;

    m_ulterius.connect(ip);

    //save off all the data descriptors
    s32 nTypes = sizeof(RP_TYPE_LIST)/sizeof(RP_TYPE_LIST[0]);
    uDataDesc desc;
    for(s32 i=0; i<nTypes; i++) {
      m_ulterius.getDataDescriptor((uData)RP_TYPE_LIST[i], desc);
      m_descs[(RP_TYPE)RP_TYPE_LIST[i]] = desc;
    }
    m_ulterius.disconnect();

    strcpy(m_ip, ip);

    m_dataAvailable = 0;

    m_lock = new QMutex();

    m_tContext.owner = this;
    m_tContext.done = 0;
    m_tContext.lock = m_lock;
    m_tContext.descs = m_descs;
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadProc, &m_tContext, 0, 0);
  }

  RPUlteriusProcessManager::~RPUlteriusProcessManager()
  {
    //kill the thread.
    m_tContext.done = true;

    for(std::map < RP_TYPE, HandleSet >::iterator i=m_handles.begin(); i!=m_handles.end(); i++) {
      //Signal process is done
      BOOL rv = SetEvent(i->second.hDone);
      if(rv != TRUE) {
        printf("RPUlteriusProcessManager::~RPUlteriusProcessManager(), Error signaling done event\n");
      }
    }

    for(s32 i=0; i<(s32)m_tContext.queue.size(); i++) {
      if(CloseHandle(m_tContext.queue[i].hDone) != TRUE)
        NTrace("RPUlteriusProcessManager::~RPUlteriusProcessManager, Error closing done handle\n");
      if(CloseHandle(m_tContext.queue[i].hPipe) != TRUE)
        NTrace("RPUlteriusProcessManager::~RPUlteriusProcessManager, Error closing named pipe\n");
      if(CloseHandle(m_tContext.queue[i].hRead) != TRUE)
        NTrace("RPUlteriusProcessManager::~RPUlteriusProcessManager, Error closing read handle\n");
      if(CloseHandle(m_tContext.queue[i].hWrite) != TRUE)
        NTrace("RPUlteriusProcessManager::~RPUlteriusProcessManager, Error closing write handle\n");
    }
  }

  static bool checkFullSet(const RPData *data, u32 enabledTypes)
  {
    //GPS is kept in another stream and assembled at the end
    enabledTypes &= ~RPF_GPS;
    if(enabledTypes&RPF_BPOST8 && !data->b8)
      return false;
    if(((enabledTypes&RPF_COLOR)||(enabledTypes&RPF_BPOST32)) && !data->color)
      return false;
    if(enabledTypes&RPF_COLOR_CVV && !data->sig)
      return false;
    if(enabledTypes&RPF_COLOR_CVV_VAR && !data->var)
      return false;
    if(enabledTypes&RPF_GPS && !data->gps.valid)
      return false;
    return true;
  }

  static s32 countTypes(const RPData *data)
  {
    s32 rv=0;
    if(data->b8)
      rv++;
    if(data->color)
      rv++;
    if(data->gps.valid)
      rv++;
    if(data->sig || data->var)
      rv++;
    return rv;
  }

  static bool isSlotAvailable(const RPData *dst, const RPData *src, u32 type)
  {
    bool rv = false;
    switch(type) {
    case RPF_BPOST8:
      if(!dst->b8)
        rv = true;
      break;
    case RPF_BPOST32:
    case RPF_COLOR:
      if(!dst->color)
        rv = true;
      break;
    case RPF_COLOR_CVV:
      if(!dst->sig)
        rv = true;
      break;
    case RPF_COLOR_CVV_VAR:
      if(!dst->var)
        rv = true;
      break;
    case RPF_GPS:
      if(!dst->gps.valid)
        rv = true;
      break;
    }
    return rv;
  }

  u32 getTick(const RPData *data, u32 type)
  {
    u32 tick = 0;
    switch(type) {
    case RPF_BPOST8:
      if(data->color)
        tick = (u32)data->color->alphaChannel;
      else
        tick = data->gps.tick;
      break;
    case RPF_BPOST32:
    case RPF_COLOR:
      if(data->b8)
        tick = (u32)data->b8->alphaChannel;
      else
        tick = data->gps.tick;
      break;
    case RPF_COLOR_CVV:
    case RPF_COLOR_CVV_VAR:
      ASSERT(0);
      break;
    case RPF_GPS:
      if(data->gps.valid)
        tick = (u32)data->gps.tick;
      else if(data->color)
        tick = (u32)data->color->alphaChannel;
      else if(data->b8)
        tick = (u32)data->b8->alphaChannel;
      break;
    }

    return tick;
  }

  static void addDatum(RPData *dst, const RPData *src, u32 type)
  {
    switch(type) {
    case RPF_BPOST8:
      dst->b8 = src->b8;
      break;
    case RPF_BPOST32:
    case RPF_COLOR:
      dst->color = src->color;
      break;
    case RPF_COLOR_CVV:
      dst->sig = src->sig;
      break;
    case RPF_COLOR_CVV_VAR:
      dst->var = src->var;
      break;
    case RPF_GPS:
      dst->gps = src->gps;
      break;
    }
  }

  RPData RPUlteriusProcessManager::GetNextRPData()
  {
    if((!m_queue.size() && m_enabledTypes != RPF_GPS) || (!m_qGps.size() && m_enabledTypes&RPF_GPS)) {
      if(m_dataAvailable < -10) {
        printf("GetNextRPData():   No data available.  Are all data modalities enabled on the ultrasound machine?  Are all connections working?\n");
        m_dataAvailable = 0;
      }

      //decrement data available for error recording purposes
      m_dataAvailable--;

      return RPData();
    }
    //set to a high number so it takes a while to report again
    m_dataAvailable = 20;
    if(!m_lock->tryLock(30)) {
      printf("GetNextRPData():   Could not acquire lock\n");
      return RPData();
    }

    RPData res;
    bool fullSet = false;

    //if it's not GPS only
    if(m_enabledTypes != RPF_GPS) {
      if(m_queue.size() > 0 && m_qGps.size() > 0) {
        DatumQueue::iterator i = m_queue.begin();
        do {
          fullSet = checkFullSet(&(*i), m_enabledTypes);
          if(!fullSet)
            i++;
        } while(i!= m_queue.end() && !fullSet);

        if(i != m_queue.end()) {
          res = *i;
          m_queue.erase(i);
        }
      }

      if(fullSet) {
        //Find closest gps match and add to RPDatum 
        u32 tick = getTick(&res, RPF_GPS);
        s32 minDist = 100000;
        s32 minIdx = -1;
        for(s32 i=(s32)m_qGps.size()-1; i>=0; i--) {
          s32 dist = std::abs((s32)(m_qGps[i].tick-tick));
          if(dist < minDist) {
            minDist = dist;
            minIdx = i;
          }
        }
        res.gps = m_qGps[minIdx];
        m_qGps.erase(m_qGps.begin(), m_qGps.begin()+minIdx+1);
        //m_qGps.erase(m_qGps.begin()+minIdx);
      }
    } else { //GPS only
      res.gps = m_qGps.front();
      m_qGps.pop_front();
    }

    m_lock->unlock();
    res.origin = m_origin;
    res.mpp = (f32)m_mpp;
    return res;
  }

  HANDLE ghJob = NULL;

  void RPUlteriusProcessManager::EnableType(RP_TYPE type, u8 enable)
  {
    if(!enable) {
      //kill it 
    }

    if(m_handles.find(type) != m_handles.end()) {
      NTrace("RPUlteriusProcessManager::EnableType, Already receiving this type\n");
    }

    if(!m_lock->tryLock(1000)) {
      NTrace("RPUlteriusProcessManager::EnableType, Failed to acquire handleLock\n");
    }

    uDataDesc desc = m_descs[RPF_BPOST32];

    s32 nBytes;
    if(type == RPF_GPS)
      nBytes = sizeof(RPDatumHeader)+3*sizeof(GPS_Data);
    else if(type == RPF_BPOST8)
      nBytes = sizeof(RPDatumHeader)+3*desc.w*desc.h;
    else
      nBytes = sizeof(RPDatumHeader)+3*desc.w*desc.h*4;
    HandleSet handle;
    char baseStr[] = "AC87ED25-C831-44f1-A0A1-675F661C2460";
    char pipeName[500];
    sprintf(pipeName, "\\\\.\\pipe\\%s_%d", baseStr, (s32)type);

    handle.hPipe = CreateNamedPipeA(pipeName,            // pipe name 
      PIPE_ACCESS_INBOUND|PIPE_WAIT,
      PIPE_WAIT,                // blocking mode 
      2, // max. instances  
      nBytes,                  // output buffer size 
      nBytes,                  // input buffer size 
      0,                        // client time-out 
      NULL);         
    if(handle.hPipe == 0 || (u32)handle.hPipe == 0xFFFFFFFF) {
      NTrace("RPUlteriusProcessManager::EnableType, Error creating namedPipe %d\n", GetLastError());
      ASSERT(0);
    }

    char pName[500];
    sprintf(pName, "%s_%d.read",  baseStr, (s32)type);
    handle.hRead = CreateEventA(0, TRUE, FALSE, pName);
    if(handle.hRead == 0) {
      NTrace("RPUlteriusProcessManager::EnableType, Error creating read event %d\n", GetLastError());
      ASSERT(0);
    }

    sprintf(pName, "%s_%d.write",  baseStr, (s32)type);
    handle.hWrite = CreateEventA(0, TRUE, FALSE, pName);
    if(handle.hWrite == 0) {
      NTrace("RPUlteriusProcessManager::EnableType, Error creating write event %d\n", GetLastError());
      ASSERT(0);
    }

    sprintf(pName, "%s_%d.done",  baseStr, (s32)type);
    handle.hDone = CreateEventA(0, TRUE, FALSE, pName);
    if(handle.hDone == 0) {
      NTrace("RPUlteriusProcessManager::EnableType, Error creating done event %d\n", GetLastError());
      ASSERT(0);
    }

    m_tContext.queue.push_back(handle);

    //Create job object so that when we die our child processes go with us
    if(!ghJob) {
      ghJob = CreateJobObjectA( NULL, NULL );
      JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = { 0 };

      // Configure all child processes associated with the job to terminate when the
      jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
      if( 0 == SetInformationJobObject( ghJob, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli)))
      {
        NTrace("Error setting information job object\n");
      }
    }


    char startString[800];
    sprintf(pName, "%s_%d", baseStr, type);
#ifdef _DEBUG
    sprintf(startString, "F:/ultrasteer/build/UlteriusDataGrabber/Debug/UlteriusDataGrabber.exe -ip %s -n %s -t %d",
      m_ip, pName, type);
#else
    sprintf(startString, "F:/ultrasteer/build/UlteriusDataGrabber/Release/UlteriusDataGrabber.exe -ip %s -n %s -t %d",
      m_ip, pName, type);
#endif
    //Now Create the process
    STARTUPINFOA info = {sizeof(info)};
    PROCESS_INFORMATION pi;
    if(!CreateProcessA(NULL, (LPSTR)startString, NULL, NULL, FALSE, CREATE_BREAKAWAY_FROM_JOB, NULL, NULL, &info, &pi)) {
      NTrace("RPUlteriusProcessManager::EnableType, Error creating process %d\n", GetLastError());
      ASSERT(0);
    }

    NTrace("ghJob %d\n", ghJob);

    //Assign the process to our global job object
    if(0 == AssignProcessToJobObject( ghJob, pi.hProcess)) {
      NTrace("Failure to assign process to job object %d\n", GetLastError());
    }

    Sleep(30);

    m_lock->unlock();

    m_enabledTypes |= type;

    ASSERT(!((m_enabledTypes&RPF_COLOR)&&(m_enabledTypes&RPF_BPOST32)));
  }

  uDataDesc RPUlteriusProcessManager::GetImageDesc(RP_TYPE type)
  {
    return m_descs[type];
  }


  void RPUlteriusProcessManager::SetFrameRate(f32 framerate)
  {
    m_msPerFrame = 1000/framerate;
  }

  static void dumpPoseDumpPos(const GPS_Data *data)
  {
    NTrace("pos:  [%f %f %f]\n", data->pos.x, data->pos.y, data->pos.z);
    NTrace("pose: [%f %f %f; %f %f %f; %f %f %f];\n", data->pose.at<f64>(0,0), data->pose.at<f64>(1,0), data->pose.at<f64>(2,0),
      data->pose.at<f64>(0,1), data->pose.at<f64>(1,1), data->pose.at<f64>(2,1),
      data->pose.at<f64>(0,2), data->pose.at<f64>(1,2), data->pose.at<f64>(2,2));
  }

  s32 RPUlteriusProcessManager::AddData(const RPDatum *data)
  {
    RPData rp;
    u32 tick = data->header.tick;

    RP_TYPE type = (RP_TYPE)data->header.type;

    rp.roi = squarei_uDataDesc(m_descs[RPF_BPOST8]);

    //NTrace("Type %d Tick %d\n", type, tick);

    uDataDesc desc = m_descs[type];
    CvSize size = cvSize(desc.w, desc.h);
    switch(type) {
        case RPF_BPOST8: 
          {
            IplImage *im = cvCreateImage(size/*cvSize(desc.roi.brx-desc.roi.ulx, desc.roi.bry-desc.roi.uly)*/,
              IPL_DEPTH_8U, 1);
            rp.b8 = im;
            im->alphaChannel = (s32)data->header.tick;

            u8 *psrc = (u8 *)data->data;//+(desc.w/**desc.roi.uly+desc.roi.ulx*/)*(desc.ss>>3);
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
            im->alphaChannel = (s32)data->header.tick;
            rp.color = im;

            u8 *psrc = (u8 *)data->data;//+(desc.w/**desc.roi.uly+desc.roi.ulx*/)*(desc.ss>>3);
            u8 *pdst = (u8 *)im->imageData;
            for(s32 r=0; r<im->height; r++) 
              memcpy(pdst+r*im->widthStep, psrc+r*desc.w*(desc.ss>>3), im->width*(desc.ss>>3));
          }
          break;
        case RPF_GPS:
          {
            u8 *raw = (u8 *)data->data;
            s32 idx = 0;
            rp.gps.tick = tick;
            rp.gps.pos.x = *(double *)&raw[idx];  idx += sizeof(double);
            rp.gps.pos.y = *(double *)&raw[idx];   idx += sizeof(double);
            rp.gps.pos.z = *(double *)&raw[idx];   idx += sizeof(double);

            rp.gps.posaer.x = *(double *)&raw[idx];  idx += sizeof(double);
            rp.gps.posaer.y = *(double *)&raw[idx];   idx += sizeof(double);
            rp.gps.posaer.z = *(double *)&raw[idx];   idx += sizeof(double);

            //pose comes in memory in column major form, therefore 
            //need to take transpose
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



            //NTrace("pos x pos y posz %f %f %f\n", rp.gps.pos.x, rp.gps.pos.y, rp.gps.pos.z);
            if(false) {
              dumpPoseDumpPos(&rp.gps);
            }

            for(int i=0; i<5; i++) {
              rp.gps.offset[i] = *(double *)&raw[idx];  idx += sizeof(double);
            }

            rp.gps.quality = *(unsigned short *)&raw[idx];  idx+= sizeof(unsigned short);

            rp.gps.tick = data->header.tick;
            rp.gps.valid = 1;
          }
          break;
    }

    if(!m_lock->tryLock(45)) {
      releaseRPData(&rp);
      return 0;
    }
#if 0
    if((RP_TYPE)type == RPF_GPS) {
      //if it's gps just replace the last sample with gps
      if(m_frameQueue.size())
        frmnum = m_frameQueue.rbegin()->first;
      else
        frmnum = 0;
    }
    RPData rpp = m_frameQueue[frmnum];
    switch(tp) {
        case RPF_BPOST8: 
          {
            rpp.b8 = rp.b8;
          }
          break;
#if 0
        case RPF_BPOST32:
          {
            rpp.color = rp.color;
            rpp.b8 = rp.b8;
          }
          break;
#else
        case RPF_BPOST32:
#endif
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
#else
    if(type != RPF_GPS) {
      bool slotFound = false;
      s32 maxDist = m_msPerFrame/2;  //TODO:  FIGURE THIS VALUE OUT
      if(type == RPF_GPS)
        maxDist = m_msPerFrame;
      for(s32 i=0; i<(s32)m_queue.size(); i++) {
        if(isSlotAvailable(&m_queue[i], &rp, data->header.type)) {
          //slot is available 
          if(abs((s32)(getTick(&m_queue[i], type)-tick)) < maxDist) {
            //Should not be possible to fit into two separate slots
            ASSERT(!slotFound);
            addDatum(&m_queue[i], &rp, type);
            slotFound = true;
          } 
        }
      }
      if(!slotFound)
        m_queue.push_back(rp);
    } else {
      m_qGps.push_back(rp.gps);
    }
#endif

    //check if we have no complete sets
    if(m_queue.size() > MAX_FRAMES) {

      bool fullsetExists = false;
      for(s32 i=0; i<(s32)m_queue.size(); i++) {
        if(checkFullSet(&m_queue[i], m_enabledTypes)) {
          fullsetExists = true; 
          break;
        }
      }

      if(!fullsetExists) {
        printf("Not receiving all enabled data modalities.  Did you forget to enable everything on the ultrasound machine?\n");
      }
    }

    //s32 sz = m_frameQueue.size();
    while(m_queue.size() > MAX_FRAMES) {
      RPData data = m_queue.front();
      releaseRPData(&data);
      m_queue.pop_front();
      printf("Dropping Frame\n");
    }

    if(m_queue.size() > 0) {
      //look for sandwiched partial sets and remove them since there is no chance of getting a full set after that
      for(DatumQueue::iterator i=m_queue.begin(); i+1!=m_queue.end(); i++) {
        if(!checkFullSet(&(*i), m_enabledTypes) && checkFullSet(&(*(i+1)), m_enabledTypes)) {
          releaseRPData(&(*i));
          i = m_queue.erase(i);
        }
        if(i == m_queue.end() || i+1 == m_queue.end())
          break;
      }
    }

    while(m_qGps.size() > MAX_GPS) {
      printf("Dropping gps frames\n");
      m_qGps.pop_front();
    }

    m_lock->unlock();
    return 1;
  }

};