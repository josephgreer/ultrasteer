#include "RPFileReader.h"
#include <iostream>

namespace Nf
{
  typedef std::map < RP_TYPE, RPFileReader * >::iterator rpcol_it;

  static RPData readIplImage8uFromCurrentOffset(FILE *f, RPFileHeader *header)
  {
    IplImage *res = NULL;
    IplImage *var = NULL;

    RPData rv;

    switch(header->type) {
    //8bit b-mode post
    case RPF_BPOST8: 
      {
        rv.b8 = cvCreateImage(cvSize(header->width, header->height), IPL_DEPTH_8U, 1);
        char *dst = rv.b8->imageData;
        int rvc;
        for(int i=0; i<header->height; i++) {
          rvc = fread(dst, 1, header->width, f);
          if(rvc != header->width)
            OutputDebugString("Failed to read");
          dst += rv.b8->widthStep;
        }
      }
      break;
    case RPF_BPOST32:
    case RPF_COLOR:
      {
        rv.color = cvCreateImage(cvSize(header->width, header->height), IPL_DEPTH_8U, 4);
        char *dst = rv.color->imageData;
        int rvc = 0;
        for(int i=0; i<header->height; i++) {
          rvc = fread(dst, 1, header->width*4, f);
          if(rvc != header->width*4)
            OutputDebugString("Failed to read");
          dst += rv.color->widthStep;
        }
      }

      break;
    case RPF_COLOR_CVV:
      {
        rv.sig = cvCreateImage(cvSize(header->width, header->height), IPL_DEPTH_8U, 1);
        rv.var = cvCreateImage(cvSize(header->width, header->height), IPL_DEPTH_8U, 1);
        char *rdst = rv.sig->imageData;
        char *vdst = rv.var->imageData;

        int rvc;
        for(int i=0; i<header->height; i++) {
          rvc = fread(rdst, 1, header->width*1, f);
          if(rvc != header->width*1)
            OutputDebugString("Failed to read\n");
          //for(int j=0; j<header->width; j+=2) {
          //  rdst[j>>1] = temp[j+1];
          //  vdst[j>>1] = temp[j+1];
          //}
          rdst += rv.sig->widthStep;
        }
        for(int i=0; i<header->height; i++) {
          rvc = fread(vdst, 1, header->width*1, f);
          if(rvc != header->width*1)
            OutputDebugString("Failed to read\n");
          //for(int j=0; j<header->width; j+=2) {
          //  rdst[j>>1] = temp[j+1];
          //  vdst[j>>1] = temp[j+1];
          //}
          vdst += rv.var->widthStep;
        }
      }

      break;
    default:
      assert(0);
    }

    rv.mpp = header->dr;
    rv.origin = Vec2d(header->ld, header->extra);

    return rv;
  }

  static int readGPSDatumImageFromCurrentOffset(FILE *f, RPFileHeader *header, GPS_Data *data)
  {
    assert(header->type == RPF_GPS);
    assert(f != NULL);

    char raw[168] = {0};

    int rvc = 0;
    rvc = fread(&raw[0], sizeof(char), 168, f);
    if(rvc != 168)
      OutputDebugString("Failed to read GPS\n");

    int idx = 0;
    data->pos.x = *(double *)&raw[idx];  idx += sizeof(double);
    data->pos.y = *(double *)&raw[idx];   idx += sizeof(double);
    data->pos.z = *(double *)&raw[idx];   idx += sizeof(double);

    data->posaer.x = *(double *)&raw[idx];  idx += sizeof(double);
    data->posaer.y = *(double *)&raw[idx];   idx += sizeof(double);
    data->posaer.z = *(double *)&raw[idx];   idx += sizeof(double);

    double m[9];
    for(int i=0; i<9; i++) {
      m[i] = *(double *)&raw[idx];   idx += sizeof(double);
    }
    //matrix is in column major form
    //data->pose = Matrix44d(m[0*3+0], m[1*3+0], m[2*3+0], 0,
    //  m[0*3+1], m[1*3+1], m[2*3+1], 0,
    //  m[0*3+2], m[1*3+2], m[2*3+2], 0,
    //  0,        0,        0,        1);
	double me[16] = {m[0], m[1], m[2], 0, 
					 m[3], m[4], m[5], 0, 
					 m[6], m[7], m[8], 0, 
					 0,    0,    0,    1};


	cv::Mat(4,4,CV_64F,me).copyTo(data->pose);
  data->pose = data->pose.t();

    for(int i=0; i<5; i++) {
      data->offset[i] = *(double *)&raw[idx];  idx += sizeof(double);
    }

    data->quality = *(unsigned short *)&raw[idx];  idx+= sizeof(unsigned short);
    data->valid = 1;
    //data->tick = std::time_tck

    return 0;
  }

