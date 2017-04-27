/*
 * Copyright (C)2007-2016 SightLine Applications Inc
 * SightLine Applications Library of signal, vision, and speech processing
 * http://www.sightlineapplications.com
 *------------------------------------------------------------------------*/

#pragma once

#include "sltypes.h"
#define SLA_IMAGE_DEFINED 

/*!@file
 *  SLAImage.
 */

/*!@defgroup SLAImageGroup SlaImage
 *  SLA image structure.
 */
//@{


/// YUV color value
typedef struct SLAYuv_s {
  u8 y;  //!< y color value
  u8 u;  //!< u color value
  u8 v;  //!< v color value
} SLAYuv;

/// Image type specifier for YUV (luma / chroma) images
typedef enum {
  SLA_IMAGE_UNKNOWN = 0,
  SLA_IMAGE_YUV_422 = 1,      //!< Planar, uv is 2X downsampled in row
  SLA_IMAGE_YUV_420 = 2,      //!< Planar, uv is 2X downsampled in row and column
  SLA_IMAGE_UYVY    = 3,      //!< Interleaved pixels in the order U, Y, V, Y
  SLA_IMAGE_YUYV    = 4,      //!< Interleaved pixels in the order Y, U, Y, V
  SLA_IMAGE_G8      = 5,      //!< 8 bit grey scale image in Y plane only
  SLA_IMAGE_G16     = 6,      //!< 16 bit grey scale image in Y plane only
  SLA_IMAGE_NV12    = 7,      //!< YUV 420 semiplanar (Y plane, combined UV plane)
  SLA_IMAGE_BGGR    = 15,     //!< Internal use only
  SLA_IMAGE_RGGB    = 17,     //!< Internal use only
  SLA_IMAGE_C24_PACKED = 32,  //!< 8 bits each of R, G, B packed together
  SLA_IMAGE_C32_PACKED = 33   //!< 8 bits each of R, G, B, A, packed together
} SLA_IMAGE_TYPE;

/// A 32bit aligned image structure
typedef struct SLAImage {
  SLA_IMAGE_TYPE type; //!< Indicates the order of the pixel data
  u32 yhigh;    //!< Image height in pixels
  u32 ywide;    //!< Image width in pixels
  u32 ystride;  //!< Width in bytes of the image buffer
  u8 *y;        //!< Image luma buffer
  u32 uvhigh;   //!< Image height in pixels
  u32 uvwide;   //!< Image width in pixels
  u32 uvstride; //!< Width in bytes of the image buffer
  u8 *uv[2];    //!< Image chroma buffers
} SLAImage;

/*! Set up an SLAImage structure
 * @return SLA_SUCCESS if object was initialized
 */
SLStatus SLASetupImage(
      SLAImage *image,     //!< Resulting color scale image structure
      SLA_IMAGE_TYPE type, //!< UYVY or YUYV or YUV_420 or YUV_422.
      s32 high,            //!< Height of the image in pixels
      s32 wide,            //!< Width of the image in pixels
      s32 stride,          //!< Pixels from one row to the next
      u8 *pxl              //!< Pointer to the pixel buffer
      );

/*! Set up an SLAImage structure
 * @return SLA_SUCCESS if object was initialized
 */
SLStatus SLASetupImage(
      SLAImage *image,     //!< Resulting color scale image structure
      SLA_IMAGE_TYPE type, //!< UYVY or YUYV or YUV_420 or YUV_422.
      s32 high,            //!< Height of the image in pixels
      s32 wide,            //!< Width of the image in pixels
      s32 ystride,         //!< Luma pixels from one row to the next
      s32 uvstride,        //!< Luma pixels from one row to the next
      u8 *y,               //!< Pointer to the luma pixel buffer
      u8 *u,               //!< Pointer to the u chroma pixel buffer
      u8 *v                //!< Pointer to the v chroma pixel buffer
      );
//@}

