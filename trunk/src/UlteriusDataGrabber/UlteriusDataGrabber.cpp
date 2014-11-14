#include <conio.h>
#include <stdio.h>
#include "UlteriusInterface.h"
#include "RPFileReader.h"
#include "RPUlterius.h"
#include "RPFileWriter.h"

using namespace Nf;

static void printUsage()
{
  printf("UlteriusDataGrabber.exe -ip xxx.xxx.xxx.xxx -n name -t typeMask\n");
}

//This is a CLIENT process that reads data from Ulterius and transfers it to 
//the SERVER process (which consumes the data) using named pipes and events.  Normally, one process would be 
//used with multiple threads.  However, Ulterius has a bug in it that makes it so that
//the only way to read multiple data types is to create a process per data type.

//#define SPOOF
int main( int argc, const char* argv[] )
{
  if(argc < 7) {
    printUsage();
    return 0;
  }

  char ip[100] = {0};
  strcpy(ip, argv[2]);

  char name[100];  
  strcpy(name, argv[4]);

  s32 type = atoi(argv[6]);

#ifndef SPOOF
  RPUlteriusReader reader(ip, (RP_TYPE)type);
#endif

  uDataDesc desc = reader.GetDesc(RPF_BPOST32);

  //bool wait = true;
  //while(wait)
  //  int x = 0;

  //Three events are needed for synchronization:
  //1.  A WRITE event which is signaled by the client (this process).  This tells the server that
  //data is ready to be consumed.  
  //2.  A READ event which is signaled by the server.  This tells the client (this process)
  //that data is done being consumed
  //3.  A PROCESS_DONE event which is signaled by the server.  This tells the client that
  //it should shut down. 

  char eName[100];
  sprintf(eName, "%s.write", name);
  HANDLE hWriteEvent = OpenEventA(EVENT_ALL_ACCESS, FALSE, eName);
  if(!hWriteEvent) {
    printf("Failed to open writeEvent.\n");
    return -1;
  }

  sprintf(eName, "%s.read", name);
  HANDLE hReadEvent = OpenEventA(EVENT_ALL_ACCESS, FALSE, eName);
  if(!hWriteEvent) {
    printf("Failed to open ReadEvent.\n");
    return -1;
  }

  sprintf(eName, "%s.done", name);
  HANDLE hDoneEvent = OpenEventA(EVENT_ALL_ACCESS, FALSE, eName);
  if(!hWriteEvent) {
    printf("Failed to open DoneEvent.\n");
    return -1;
  }

  sprintf(eName, "%\\\\.\\pipe\\%s", name);
  HANDLE hPipe = CreateFileA(eName, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
  if(!hPipe) {
    printf("Failed to get access to pipe.\n");
    return -1;
  }

  
  //u32 _mode = PIPE_TYPE_BYTE; 
  //BOOL fSuccess = SetNamedPipeHandleState(hPipe, (LPDWORD)&_mode, NULL, NULL);    
  //if (!fSuccess) 
  //{
  //  printf("Failed to set pipe mode %d\n", GetLastError());
  //  return -1;
  //}


  u32 bufSz = desc.h*desc.w*4+sizeof(RPDatumHeader);
  u8 *mem = (u8 *)calloc(bufSz+sizeof(RPDatumHeader), 1);
  if(!mem) {
    printf("Error allocating memory\n");
    return -1;
  }
  bool run = true;

#ifdef SPOOF
  s32 byteCount = 0;
  if(type == RPF_COLOR || type == RPF_BPOST32)
    byteCount = 640*480*4;
  else if(type == RPF_BPOST8)
    byteCount = 640*480;
  else 
    byteCount = sizeof(GPS_Data);
#endif

  while(run) {
    //Get next datum from ulterius
#ifndef SPOOF
    RPDatum data = reader.GetNextRPDatum();
    if(data.header.nBytes <= 0)
      continue;
#else
    Sleep(80);
    RPDatum data;
    data.header.type = type;
    data.header.tick = GetTickCount();
    data.header.nBytes = byteCount;
    data.data = (u8 *)malloc(byteCount);

    u8 yep[] = {0xd, 0xe, 0xa, 0xd, 0xb, 0xe, 0xe, 0xf};
    for(s32 i=0; i<byteCount/sizeof(yep); i++) {
      memcpy(data.data+i*sizeof(yep), yep, sizeof(yep));
    }
#endif

    if(sizeof(RPDatumHeader)+data.header.nBytes > bufSz) {
      printf("UlteriusDataGrabber:  Pipe buffer size too small bufferSize: %d, currentSz: %d\n", 
        bufSz, sizeof(RPDatumHeader)+data.header.nBytes);
      assert(0);
    }
    memcpy(mem, &data.header, sizeof(RPDatumHeader));
    memcpy(mem+sizeof(RPDatumHeader), data.data, data.header.nBytes);
    s32 nBytes = sizeof(RPDatumHeader)+data.header.nBytes;
    u32 bytesWritten;
    WriteFile(hPipe, mem, nBytes, (LPDWORD)&bytesWritten, NULL);
    if(bytesWritten != nBytes) {
      printf("Failed to write data\n");
      free(data.data);
      continue;
    }

    switch(SignalObjectAndWait(hWriteEvent, hReadEvent, 0xFFFFFFFF, FALSE)) {
      case WAIT_OBJECT_0:
        break;
      default:
        NTrace("Read failed\n");
        break;
    }
    free(data.data);
    ResetEvent(hReadEvent);

    //Should we die?
    u32 rv = WaitForSingleObject(hReadEvent, 0);
    switch(rv) {
      case WAIT_OBJECT_0:
      case WAIT_ABANDONED:
      case WAIT_FAILED:
        run = false;
        break;
      case WAIT_TIMEOUT:
        run = true;
        break;
    }
  }

  free(mem);

  return 0;
}