  RPGPSReader::RPGPSReader(const char *path)
    : m_idx(0)
  {
    m_file = fopen(path, "rb");
    if(!m_file)
      return;

    fread(&m_header, 1, sizeof(RPFileHeader), m_file);

    m_bytesPerDatum = 168;
  }

  RPGPSReader::~RPGPSReader()
  {
    if(m_file)
      fclose(m_file);
  }

  GPS_Data RPGPSReader::GetNextGPSDatum()
  {
    if(!m_file || m_idx < 0 || m_idx+1 >= m_header.frames)
      return GPS_Data();

    m_idx++;

    GPS_Data data;
    readGPSDatumImageFromCurrentOffset(m_file, &m_header, &data);
    return data;
  }

  GPS_Data RPGPSReader::GetPreviousGPSDatum()
  {
    int idx = m_idx-1 >= 0 ? m_idx-1 : 0;
    return GetGPSDatum(idx);
  }

  GPS_Data RPGPSReader::GetGPSDatum(int frame)
  {
    m_idx = frame;
    if(!m_file || m_idx < 0 || m_idx >= m_header.frames)
      return GPS_Data();

    fseek(m_file, sizeof(m_header)+m_idx*m_bytesPerDatum, SEEK_SET);

    GPS_Data data;
    readGPSDatumImageFromCurrentOffset(m_file, &m_header, &data);
    return data;
  }

  int RPGPSReader::GetCurrentFrame()
  {
    return m_idx;
  }

  RPFileReader::RPFileReader(const char *path)
    : m_idx(0)
  {
    m_file = fopen(path, "rb");
    if(!m_file)
      return;

    fread(&m_header, 1, sizeof(RPFileHeader), m_file);

    switch(m_header.type) {
      case RPF_BPOST8:
        m_bytesPerIm = m_header.width*m_header.height;
        break;
      case RPF_BPOST32:
      case RPF_COLOR:
        m_bytesPerIm = m_header.width*m_header.height*4;
        break;
      case RPF_COLOR_CVV:
        m_bytesPerIm = m_header.width*m_header.height*2;
      default:
        break;
    }
  }

  RPFileReader::~RPFileReader()
  {
    if(m_file)
      fclose(m_file);
  }

  RPFileHeader RPFileReader::GetHeader()
  {
    return m_header;
  }

  RPData RPFileReader::GetNextRPData()
  {
    if(!m_file || m_idx < 0 || m_idx+1 >= m_header.frames) { 
      return RPData();
    }

    m_idx++;

    return readIplImage8uFromCurrentOffset(m_file, &m_header);
  }

  RPData RPFileReader::GetPreviousRPData()
  {
    int idx = m_idx-1 >= 0 ? m_idx-1 : 0;
    return GetRPData(idx);
  }

  RPData RPFileReader::GetRPData(int frame)
  {
    m_idx = frame;
    if(!m_file || m_idx < 0 || m_idx >= m_header.frames) {
      return RPData();
    }

    fseek(m_file, sizeof(m_header)+m_idx*m_bytesPerIm, SEEK_SET);

    return readIplImage8uFromCurrentOffset(m_file, &m_header);
  }

