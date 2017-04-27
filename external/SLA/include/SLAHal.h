/*
 * Copyright (C)2007-2016 SightLine Applications Inc
 * SightLine Applications Library of signal, vision, and speech processing
 * http://www.sightlineapplications.com
 *------------------------------------------------------------------------*/

#pragma once

#include "sltypes.h"
#include "slport.h"

#if defined(linux)
#include <unistd.h>
#include <pthread.h>
#endif

typedef void* SLA_Sem;
typedef void* SLA_Task;
typedef void* SLA_Mbx;
typedef unsigned int SLA_TaskId;

#if defined (_WIN32) || defined (linux)
  #define SEM_FOREVER 0xFFFFFFFF  // win32 = INFINITE?
  #define SL_FOREVER 0xFFFFFFFF  // win32 = INFINITE?
  #ifndef NDEBUG
    #define SL_FOREVER_DEBUG (1000*17)   // For debugging: can be used to detect dead-lock situation.
  #else
    #define SL_FOREVER_DEBUG SL_FOREVER  
  #endif
#else
  #error Not defined for this platform.
#endif

#if defined(_WIN32) // Structured Exception Handling (used to detect memory access violation, which is usually not detected by C++ try..except)
# ifndef _SL_NO_WIN_H_
#  include <winsock2.h>
#  include <windows.h>
# endif
#include <excpt.h>
#define SL_TRY      __try
#define SL_EXCEPT   __except(EXCEPTION_EXECUTE_HANDLER) 
#else
#define SL_TRY      if(sizeof(int)>0)
#define SL_EXCEPT   else 
#endif

enum SLTaskPriority {
#if defined(_WIN32)
# ifndef _SL_NO_WIN_H_
  SL_PRI_15 = THREAD_PRIORITY_TIME_CRITICAL,
  SL_PRI_8 = THREAD_PRIORITY_HIGHEST, // 7 & 8 are the same on Windows.
  SL_PRI_7 = THREAD_PRIORITY_HIGHEST,
  SL_PRI_6 = THREAD_PRIORITY_ABOVE_NORMAL,
  SL_PRI_5 = THREAD_PRIORITY_NORMAL,
  SL_PRI_4 = THREAD_PRIORITY_BELOW_NORMAL,
  SL_PRI_3 = THREAD_PRIORITY_LOWEST
# else
  SL_PRI_5 = 0,
  SL_PRI_6 = 1,
# endif
#elif defined(linux)
  SL_PRI_15 = 15,
  SL_PRI_8 = 8,
  SL_PRI_7 = 7,
  SL_PRI_6 = 6,
  SL_PRI_5 = 5, // Considered to be "normal priority"
  SL_PRI_4 = 4,
  SL_PRI_3 = 3
#endif
};

// Constants.
enum {
  SL_DEFAULT_STACK_SIZE  = (32 * 1024),
  SL_SEM_MAX_COUNT       = 100 // Absolute maximum count size, for detecting logic error. Make this bigger if needed.
};


#define SLAMemset memset
#define SLAMemcpy memcpy
#define SLACalloc(x) calloc(1,x)
#define SLAMalloc(x) malloc(x)
#define SLAFree(x) free(x)

void SLATrace (const char *str, ...);

/*!@file
 *  SLAHal.
 */

/*!@defgroup SLAHALGroup SlaHAL
 *  SLA Hardware/OS Abstraction Layer
 */
//@{

/**
 * @brief Create a semaphore
 *
 * Each time semaphore is acquired (SLASemPend), count is reduced by 1.  When
 * count reaches 0 system will block until a semphore is released (SLASemPost).
 *
 * @param count default value used to initialize semphore
 * @return Handle to a semaphore object
 */
SLA_Sem SLASemCreate( int count, const char * name = "" );

/**
 * @brief Destroy a semaphore
 *
 * @param sem handle to a semphore
 * @return None
 */
void SLASemDestroy( SLA_Sem sem );

/**
 * @brief Acquire semaphore
 * @param sem handle to a semphore
 * @param howlong amount of time (in milliseconds) to wait before timining out.
 * @see SLSemCreate, SLSemPost
 * @return true if semaphore was acquired, otherwise false
 */
bool  SLASemPend(SLA_Sem sem, u32 howlong);

/**
 * @brief
 * @param count
 */
void  SLASemPost(SLA_Sem sem);

