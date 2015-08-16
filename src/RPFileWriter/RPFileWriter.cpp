#include "RPFileWriter.h"
#include <conio.h>
#include <stdio.h>

namespace Nf
{
  static int writeGPSDatum(FILE *f, RPFileHeader *header, const GPS_Data *data)
  {
    assert(header->type == RPF_GPS || header->type == RPF_GPS2);
    assert(f != NULL);

    char raw[168] = {0};
    int idx = 0;
    memcpy(&raw[idx], &data->pos.x, sizeof(double));  idx += sizeof(double);
    memcpy(&raw[idx], &data->pos.y, sizeof(double));  idx += sizeof(double);
    memcpy(&raw[idx], &data->pos.z, sizeof(double));  idx += sizeof(double);

    memcpy(&raw[idx], &data->posaer.x, sizeof(double));  idx += sizeof(double);
    memcpy(&raw[idx], &data->posaer.y, sizeof(double));  idx += sizeof(double);
    memcpy(&raw[idx], &data->posaer.z, sizeof(double));  idx += sizeof(double);

    //pose is stored in memory in column-major form
    memcpy(&raw[idx], &data->pose.at<f64>(0,0), sizeof(double));  idx += sizeof(double);
    memcpy(&raw[idx], &data->pose.at<f64>(1,0), sizeof(double));  idx += sizeof(double);
    memcpy(&raw[idx], &data->pose.at<f64>(2,0), sizeof(double));  idx += sizeof(double);
    memcpy(&raw[idx], &data->pose.at<f64>(0,1), sizeof(double));  idx += sizeof(double);
    memcpy(&raw[idx], &data->pose.at<f64>(1,1), sizeof(double));  idx += sizeof(double);
    memcpy(&raw[idx], &data->pose.at<f64>(2,1), sizeof(double));  idx += sizeof(double);
    memcpy(&raw[idx], &data->pose.at<f64>(0,2), sizeof(double));  idx += sizeof(double);
    memcpy(&raw[idx], &data->pose.at<f64>(1,2), sizeof(double));  idx += sizeof(double);
    memcpy(&raw[idx], &data->pose.at<f64>(2,2), sizeof(double));  idx += sizeof(double);

    memcpy(&raw[idx], &data->offset[0], sizeof(double)*5);  idx += sizeof(double)*5;
    memcpy(&raw[idx], &data->quality, sizeof(u16));  idx += sizeof(u16);
    memset(&raw[idx], 0, 3*sizeof(u16));  idx += 3*sizeof(u16);

    if(fwrite(&raw[0], 1, 168, f) != 168) 
      printf("writeGPSDatum, Failed to write GPS correctly\n");

    return 0;
  }

  RPGPSWriter::RPGPSWriter(const char *path, const RPFileHeader *header)
  {
    m_file = fopen(path, "wb");
    if(!m_file) {
      printf("RPGPSWriter::RPGPSWriter, Failed to open GPS file\n");
      return;
    }

    char raw[sizeof(RPFileHeader)];  memset(raw, 0, sizeof(RPFileHeader));
    fwrite(m_file, 1, sizeof(RPFileHeader), m_file);

    m_header = *header;

    m_bytesPerDatum = 168;
  }

  RPGPSWriter::~RPGPSWriter()
  {
    if(m_file)
      fclose(m_file);
  }

  s32 RPGPSWriter::Cleanup(const RPFileHeader *header) 
  {
    RPFileHeader _header = *header;
    if(fseek(m_file, 0, SEEK_SET) != 0) {
      printf("RPGPSWriter::Cleanup, Error seeking GPS file to beginning\n");
      return -1;
    }

    if(fwrite(&_header, 1, sizeof(RPFileHeader), m_file) != sizeof(RPFileHeader)) {
      printf("RPGPSWriter::Cleanup, Error writing GPS header\n");
    }

    fclose(m_file);
    m_file = NULL;
    return 0;
  }

  s32 RPGPSWriter::WriteNextGPSDatum(const GPS_Data *gps)
  {
    if(!m_file)
      return -1;

    return writeGPSDatum(m_file, &m_header, gps);
  }

