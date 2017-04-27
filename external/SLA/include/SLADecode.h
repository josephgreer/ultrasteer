/*
 * Copyright (C)2007-2016 SightLine Applications Inc
 * SightLine Applications Library of signal, vision, and speech processing
 * http://www.sightlineapplications.com
 *------------------------------------------------------------------------*/

#pragma once

#include "SLAImage.h"
#include "SLAKlv.h"

enum SLA_PROFILE {
  NONE = 0,
  BASELINE,
  CONSTRAINED_BASELINE,
  EXTENDED,
  MAIN,
  HIGH
};

#define CAP_STATS_NAME_LENGTH 10
typedef struct {
  float TotalBitRate;	// average total kilobits per second
  float FrameRate;		// frames per second
  float VideoBitRate;	// average video kbps
  float KlvBitRate;		// average klv kbps

  char Encapsulation[CAP_STATS_NAME_LENGTH];
  char Codec[CAP_STATS_NAME_LENGTH];
  SLA_PROFILE Profile;

  unsigned int MinFrameBytes;
  unsigned int MaxFrameBytes;

  u32 KeyFrames;
  u32 IFrames, PFrames, BFrames, OtherFrames;
} SLCapStats;

/*!
*  Function type called back when a decoded frame is available
*/
typedef void (*SLADecodeCB) (
  void *context,      //!< User-defined context
  SLAImage *image,    //!< Pointer to image description.  Could be NULL
  KLVData *klv,       //!< Most current (including values unchanged on this cycle) data values
  KLVData *klvRecent  //!< Pointer to only recently-changed data values
  );


/*!
*  Function type called back at approximately 1 Hz to provide stream statistics
*/
typedef void (*SLAStatsCB) ( SLCapStats *stats, void *context );

/// Class to decode MPEG2-TS or RTP-MJPEG video streams
class SLADecode {

public:
  /*!
  *  Initialize decoder, listening to specified stream
  *  @return 0 for success, -1 for failure
  */
  int Create(
    const char *UDPAddress,  //!< Address string, ie: udp://@224.10.10.1:1234
    SLADecodeCB cb,          //!< Callback function invoked when a frame is decoded
    SLAStatsCB statsCB,      //!< Callback function for statistics of frame rate, bitstream etc.
    void *cbContext,         //!< User-supplied pointer passed to callback function
    bool rgba = false        //!< If true return SLAImage that is SLA_IMAGE_C32_PACKED format instead of SLA_IMAGE_C24_PACKED
    );

  /*!
  *  Initialize decoder, listening to specified stream
  *  @return 0 for success, -1 for failure
  */
  int Create(
    const char *UDPAddress,  //!< Address string, ie: udp://@224.10.10.1:1234
    SLADecodeCB cb,          //!< Callback function invoked when a frame is decoded
    void *cbContext          //!< User-supplied pointer passed to callback function
    );

  /*!
  *  Shut down the decoder and release all resources
  *  @return 0 for success, -1 for failure
  */
  int Destroy();

  /*!
  *  Change the stream that the decoder is listenting to
  *  @return 0 for success, -1 for failure
  */
  int SetAddress(
    const char *UDPAddress   //!< Address string, ie: udp://@224.10.10.1:1234
    );

  /*!
  *  Set special frame size resampling to compensate for non-square pixels
  *  When frame size is 720x576 and flag is true, resample to 768x576
  *  @return 0 for success, -1 for failure
  */
  int SetPALResampling(
    bool resample   //!< true to resample, false to not
    );


  /*!
  *  Begin saving decoded video/metadata stream to specified filename
  *  No format translation is performed.  e.g.  "downlink.ts" or "c:\temp\downlink.ts"
  *  @return 0 for success, -1 for failure
  */
  int StartSaving(
    const char *filename     //!< Filename of destination file
    );

  /*!
  *  Stop saving video to the file previously specified.  StopSaving should be
  *  called before program termination to ensure file is properly closed and data is
  *  flushed.
  *  @return 0 for success, -1 for failure
  */
  int StopSaving();

  /*!
  *	pause the stream at the last decoded frame.
  * Dont know what will happen for a network stream
  *	@retrn 0 for success, -1 for failure.
  */
  int Pause(bool enable);

  /*!
  *	seek the stream keyframe at the neatest timestamp
  * Dont know what will happen for a network stream
  * @params[in] timeStamp - time in seconds from the start of the file to seek to
  *	@return 0 for success, -1 for failure.
  */
  int Seek(double timeStamp);

  /*!
  * Returns the duration of the recording in seconds
  * what does it do for the network
  */
  double GetDuration();

  /*!
  *  Static helper function to take a raw block of data, and decode the KLV elements
  *  into a KLVData element.
  *  @return 0 for success, -1 for failure
  */
  static s32 BufferToKLVData(KLVData *klv, const u8* buf, u16 len, s32 bufStartOffset = 0);
private:
  void *Data;
};