/**
 * @brief (Linux only) Create a Thread as joinable or detached.
 * @param fn function to be exectuted by thread
 * @param stacksize Override the default stack size with this amount (in bytes)
 * @param name Human readable name of thread
 * @param data parameter
 * @param priority Thread priority level
 * @param joinable (Linux only) Creates thread as "joinable" (as opposed to detached). Care should be taken for a joinable thread since it causes memory leak unless SLWaitFoThread gets called against the thread.
 * @return Handle to the thread.
 */
SLA_Task SLACreateThread(int (*fn)(void *), u32 stacksize, const char *name, void *data, SLTaskPriority priority = SL_PRI_5, bool joinable = false);

/**
 * @brief Wait for a thread to terminate. Calling thread will be suspended. (Linux only)Thread must be created as joinable.
 * @return Exit status of the thread
 */
int SLAWaitForThread( SLA_Task tsk );

void SLADestroyThread( SLA_Task handle );



/**
 * @brief Hold off thread for time specified.
 * @param ms amount of time to sleep (in milliseconds)
 */
void SLASleep(u32 ms);

/**
 * @brief Create a default mailbox
 * @param size Size (in bytes) of default mailbox entry
 * @param length Maximum number of items in the mailbox
 * @return handle to the mailbox
 */
SLA_Mbx SLAMbxCreate(u32 size, u32 length, const char * name = "");

/**
 * @brief Destroy the mailbox
 *
 * Release any resources allocated during create.  Sets mbx to NULL
 * on success.
 *
 * @param mbx Handle to mailbox created with SLMbxCreate
 * @see SLMbxCreate
 */
void SLAMbxDestroy(SLA_Mbx mbx);

/**
 * @brief Pop a message from the mailbox
 * @param mbx handle to the mailbox
 * @param msg handle to message that came off mailbox
 * @param timeout amount of time to wait for semaphore
 * @return true if sucessfully popped message from queue
 */
bool SLAMbxPend(SLA_Mbx mbx, void* msg, u32 timeout);

/**
 * @brief Push a message onto the mailbox
 * @param mbx handle to the mailbox
 * @param msg
 * @param timeout
 * @param typeClass a field user can use (not supported by SYSBIOS)
 * @return true if sucessfully posted message onto queue
 */
bool SLAMbxPost(SLA_Mbx mbx, void* msg, u32 timeout, u32 typeClass = 0);

/**
 * @brief Scan a mailbox for messages matching the application-defined type class (not supportred by SYSBIOS)
 * @param mbx handle to the mailbox
 * @param typeClass application-defined non-zero type class to match
 * @return number of matching messages found
 */
s32 SLAMbxCount(SLA_Mbx mbx, u32 typeClass);

void SLAGetMHzTime(u64 *time);


///////////////// Communications //////////////////////

/*! Contains socket releated information
 */
struct SLASocket {
  SOCKET socket;    //!< Actual socket handle
  u32 addr;         //!< address of...
  u16 port;         //!< Port number of open socket
  u32 sndrAddr;     //!< Address of client (where applicable)

  SLASocket() : socket(0), addr(0), port(0), sndrAddr(0) {
  };
};


#define MAC_STR_FORMAT    "%02X:%02X:%02X:%02X:%02X:%02X"
#define STR_FORMAT_IPADDR "%d.%d.%d.%d"

// Per-thread socket intialization
s32 SLASockStartup(void *hTsk=NULL);
void SLASockCleanup();

void SLASockClose(SLASocket *sock);
u32 SLASockError();

/*!
 * In systems with multiple ethernet adapters ("mulitple-homed" or "multi-homed"),
 * you may specify the adapter to use by providing the ip address of that adapter (srcAddr).
 * If srcAddr is NULL, then INADDR_ANY is used allowing the stack to select the 
 * correct adapter.
 *
 * @param sock 
 * @param sockType {SOCK_DGRAM, SOCK_STREAM, SOCK_RAW?}
 * @param protocol {IPPROTO_UDP, IPPROTO_TCP, IPPROTO_RAW? }
 * @param do_listen
 * @param srcAddr Specify the IP address of the network interface card
 * @return
 */
s32 SLASockServerBind(SLASocket *sock, s32 socktype, s32 protocol, u32 do_listen, const char *srcAddr=NULL);
void SLASockDisconnect(SLASocket *sock);

s32 SLASockSetTimeoutMs(SLASocket *sock, s32 name, u32 timeoutms);