  RPFileWriter::RPFileWriter(const char *path, const RPFileHeader *header, int type)
  {
    if(type != RPF_BPOST8 && type != RPF_BPOST32 && type != RPF_COLOR) {
      printf("RPFileWriter::RPFileWriter, Error unrecognized file type\n");
    }
    m_file = fopen(path, "wb");
    if(!m_file) {
      printf("RPFileWriter::RPFileWriter, Error opening image file\n");
      return;
    }

    char raw[sizeof(RPFileHeader)];  memset(raw, 0, sizeof(RPFileHeader));
    fwrite(m_file, 1, sizeof(RPFileHeader), m_file);

    m_header = *header;
    m_type = type;

    switch(type) {
      case RPF_BPOST8:
        m_bytesPerIm = header->width*header->height;
        break;
      case RPF_BPOST32:
        m_bytesPerIm = header->width*header->height*4;
        break;
      case RPF_COLOR:
        m_bytesPerIm = header->width*header->height*4;
        break;
      default:
        printf("RPFileWriter::RPFileWriter, Unrecognized file type\n");
        break;
    }
  }

  RPFileWriter::~RPFileWriter()
  {
    if(m_file)
      fclose(m_file);
  }

  s32 RPFileWriter::WriteNextImage(const IplImage *im)
  {
    if(!m_file) {
      printf("RPFileWriter::WriteNextImage, m_file is NULL\n");
      return -1;
    }

    s32 bytesWritten = 0;
    u8 mul = 1;
    if(m_type == RPF_BPOST32 || m_type == RPF_COLOR)
      mul = 4;

    for(s32 r=0; r<im->height; r++) {
      if(fwrite(im->imageData+r*im->widthStep, 1, im->width*mul, m_file) != im->width*mul)
        printf("RPFileWriter::WriteNextImage, Error Writing Image Row\n");
      bytesWritten += im->width*mul;
    }
    if(bytesWritten != im->width*im->height*mul) {
      printf("RPFileWriter::WriteNextImage, Bytes Written was incorrect\n");
      return -1;
    }

    return 0;
  }

  s32 RPFileWriter::Cleanup(const RPFileHeader *header) 
  {
    RPFileHeader _header = *header;
    _header.type = m_type;
    if(fseek(m_file, 0, SEEK_SET) != 0) {
      printf("RPGPSWriter::Cleanup, Error seeking GPS file to beginning\n");
      return -1;
    }

    if(fwrite(&_header, 1, sizeof(RPFileHeader), m_file) != sizeof(RPFileHeader)) {
      printf("RPGPSWriter::Cleanup, Error writing GPS header\n");
    }

    fclose(m_file);
    m_file = NULL;
    return 0;
  }

  RPFileWriterCollection::RPFileWriterCollection(const char *path, const RPFileHeader *header) 
  {
    strcpy(m_path, path);
    m_header = *header;
    m_writeOver = false;

    m_gps = NULL;
    m_gps2 = NULL;
  }

