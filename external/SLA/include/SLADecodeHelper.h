///////////////////////////////////////////////////////////////////////////////
// SLALIB - diagnostic support public inerface
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <stddef.h>
#include "sltypes.h"
#include "SLAImage.h"


// forward declarations
struct SLAStabTrackArgs;
struct SLAStabTrackResult;
struct SLADiagArgs;

////////////////////////////////////////////////////////////////////////////////

/** Class to decode SLA diagnostic data
*/
class SLADecodeHelper {
public:
  SLADecodeHelper();
  ~SLADecodeHelper();

  /** Mode values used in Open()
  */
  enum DECODE {
    DECODE_IMG  = 0x01,
    DECODE_KLV  = 0x02,
    DECODE_DIAG = 0x04
  };
  enum FLAGS {
    FLAGS_DEBUG = 0x01
  };

  /** 
  * Open a MPEG-TS file
  * CAUTION: as of 20140716, the only decode value is (DECODE_IMG | DECODE_DIAG)
  * @param fname MPEG-TS file name.
  * @param decode specifies what to decode from the file, must be bit ORed DECODE. 
  * @param flags bit ORed FLAGS.
  */
  SLStatus Open(const char *fname, s32 decode, s32 flags = 0);

  /**
  * Close the file
  */
  void Close();

  /**
  * Get an image and/or data from a frmae. Subsequent call will return data from the next frame.
  * Please note img and res are the results of stab&track using args as input.
  * @param img Image from the frmae is copied over to img if not null. img must be a valied image and the pixel buffer (y) must be large enough. img dimention may be change.
  * @param imgBufSize size of the buffer pointer by img->y, or 0 (then no buffer size check will be performed)
  * @param args if not null, then it will be fileed with arguments for the frame.
  * @param res if not null, then it will be fileed with the result for the frmae.
  * @param darg if not null, then it will be fileed with the additional parameters for the frmae.
  * @return 
  *   SLA_SUCCESS img and args are filled.
  *   SLA_TERMINATE there is no more frame
  *   SLA_FAIL error
  */
  SLStatus NextFrame(SLAImage *img, u32 imgBufSize, SLAStabTrackArgs *args, SLAStabTrackResult *res=0, SLADiagArgs *darg=0);

  /**
  * struct to be filled by GetStatistics.
  */
  struct Statistics {
    u32 frameCnt;
    u32 imgCnt;
    u32 diagCnt;
  };

  /**
  * Get statistics, return how many images, or data have been decoded.
  */
  void GetStatistics(Statistics *dghs);

protected:
  void *data;

};


////////////////////////////////////////////////////////////////////////////////