  int RPFileReader::GetCurrentFrame()
  {
    u32 pos = ftell(m_file);
    pos -= sizeof(RPFileHeader);
    ASSERT(pos%m_bytesPerIm == 0);
    pos = pos/m_bytesPerIm; 
    //pos now contains the number of frames that have been read.  
    return pos;
  }

  RP_TYPE RPFileReader::GetType()
  {
    return (RP_TYPE)m_header.type;
  }

  RPFileReaderCollection::RPFileReaderCollection() 
    : m_type(RPF_NULL_TYPE)
    , m_gps(NULL)
  {
  }

  
  RPFileReaderCollection::~RPFileReaderCollection()
  {
    for(std::map < RP_TYPE, RPFileReader * >::iterator i = m_readers.begin(); i != m_readers.end(); i++) {
      delete i->second;
    }
  }


  void RPFileReaderCollection::AddReader(RP_TYPE type, RPReader * reader)
  {
    //TODO:  Make sure readers are synced up
    if(type == RPF_COLOR_CVV) {
      m_readers[RPF_COLOR_CVV_VAR] = (RPFileReader *)reader;
    }
    if(type == RPF_COLOR_CVV_VAR) {
      m_readers[RPF_COLOR_CVV] = (RPFileReader *)reader;
    }
    m_readers[type] = (RPFileReader *)reader;
  }

  
  void RPFileReaderCollection::AddGPSReader(RPGPSReaderBasic *gps)
  {
    m_gps = (RPGPSReader *)gps;
  }

  s32 RPFileReaderCollection::GetCurrentFrame()
  {
    return m_readers.begin()->second->GetCurrentFrame();
  }


  RPData RPFileReaderCollection::GetNextRPData()
  {
    RPData rv;
    for(std::map < RP_TYPE, RPFileReader * >::iterator i = m_readers.begin(); i != m_readers.end(); i++) {
      if(i->first == RPF_COLOR_CVV_VAR)
        continue;
      RPData temp = i->second->GetNextRPData();
      switch(i->first) {
          case RPF_BPOST8:
            rv.b8 = temp.b8;
            if(temp.origin.x > 0 && temp.origin.y > 0)
              rv.origin = temp.origin;
            if(temp.mpp)
              rv.mpp = temp.mpp;
            break;
          case RPF_BPOST32:
          case RPF_COLOR:
            rv.color = temp.color;
            if(temp.origin.x > 0 && temp.origin.y > 0)
              rv.origin = temp.origin;
            if(temp.mpp)
              rv.mpp = temp.mpp;
            break;
          case RPF_COLOR_CVV:
            rv.sig = temp.sig;
            rv.var = temp.var;
            break;
          case RPF_COLOR_CVV_VAR:
            rv.var = temp.var;
            rv.sig = temp.sig;
            break;
          default:
            break;
      }
    }
    if(m_gps)
      rv.gps = m_gps->GetNextGPSDatum();
    else 
	  rv.gps = GPS_Data();

    RPFileHeader header;
    if(m_readers.find(RPF_BPOST8) != m_readers.end())
      header = m_readers[RPF_BPOST8]->GetHeader();
    else if(m_readers.find(RPF_BPOST32) != m_readers.end())
      header = m_readers[RPF_BPOST32]->GetHeader();
    else
      header = m_readers[RPF_COLOR]->GetHeader();
    rv.roi = Squarei(Vec2i(header.ul[0], header.ul[1]), Vec2i(header.br[0], header.br[1]));

    if(rv.origin.x == 0 && rv.origin.y == 0)
      rv.origin = Vec2d(320.0, 0.0);
    if(rv.mpp == 0)
      rv.mpp = 83;

    return rv;
  }

