/*
 * Copyright (C)2007-2016 SightLine Applications Inc
 * SightLine Applications Library of signal, vision, and speech processing
 * http://www.sightlineapplications.com
 *------------------------------------------------------------------------*/

#pragma once

#include "SLAImage.h"

enum {
  SLA_PROFILE_NONE = 0,
  SLA_PROFILE_BASELINE,
  SLA_PROFILE_CONSTRAINED_BASELINE,
  SLA_PROFILE_EXTENDED,
  SLA_PROFILE_MAIN,
  SLA_PROFILE_HIGH
};

#define STATS_NAME_LENGTH 10

typedef struct {
  f32 TotalBitRate;
  f32 FrameRate;
  f32 VideoBitRate;
  f32 KlvBitRate;

  char Encapsulation[STATS_NAME_LENGTH];
  char Codec[STATS_NAME_LENGTH];
  s32 Profile;

  u32 MinFrameBytes;
  u32 MaxFrameBytes;

  u32 KeyFrames;
  u32 IFrames, PFrames, BFrames, OtherFrames;
} CapStats;

/// Callback function type to be called when a frame is captured 
typedef bool (*SLCaptureCallback)(SLAImage *image, void *context, u32 capFlags);
typedef void (*SLKLVCallback)(struct KLVData *klv, struct KLVData *klvRecent, void *context);
typedef void (*SLStatsCallback)(CapStats *stats, void *context);
typedef void (*SLMtsPrivCallback)(const void *data, u32 dataSize, void *context, u64 pts);

class SLADecodeFFMPEG {
public:
  SLADecodeFFMPEG();
  virtual ~SLADecodeFFMPEG();

public:
  /*!
   *  Initialize a camera.
   *  @return SLA_SUCCESS for success, SLA_FAIL for failure
   *  @see Cleanup
   */
  SLStatus Initialize(
       const char *dirName,             //!< Name of directory to read files from
       SLA_IMAGE_TYPE outType,          //!< Type of image frames to output
       SLCaptureCallback callBack=0,    //!< Function pointer to call when capture is complete
       void *context=0,                 //!< Pointer to data structure to pass to callback
       s32 nLoop=1,                     //!< Number of times to loop on the input files (-1 forever)
       s32 startFrame=0,                //!< Starting frame index (-1 for beginning)
//       s32 doRGB=0,                   //!< 1 to return BGR images, 2 to return RGB images (default is YUV images)
       s32 noRelease=0,                 //!< not using release to indicate that image has been consumed. 
       
//       SLGetSharedImage getSharedImage=0, 
//       SLReleaseSharedImage releaseSharedImage=0,
//       SLVBIDecodeCallback vbiDecodeCallback=0,
       SLMtsPrivCallback mtsPrivCallback=0,
       bool useSlDemux=false // Use SLUDPReceive demux instead of FFMPEG internal version. Decoding SLALIB diag data works better with SLUDPReceive. 
       );

  void SetPALResample(bool flag);

  /*!
   *  Clean up / destroy camera.
   *  @return none
   *  @see Initialize
   */
  virtual SLStatus Cleanup(
    );

  /*!
   *  Capture an image frame.
   *  @param image Pointer to a pre-allocated image buffer to capture into
   *  @return SLA_SUCCES if capture was successful, SLA_FAIL if no image was captured.
   *  @see Release
   */
  virtual SLStatus Get(
    SLAImage *image  //!< Image to capture into
    );
    
  /*!
   *  Release a captured image frame.
   *  @param image Pointer to a pre-allocated image buffer to capture into
   *  @return SLA_SUCCESS for success, SLA_FAIL for failure
   *  @see Get
   */
  virtual SLStatus Release(
    SLAImage *image  //!< Image to release
    );

  /*!
   *  Reset the video capture hardware
   *  @return SLA_SUCCESS for success, SLA_FAIL for failure
   *  @see Initialize
   */
  virtual SLStatus Reset(
    );

  virtual void SetKLVCallBack(SLKLVCallback callBack);
  virtual void SetStatsCallBack(SLStatsCallback callback, void *context);

  virtual void GetImageInfo(
       s16 *high,                  //!< Requested image height, NULL or *wide==0 for default, valid pointer returns high
       s16 *wide                  //!< Requested image width, NULL or *wide==0 for default, valid pointer returns wide
       );

  // Manage saving of input stream
  SLStatus StartSaving(const char *filename);
  SLStatus StopSaving();
  SLStatus Start();
  SLStatus Pause(bool pause);
  SLStatus Seek(double timeStamp);
  double GetDuration();

private:
  void *Data;
};