  s32 RPFileWriterCollection::AddWriter(RP_TYPE type)
  {
    if(type != RPF_BPOST8 && type != RPF_BPOST32 && type != RPF_COLOR) {
      printf("RPFileWriterCollection::AddWriter, Unsupported writing type\n");
      return -1;
    }

    char extension[20];
    switch(type) {
      case RPF_BPOST8:
        strcpy(extension, "b8");
        break;
      case RPF_BPOST32:
        strcpy(extension, "b32");
        break;
      case RPF_COLOR:
        strcpy(extension, "col");
        break;
      default:
        printf("RPFileWriterCollection::AddWriter, Unrecognized writer type\n");
        break;
    }

    char filename[100];
    sprintf(filename, "%s.%s", m_path, extension);

    FILE *temp = fopen(filename, "rb");
    if(temp && !m_writeOver) {
      printf("File %s already exists.  Do you want to open? (Y/N)\n", filename);


      bool writeOver = false;
      bool run = true;
      while(run) {
        if(kbhit()) {
          char input = getch();
          switch(input) {
        case 'y':
        case 'Y':
          writeOver = true;
          break;
        case 'n':
        case 'N':
          writeOver = false;
          break;
        default:
          break;
          }
          run = false;
        }
      }
      if(!writeOver) {
        return  -1;
      } else {
        m_writeOver = true;
      }
    }
    if(temp)
      fclose(temp);

    RPFileHeader _header = m_header;
    _header.type = (int)type;
    RPFileWriter *writer = new RPFileWriter(filename, &_header, _header.type);

    m_writers[type] = writer;
    return 0;
  }

  
  s32 RPFileWriterCollection::AddGPSWriter()
  {
    char filename[100];
    sprintf(filename, "%s.%s", m_path, "gps1");
    

    FILE *temp = fopen(filename, "rb");
    if(temp && !m_writeOver) {
      printf("File %s already exists.  Do you want to open? (Y/N)\n", filename);


      bool writeOver = false;
      bool run = true;
      while(run) {
        if(kbhit()) {
          char input = getch();
          switch(input) {
        case 'y':
        case 'Y':
          writeOver = true;
          break;
        case 'n':
        case 'N':
          writeOver = false;
          break;
        default:
          break;
          }
          run = false;
        }
      }
      if(!writeOver) {
        return  -1;
      } else {
        m_writeOver = true;
      }
    }
    if(temp)
      fclose(temp);


    RPFileHeader _header = m_header;
    _header.type = RPF_GPS;
    m_gps = new RPGPSWriter(filename, &_header);
    return 0;
  }

  
  s32 RPFileWriterCollection::AddGPSWriter2()
  {
    char filename[100];
    sprintf(filename, "%s.%s", m_path, "gps2");
    

    FILE *temp = fopen(filename, "rb");
    if(temp && !m_writeOver) {
      printf("File %s already exists.  Do you want to open? (Y/N)\n", filename);


      bool writeOver = false;
      bool run = true;
      while(run) {
        if(kbhit()) {
          char input = getch();
          switch(input) {
        case 'y':
        case 'Y':
          writeOver = true;
          break;
        case 'n':
        case 'N':
          writeOver = false;
          break;
        default:
          break;
          }
          run = false;
        }
      }
      if(!writeOver) {
        return  -1;
      } else {
        m_writeOver = true;
      }
    }
    if(temp)
      fclose(temp);


    RPFileHeader _header = m_header;
    _header.type = RPF_GPS2;
    m_gps2 = new RPGPSWriter(filename, &_header);
    return 0;
  }


  s32 RPFileWriterCollection::WriteNextRPData(const RPData *data)
  {
    for(std::map < RP_TYPE, RPFileWriter * >::iterator i = m_writers.begin(); i != m_writers.end(); i++) {
      switch(i->first) {
          case RPF_BPOST8:
            i->second->WriteNextImage(data->b8);
            break;
          case RPF_BPOST32:
          case RPF_COLOR:
            i->second->WriteNextImage(data->color);
            break;
          default:
            printf("RPFileWriterCollection::WriteNextRPData, unrecognized type\n");
            break;
      }
    }
    if(m_gps)
      m_gps->WriteNextGPSDatum(&data->gps);
    if(m_gps2)
      m_gps2->WriteNextGPSDatum(&data->gps2);

    return 0;
  }

  s32 RPFileWriterCollection::Cleanup(const RPFileHeader *header)
  {
    if(!m_writers.empty()) {
      for(std::map < RP_TYPE, RPFileWriter * >::iterator i = m_writers.begin(); i != m_writers.end(); i++) {
        i->second->Cleanup(header);
        delete i->second;
        i->second = NULL;
      }
    }

    RPFileHeader temp; memcpy(&temp, header, sizeof(RPFileHeader));
    if(m_gps) {
      temp.type = RPF_GPS;
      m_gps->Cleanup(&temp);
    }
    if(m_gps2) {
      temp.type = RPF_GPS2;
      m_gps2->Cleanup(&temp);
    }

    return 0;
  }
};