s32 SLASockUDPInit(SLASocket *sock, u32 addr, u16 port);
s32 SLASockUDPInit(SLASocket *sock, const char *addr, u16 port);
s32 SLASockRecvFrom(SLASocket *sock, char *buf, s32 len, s32 timeoutms = -1);

/*!
 * @param grpaddr The address of the IPv4 multicast group (IP address where packets are sent).
 * @param srcaddr The address of the IPv4 multicast source (device that sends packets).
 * @param iaddr The local IPv4 address of the interface on which the multicast group should be joined.
 * IP_ADD_MEMBERSHIP option must be called for each local interface over which the multicast datagrams are to be received.
 * @return 0 on success
 */
s32 SLASockJoinSourceGroup(SLASocket *sock, u32 grpaddr, u32 srcaddr, u32 iaddr = INADDR_ANY);
s32 SLASockLeaveSourceGroup(SLASocket *sock, u32 grpaddr, u32 srcaddr, u32 iaddr);

/*!
 * sends explicity to the the address field supplied by sock.
 * @param sock a socket that is already open
 * @param buf the buffer to write
 * @param len number of characters from the buffer to write
 * @return number of characters sent
 */
s32 SLASockSendTo(SLASocket *sock, char *buf, s32 len);

s32 SLASockGetHostInfo(char *name, u32 namesize, 
                      char *mac, u32 macsize, 
                      char *ipaddr, u32 ipaddrsize, 
                      char *netmaskstr = 0, u32 netmasksize = 0, u32 *pnetmask = 0, char * iName = 0);

class SLASockUDP : public SLPort 
{
public:
  s32 Connected;
  SLASocket ReadSock;
  SLASocket WriteSock;

  SLASockUDP();
  ~SLASockUDP();
  s32 IsConnected() const { return Connected; }

  virtual SLStatus Initialize(const char *addr, u32 writePortNumber, u32 readPortNumber, bool broadcast=false);
  virtual SLStatus Initialize(u32 addr, u32 writePortNumber, u32 readPortNumber, bool broadcast=false);

  virtual s32 Read(
    void *data,   //!< Pointer to binary data to read
    u32 len,      //!< Length of data to read in bytes
    s32 timeoutMs //!< Time out in milliseconds
    );

  virtual s32 Write(
    const void *data,  //!< Pointer to binary data to write
    u32 len            //!< Length of data to write in bytes
    );

  virtual s32 Close();
};

typedef enum {
  SLA_UART_ENABLE_CTS = 0x01,
  SLA_UART_ENABLE_RTS = 0x02,
  SLA_UART_ENABLE_DTR = 0x04
} SLA_HANDSHAKE_LINES;

typedef enum {
  SLA_UART_PARITY_NONE = 0,
  SLA_UART_PARITY_ODD = 1,
  SLA_UART_PARITY_EVEN  = 2
} SLA_UART_PARITY;

class SLARs232 : public SLPort {
private:

public:
  SLARs232();
  
  virtual ~SLARs232() {
    Close();
  }
  
  /*!
   * @param flags @see SLA_HANDSHAKE_LINES
   * @return 0 if successful.
   */
  virtual s32 Open(u32 portNumber, u32 baud, u32 dataBits=8, u32 stopBits=1, u32 parity=SLA_UART_PARITY_NONE, u32 flags=0);
  
  /*!
   * @return 0 if successful.
   */
  virtual s32 Open(const char * portNumberStr, u32 baud, u32 dataBits=8, u32 stopBits=1, u32 parity=SLA_UART_PARITY_NONE, u32 flags=0);

  virtual s32 Read(void *data, u32 dataLen, s32 timeoutms);
#ifdef _WIN32
  virtual s32 ClearPending();
  virtual s32 EnableReadWaiting(bool enable);
#endif
#ifdef linux
  virtual s32 Read(void *data, u32 dataLen, s32 timeoutms, s32 initialTimeoutMs);
  virtual bool IsOpen() {
    return (handle >= 0) ? true : false;
  }
#else
  virtual bool IsOpen() {
    return (handle != 0) ? true : false;   // todo: is this correct?
  }
#endif
  virtual s32 Write(const void *data, u32 dataLen);
  virtual s32 Close();

protected:
  u32 LookupBaudRate( u32 baud );
  u32 LookupDateBits( u32 data );
  u32 LookupParity( u32 parity );
  u32 LookupStopBits( u32 stopBits );
  u32 port;
#ifdef linux
  fd_set com_set;
  s32 handle;
#else
  void *handle;
  bool enableReadWaiting;
#endif
};


//@}