  RPData RPFileReaderCollection::GetPreviousRPData()
  {
    RPData rv;
    for(std::map < RP_TYPE, RPFileReader * >::iterator i = m_readers.begin(); i != m_readers.end(); i++) {
      if(i->first == RPF_COLOR_CVV_VAR)
        continue;
      RPData temp = i->second->GetPreviousRPData();
      switch(i->first) {
          case RPF_BPOST8:
            rv.b8 = temp.b8;
            if(temp.origin.x > 0 && temp.origin.y > 0)
              rv.origin = temp.origin;
            if(temp.mpp)
              rv.mpp = temp.mpp;
            break;
          case RPF_BPOST32:
          case RPF_COLOR:
            rv.color = temp.color;
            if(temp.origin.x > 0 && temp.origin.y > 0)
              rv.origin = temp.origin;
            if(temp.mpp)
              rv.mpp = temp.mpp;
            break;
          case RPF_COLOR_CVV:
            rv.sig = temp.sig;
            rv.var = temp.var;
            break;
          case RPF_COLOR_CVV_VAR:
            rv.var = temp.var;
            rv.sig = temp.sig;
            break;
          default:
            break;
      }
    }
    if(m_gps)
      rv.gps = m_gps->GetPreviousGPSDatum();
    else 
      rv.gps = GPS_Data();

    RPFileHeader header;
    if(m_readers.find(RPF_BPOST8) != m_readers.end())
      header = m_readers[RPF_BPOST8]->GetHeader();
    else if(m_readers.find(RPF_BPOST32) != m_readers.end())
      header = m_readers[RPF_BPOST32]->GetHeader();
    else
      header = m_readers[RPF_COLOR]->GetHeader();
    rv.roi = Squarei(Vec2i(header.ul[0], header.ul[1]), Vec2i(header.br[0], header.br[1]));

    if(rv.origin.x == 0 && rv.origin.y == 0)
      rv.origin = Vec2d(320.0, 0.0);
    if(rv.mpp == 0)
      rv.mpp = 83;
    
    return rv;
  }

  RPData RPFileReaderCollection::GetRPData(int frame)
  {
    frame = frame-1;
    RPData rv;
    for(std::map < RP_TYPE, RPFileReader * >::iterator i = m_readers.begin(); i != m_readers.end(); i++) {
      if(i->first == RPF_COLOR_CVV_VAR)
        continue;
      RPData temp = i->second->GetRPData(frame);
      switch(i->first) {
          case RPF_BPOST8:
            rv.b8 = temp.b8;
            if(temp.origin.x > 0 && temp.origin.y > 0)
              rv.origin = temp.origin;
            if(temp.mpp)
              rv.mpp = temp.mpp;
            break;
          case RPF_BPOST32:
          case RPF_COLOR:
            rv.color = temp.color;
            if(temp.origin.x > 0 && temp.origin.y > 0)
              rv.origin = temp.origin;
            if(temp.mpp)
              rv.mpp = temp.mpp;
            break;
          case RPF_COLOR_CVV:
            rv.sig = temp.sig;
            rv.var = temp.var;
            break;
          case RPF_COLOR_CVV_VAR:
            rv.var = temp.var;
            rv.sig = temp.sig;
            break;
          default:
            break;
      }
    }
    if(m_gps)
      rv.gps = m_gps->GetGPSDatum(frame);
    else 
      rv.gps = GPS_Data();

    RPFileHeader header;
    if(m_readers.find(RPF_BPOST8) != m_readers.end())
      header = m_readers[RPF_BPOST8]->GetHeader();
    else if(m_readers.find(RPF_BPOST32) != m_readers.end())
      header = m_readers[RPF_BPOST32]->GetHeader();
    else
      header = m_readers[RPF_COLOR]->GetHeader();
    rv.roi = Squarei(Vec2i(header.ul[0], header.ul[1]), Vec2i(header.br[0], header.br[1]));

    if(rv.origin.x == 0 && rv.origin.y == 0)
      rv.origin = Vec2d(320.0, 0.0);
    if(rv.mpp == 0)
      rv.mpp = 83;
    
    return rv;
  }
};