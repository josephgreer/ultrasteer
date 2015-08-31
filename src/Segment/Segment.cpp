#include <vnl/vnl_cross.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_quaternion.h>
#define VNL_INCLUDE
#include "Segment.h"

using namespace std;
using namespace alglib;

namespace Nf
{
  static void makeFlat(std::vector < NeedlePoint > &flat, const std::vector < NeedleFrame > &collection)
  {
    flat.clear();

    for(s32 i=0; i<(s32)collection.size(); i++) {
      for(s32 j=0; j<(s32)collection[i].segments.size(); j++) {
        for(s32 k=0; k<(s32)collection[i].segments[j].pts.size(); k++) {
          flat.push_back(collection[i].segments[j].pts[k]);
        }
      }
    }
  }

  static void makeFlat(std::vector < NeedlePoint > &flat, const NeedleFrame &collection)
  {
    flat.clear();

    for(s32 j=0; j<(s32)collection.segments.size(); j++) {
      for(s32 k=0; k<(s32)collection.segments[j].pts.size(); k++) {
        flat.push_back(collection.segments[j].pts[k]);
      }
    }
  }

  static void makeEverythingUnused(std::vector < NeedleFrame > &collection)
  {
    for(s32 i=0; i<(s32)collection.size(); i++) {
      collection[i].used = 0;
      for(s32 j=0; j<(s32)collection[i].segments.size(); j++) {
        for(s32 k=0; k<(s32)collection[i].segments[j].pts.size(); k++) {
          collection[i].segments[j].pts[k].used = 0;
        }
      }
    }
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////
  //ransac stuff
  static s32 numPoints(const std::vector < NeedleFrame > &frames, u8 enforceFrameConsistency=0)
  {
    s32 rv = 0;
    if(!enforceFrameConsistency) {
      for(s32 i=0; i<(s32)frames.size(); i++) {
        for(s32 j=0; j<(s32)frames[i].segments.size(); j++) {
          rv += (s32)frames[i].segments[j].pts.size();
        }
      }
    } else {
      //if we enforce point consistency, number of points per frame = 
      for(s32 i=0; i<(s32)frames.size(); i++) {
        s32 minSize = 1000000;
        for(s32 j=0; j<(s32)frames[i].segments.size(); j++) {
          minSize = min((s32)frames[i].segments[j].pts.size(), minSize);
        }
        rv += minSize;
      }
    }
    return rv;
  }

  static std::vector < NeedlePoint > selectRandomPoints(std::vector < NeedleFrame > &frames, s32 nPoints, u8 enforceFrameConsistency)
  {
    //TODO:  Do we want to choose points according to score distribution?

    if(enforceFrameConsistency) {
      for(s32 i=0; i<(s32)frames.size(); i++) {
        frames[i].used = 0;
      }

      std::vector < NeedlePoint > rv;
      while((s32)rv.size() < nPoints) {
        s32 frameIdx;
        do { 
          frameIdx = rand() % (s32)frames.size();
        } while(frames[frameIdx].used);

        frames[frameIdx].used = 1;

        s32 segIdx = rand() % (s32)frames[frameIdx].segments.size();
        NeedleSeg seg = frames[frameIdx].segments[segIdx];
        for(s32 i=0; i<(s32)seg.pts.size(); i++) {
          rv.push_back(seg.pts[i]);
        }
      }
      return rv;
    } else {
      std::vector < NeedlePoint > rv;
      std::vector < NeedlePoint > flat;
      makeEverythingUnused(frames);
      makeFlat(flat, frames);

      while((s32)rv.size() < nPoints) {
        s32 id;
        do {
          id = rand() % (s32)flat.size();
        } while(flat[id].used);
        rv.push_back(flat[id]);
      }

      return rv;
    }
  }

  Vec3f EvaluatePoly(const PolyCurve *src, f32 t)
  {
    Vec3f res(0,0,0);
    ASSERT(src->rmsError < 200);
#ifdef USE_BARYCENTRIC
    f64 x = alglib::barycentriccalc(src->x, (f64)t);
    f64 y = alglib::barycentriccalc(src->y, (f64)t);
    f64 z = alglib::barycentriccalc(src->z, (f64)t);
    res.x = (f32)x;  res.y = (f32)y;  res.z = (f32)z;

#else
    f32 _t = 1;
    for(s32 i=0; i<=src->degree; i++) {
      res += Vec3f(src->coefX[i]*_t, src->coefY[i]*_t, src->coefZ[i]*_t);
      _t = t*_t;
    }
#endif
    return res;
  }

  Vec2f GenerateRange(const real_1d_array &t)
  {
    Vec2f range(1000000, -1000000);
    for(s32 i=0; i<(s32)t.length(); i++) {
      range.x = (f32)MIN(t[i], range.x);
      range.y = (f32)MAX(t[i], range.y);
    }
    return range;
  }


  // End ransac stuff
  ////////////////////////////////////////////////////////////////////////////////////////////////

  static void generateArrays(std::vector < Vec3f > & usedPoints,  std::vector < NeedlePoint > & usedNPoints, real_1d_array &t, real_1d_array &x, real_1d_array &y, real_1d_array &z, real_1d_array &w, 
    const std::vector < NeedlePoint > &flat, const PolyCurveParams *params, const PolyCurve *src)
  {
    usedPoints.clear();
    usedNPoints.clear();

    std::vector < f32 > ts;
    std::vector < f32 > ws;
    f32 mThreshSq = params->modelThresh*params->modelThresh;
    for(s32 i=0; i<(s32)flat.size(); i++) {
      f32 _t = flat[i].point.dot(params->descriptor);
      f32 distSq = 1;
      if(src) {
        Vec3f opt = EvaluatePoly(src, _t);
        distSq = opt.distanceSquared(flat[i].point);
        if(distSq > mThreshSq ) {
          continue;
        }
      }
      ts.push_back(_t);
      ws.push_back(flat[i].imageScore/MAX(distSq,1));
      usedPoints.push_back(flat[i].point);
      usedNPoints.push_back(flat[i]);
    }

    t.setlength(usedPoints.size());
    x.setlength(usedPoints.size());
    y.setlength(usedPoints.size());
    z.setlength(usedPoints.size());
    w.setlength(usedPoints.size());
    for(s32 i=0; i<(s32)usedPoints.size(); i++) {
      x[i] = usedPoints[i].x;
      y[i] = usedPoints[i].y;
      z[i] = usedPoints[i].z;

      t[i] = ts[i];
      w[i] = ws[i];//(params->imageWeight*flat[i].imageScore + params->modelWeight/(flat[i].modelScore))*flat[i].youth;
    }
    if(usedPoints.size() > 0 && usedPoints.size() != flat.size())
      int x = 0;
    if(false) {
      for(s32 i=0; i<(s32)usedPoints.size(); i++) {
        NTrace("(%f %f %f %f %f)\n", usedPoints[i].x, usedPoints[i].y, usedPoints[i].z, ts[i], ws[i]);
      }
      NTrace("===============================================================\n");
      for(s32 i=0; i<x.length(); i++) {
        NTrace("(%f %f %f %f %f)\n", x[i], y[i], z[i], t[i], w[i]);
      }
    }
    ASSERT(t.length() == x.length() && x.length() == y.length() && y.length() == z.length() && z.length() == usedPoints.size());
  }

  static s32 fitWeightedPolyCurve(PolyCurve *out, std::vector < Vec3f > &usedPoints, std::vector < NeedlePoint > & usedNPoints, std::vector < NeedlePoint > & flat, const PolyCurveParams *params, const PolyCurve *src = NULL)
  {
    real_1d_array t,x,y,z,w;

    generateArrays(usedPoints, usedNPoints, t,x, y, z, w, flat, params, src);

    if(t.length() < params->min_points)
      return -1;

    ae_int_t info;
    barycentricinterpolant px, py, pz;
    polynomialfitreport reportX,reportY,reportZ;

    real_1d_array dummy1, dummy2;
    integer_1d_array dummy3;
    ae_int_t m = params->degree+1;
    ae_int_t n = t.length();
    ae_int_t k = 0;

    polynomialfitwc(t, x, w, n, dummy1, dummy2, dummy3, k, m, info, px, reportX);
    polynomialfitwc(t, y, w, n, dummy1, dummy2, dummy3, k, m, info, py, reportY);
    polynomialfitwc(t, z, w, n, dummy1, dummy2, dummy3, k, m, info, pz, reportZ);

#ifdef USE_BARYCENTRIC
    out->x = px;
    out->y = py;
    out->z = pz;
#else
    real_1d_array resX,resY,resZ;
    polynomialbar2pow(px, resX);
    polynomialbar2pow(py, resY);
    polynomialbar2pow(pz, resZ);

    memset(out->coefX, 0, sizeof(f32)*(MAX_POLY_DEGREE+1));
    memset(out->coefY, 0, sizeof(f32)*(MAX_POLY_DEGREE+1));
    memset(out->coefZ, 0, sizeof(f32)*(MAX_POLY_DEGREE+1));

    for(s32 i=0; i<=params->degree; i++) {
      out->coefX[i] = (f32)resX[i];
      out->coefY[i] = (f32)resY[i];
      out->coefZ[i] = (f32)resZ[i];
    }
#endif
    out->degree = params->degree;
    out->descriptor = params->descriptor;
    out->rmsError = (f32)sqrt((reportX.rmserror*reportX.rmserror+reportY.rmserror*reportY.rmserror+
      reportZ.rmserror*reportZ.rmserror));
    out->dRange = GenerateRange(t);

    return (s32)info;

  }

  static s32 fitPolyCurve(PolyCurve *out, std::vector < Vec3f > &usedPoints, std::vector < NeedlePoint > & usedNPoints, std::vector < NeedlePoint > & flat, const PolyCurveParams *params, const PolyCurve *src = NULL)
  {
    real_1d_array t,x,y,z,w;

    generateArrays(usedPoints, usedNPoints, t,x, y, z, w, flat, params, src);

    if(t.length() < params->min_points)
      return -1;

    ae_int_t info;
    barycentricinterpolant px, py, pz;
    polynomialfitreport reportX,reportY,reportZ;

    ae_int_t m = params->degree+1;
    ae_int_t n = t.length();
    ae_int_t k = 0;

    polynomialfit(t, x, n, m, info, px, reportX);
    polynomialfit(t, y, n, m, info, py, reportY);
    polynomialfit(t, z, n, m, info, pz, reportZ);

#ifdef USE_BARYCENTRIC
    out->x = px;
    out->y = py;
    out->z = pz;
#else
    real_1d_array resX,resY,resZ;
    polynomialbar2pow(px, resX);
    polynomialbar2pow(py, resY);
    polynomialbar2pow(pz, resZ);

    memset(out->coefX, 0, sizeof(f32)*(MAX_POLY_DEGREE+1));
    memset(out->coefY, 0, sizeof(f32)*(MAX_POLY_DEGREE+1));
    memset(out->coefZ, 0, sizeof(f32)*(MAX_POLY_DEGREE+1));

    for(s32 i=0; i<=params->degree; i++) {
      out->coefX[i] = (f32)resX[i];
      out->coefY[i] = (f32)resY[i];
      out->coefZ[i] = (f32)resZ[i];
    }
#endif
    out->degree = params->degree;
    out->descriptor = params->descriptor;
    out->rmsError = (f32)((reportX.rmserror*reportX.rmserror+reportY.rmserror*reportY.rmserror+
      reportZ.rmserror*reportZ.rmserror));
    out->dRange = GenerateRange(t);


    return (s32)info;

  }

  static void fitPointsToNeedle(PolyCurve *dst, PolyCurveParams *pParams, std::vector < Vec3f > &usedPoints,  std::vector < NeedlePoint > &usedNPoints, std::vector < NeedlePoint > &pts, const NeedleFrame & incoming, const PolyCurve *src = NULL)
  {
    usedPoints.clear();
    usedNPoints.clear();
#ifndef USE_BARYCENTRIC
    memset(dst, 0, sizeof(PolyCurve));
#endif
    dst->rmsError = 100000;
    std::vector < NeedlePoint > flat;
    makeFlat(flat, incoming);

    for(s32 i=0; i<(s32)flat.size(); i++) {
      pts.push_back(flat[i]);
    }

    //fit to curve
    if((s32)pts.size() < pParams->min_points) {
      return;
    }

    f32 rms = (f32)fitPolyCurve(dst, usedPoints, usedNPoints, pts, pParams, src->rmsError < 200 ? src : NULL); //fitWeightedPolyCurve(&temp, usedPoints, pts, pParams, src);

    //Update model thresh based on how many points we have linearly vary from MAX_MODEL_THRESH TO MIN_MODEL_THRESH 
    //based on clamped value between 20 and 75 of number of points 
    f32 nUsedPoints = (f32)usedPoints.size();
    nUsedPoints = MAX(MIN(nUsedPoints,75),20);
    f32 maxModelThresh = pParams->minModelThresh*INIT_MODEL_THRESH_FACTOR;
    pParams->modelThresh = maxModelThresh-(nUsedPoints-20.0f)/(75.0f-20.0f)*(maxModelThresh-pParams->minModelThresh);


    //upate scores decay youth and remove pts below threshhold
    while(pts.size() > 5000) {
      pts.pop_back();
    }

  }

  //////////////////////////////////////////////////
  //Begin Curve Fitter 
  //////////////////////////////////////////////////
  CurveFitter::CurveFitter(s32 polyDegree, f32 timeDecay, f32 minYouth, f32 imageWeight, f32 modelWeight, f32 youthWeight, f32 modelThresh)
  {
    m_data.pParams.min_points = INITIAL_MODEL_POINTS;
    m_data.pParams.degree = 1;
    m_data.pParams.imageWeight = imageWeight;
    m_data.pParams.modelWeight = modelWeight;
    m_data.pParams.youthWeight = youthWeight;
    m_data.pParams.timeDecay = timeDecay;
    m_data.pParams.min_youth = minYouth;
    m_data.pParams.descriptor = Vec3f(1.0f, 0, 0);

    m_data.pParams.modelThresh = modelThresh*INIT_MODEL_THRESH_FACTOR;
    m_data.pParams.minModelThresh = modelThresh;

    m_maxDegree = polyDegree;

    m_data.incomingPts.clear();
  }

  CurveFitter::~CurveFitter()
  {
  }


  void CurveFitter::SetInitialModel(const PolyCurve *src)
  {
    m_lastModel = *src;
  }

  static s32 nPointsToDegree(s32 maxDegree, s32 nPoints)
  {
    s32 res = 0;
    if(nPoints < 40)
      res = MIN(1, maxDegree);
    else if(nPoints < 80)
      res = MIN(2, maxDegree);
    else
      res = maxDegree;
    return res;
  }

  void CurveFitter::AddNeedleFrame(NeedleFrame &frame)
  {
    PolyCurve last = m_lastModel;
    fitPointsToNeedle(&m_lastModel, &m_data.pParams, m_usedPoints, m_usedNPoints, m_pts, frame, &last);
    m_data.pParams.degree = nPointsToDegree(m_maxDegree, (s32)m_usedPoints.size());
  }

  PolyCurve CurveFitter::GetModel()
  {
    return m_lastModel;
  }

  void CurveFitter::ClearPoints()
  {
    m_pts.clear();
  }

  std::vector < NeedlePoint > * CurveFitter::GetPoints()
  {
    return &m_pts;
  }

  std::vector < Vec3f > * CurveFitter::GetUsedPoints()
  {
    return &m_usedPoints;
  }


  std::vector < NeedlePoint > CurveFitter::GetUsedNeedlePoints()
  {
    return m_usedNPoints;
  }

  //////////////////////////////////////////////////
  //End Curve Fitter 
  //////////////////////////////////////////////////

  //////////////////////////////////////////////////
  //Begin static image processing helper functions
  //////////////////////////////////////////////////
  static CvRect SquareiToCvRect(const Squarei &sq)
  {
    return cvRect(sq.ul.x, sq.ul.y, sq.lr.x-sq.ul.x, sq.lr.y-sq.ul.y);
  }

  static Squarei CvRectToSquarei(const CvRect &rect)
  {
    return Squarei(Vec2i(rect.x, rect.y), Vec2i(rect.x+rect.width, rect.y+rect.height));
  }


  static CvRect enlargeRect(CvRect rect, f32 factor)
  {
    f32 centerX = rect.x+rect.width/2.0f;
    f32 centerY = rect.y+rect.height/2.0f;
    f32 width = rect.width*factor;
    f32 height = rect.height*factor;
    return cvRect((s32)(centerX-width/2.0f+0.5f), (s32)(centerY-height/2.0f+0.5f),
      (s32)(width+0.5f), (s32)(height+0.5f));
  }

  static CvRect shrinkRect(CvRect rect, f32 factor)
  {
    f32 centerX = rect.x+rect.width/2.0f;
    f32 centerY = rect.y+rect.height/2.0f;
    f32 width = rect.width*(1.0f/factor);
    f32 height = rect.height*(1.0f/factor);
    return cvRect((s32)(centerX-width/2.0f+0.5f), (s32)(centerY-height/2.0f+0.5f),
      (s32)(width+0.5f), (s32)(height+0.5f));
  }

  static void B82BGRA(const IplImage *src, IplImage *dst)
  {
    for(int y=0; y<src->height; y++) {
      u8 *s = (u8 *)src->imageData+src->widthStep*y;
      u32 *d = (u32 *)(dst->imageData+dst->widthStep*y);
      for(int x=0; x<src->width; x++) {
        d[x] = ((u32)s[x]<<0)|((u32)s[x]<<8)|((u32)s[x]<<16)|0xFF000000;
      }
    }
  }


  static Vec2f dopplerCentroid(const IplImage *im)
  {
    u32 sx, sum, rsum;
    sum = sx = 0;
    rsum = 0;
    u32 rcen = 0;
    for(s32 r=0; r<im->height; r++) {
      const u8 *psrc = (u8 *)im->imageData+r*im->widthStep;
      for(s32 c=0; c<im->width; c++) {
        sum += (u32)psrc[c];
        sx += (u32)psrc[c]*c;
      }
    }

    for(s32 r=0; r<im->height; r++) {
      const u8 *psrc = (u8 *)im->imageData+r*im->widthStep;
      for(s32 c=0; c<im->width; c++) {
        rsum += (u32)psrc[c];
      }
      if(rsum >= sum/4) {
        rcen = r;
        break;
      }
    }

    Vec2f res((f32)sx/(f32)sum, (f32)rcen);
    return res;
  }


  //Automatically find, expand, and cluster contours.  
  //A threshold is established for smallest contours.  If minArea is true, then this threshold is set
  //to 1/2 the area of the largest contour
  static std::vector < Squarei > findExpandAndClusterContours(IplImage *colorMask, f32 expand, bool minArea = false, bool shrinkBack = false)
  {
    CvMemStorage *storage = cvCreateMemStorage();
    CvSeq *cons;
    cvFindContours(colorMask, storage, &cons, sizeof(CvContour), CV_RETR_EXTERNAL);

    CvSeq *first = cons;

    std::vector < u8 > useRect;
    s32 maxArea = 0;
    s32 area;
    if(minArea) {
      for(; cons != 0; cons = cons->h_next) {
        area = CvRectToSquarei(cvBoundingRect(cons)).Area();
        if(area > maxArea)
          maxArea = area;
      }
      cons = first;
    }
    for(; cons != 0; cons = cons->h_next) {
      area = CvRectToSquarei(cvBoundingRect(cons)).Area();
      useRect.push_back(area < maxArea/4 ? 0 : 1);
    }
    cons = first;


    std::vector < Squarei > rects;
    for(s32 i=0; cons != 0; cons = cons->h_next, i++) {
      if(useRect[i])
        rects.push_back(CvRectToSquarei(enlargeRect(cvBoundingRect(cons), expand)));
    }

    bool changed = true;
    while(changed) {
      changed = false;
      for(std::vector < Squarei >::iterator i=rects.begin(); i!=rects.end(); i++) {
        std::vector < Squarei >::iterator j = i+1;
        while(j != rects.end()) {
          //TODO: Figure out parameter
          //combine rects if they're close
#if 1
          if(i->Dist(*j) < 3) {
#else
          if(i->Overlaps(*j)) {
#endif
            changed = true;
            *i += *j;
            j = rects.erase(j);
          } else {
            j++;
          }
        }
      }
    }
    if(shrinkBack) {
      for(s32 i=0; i<rects.size(); i++) {
        rects[i] = CvRectToSquarei(shrinkRect(SquareiToCvRect(rects[i]), expand));
      }
    }
    //remove spurious noise
    //std::vector < Squarei >::iterator i = rects.begin();
    //while(i != rects.end()) {
    //	if(i->Area() < minArea) {
    //		i = rects.erase(i);
    //	} else {
    //		i++;
    //	}
    //}

    cvReleaseMemStorage(&storage);
    return rects;

  }

  static u8 calcThresh(CvHistogram *hist, float frac)
  {
    cvNormalizeHist(hist, 1.0f);
    f32 total = 0;
    s32 i=255;
    for(; i>=0; i--){
      total += cvQueryHistValue_1D(hist, i);
      if(total >= frac)
        break;
    }
    return i;
  }

  typedef struct {
    f32 majorLen;
    f32 minorLen;
    f32 cenX;
    f32 cenY;
    f32 orientation;
  } Ellipse;

  static Ellipse estimateEllipseFromMoments(CvMoments *moments, Squarei rect)
  {
    Ellipse rv;

    f32 xc = (f32)(moments->m10/moments->m00);  f32 yc = (f32)(moments->m01/moments->m00);
    f32 a = (f32)((moments->m20/moments->m00)-(xc*xc));
    f32 b = (f32)(2*((moments->m11/moments->m00)-(xc*yc)));
    f32 c = (f32)((moments->m02/moments->m00)-(yc*yc));

    f32 principal = sqrtf((b*b)+(a-c)*(a-c));
    f32 major = sqrtf(6*(a+c+principal));
    f32 minor = sqrtf(6*(a+c-principal));
    f32 theta = fmodf(((atan2(b, a-c)/2.0f)*180.0f/PI)+360.0f, 360.0f);

    rv.majorLen = major*.65f;
    rv.minorLen = minor*.65f;
    rv.orientation = theta;
    rv.cenX = rect.ul.x+xc;
    rv.cenY = rect.ul.y+yc;
    return rv;
  }

  static std::vector < s32 > makeCorrespondences(const std::vector < Squarei > &a, std::vector < Squarei > &b)
  {
    std::vector < s32 > res(a.size(), -1);

    for(s32 i=0; i < (s32)a.size(); i++) {
      Squarei as = a[i];
      for(s32 j=0; j<(s32)b.size(); j++) {
        if(as.Dist(b[j]) == 0) {
          if(res[i] >= 0) {
            s32 x = 0;
            s32 y = 1;
            //x = y/x;
          }
          res[i] = j;
        }
      }
    }

    return res;
  }

#define N_BEST 3
#define DOPPLER_SUM_SCALE 100.0
#define BMODE_SUM_SCALE 100.0
#define BMODE_DOPPLER_COMBINE_SCALE 1000.0 //final score computed as DOPPLER_SUM/DOPPLER_SUM_SCALE+

  static void chooseNBestBmodeROIs(std::vector < Squarei > & bRects, f32 bSums[], f32 bDists[], Vec2f centroid, IplImage *analyzeImage)
  {
    if(bRects.size() > 0) {
      //Find top N_BEST bmode candidates based on:
      //1) Distance from Doppler Centroid
      //2) Bmode respone
      s32 best[N_BEST];  for(s32 i=0; i<N_BEST; i++) best[i] = -1;
      f32 bestDist[N_BEST];  for(s32 i=0; i<N_BEST; i++) bestDist[i] = 100000.0f;
      for(s32 i=0; i<(s32)bRects.size(); i++) {
        if(((Vec2f)(bRects[i])).y < 10)
          continue;
        Vec2f delta = Vec2f((f32)bRects[i].DistX((s32)centroid.x), (f32)bRects[i].DistY((s32)centroid.y));
        f32 dist = delta.magnitudeSquared();

        cvSetImageROI(analyzeImage, SquareiToCvRect(bRects[i]));
        CvScalar sum = cvSum(analyzeImage);
        sum.val[0] /= BMODE_SUM_SCALE;
        cvResetImageROI(analyzeImage);


        f32 tDist = dist/(f32)sum.val[0];
        for(s32 j=0; j<N_BEST; j++) {
          if(tDist < bestDist[j]) {
            for(s32 k=N_BEST-2; k>=j; k--) {
              bSums[k+1] = bSums[k];
              bDists[k+1] = bDists[k];
              bestDist[k+1] = bestDist[k];
              best[k+1] = best[k];
            }
            bSums[j] = (f32)sum.val[0];
            bDists[j] = dist;
            bestDist[j] = tDist;
            best[j] = i;
            break;
          }
        }

      }

      std::vector < Squarei > tempRects = bRects;
      bRects.clear();
      for(s32 i=0; i<N_BEST; i++) {
        if(best[i] >= 0) {
          Squarei bestRect = tempRects[best[i]];
          bRects.push_back(bestRect);
        }
      }
    }
  }

  //////////////////////////////////////////////////
  //End static image processing helper functions
  //////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////////////////
  //Begin Needle Estimator
  ////////////////////////////////////////////////////////////////////////
  NeedleSegmenter::NeedleSegmenter(s32 width, s32 height, Updateable *update)
    : m_mainModelInit(false)
    , m_update(update)
    , ParameterCollection("2D US Segmentation")
    , m_model(3, .99f/*timeDecay*/, .001f/*minYouth*/, 1.0f/3.0f/*imageWeight*/, 1.0f/3.0f/*modelWeight*/, 1.0f/3.0f/*timeWeight*/, 1.5f)
    , m_initialModel(2, .99f/*timeDecay*/, .001f/*minYouth*/, 1.0f/3.0f/*imageWeight*/, 1.0f/3.0f/*modelWeight*/, 1.0f/3.0f/*timeWeight*/, 1000000.0f)
  {
    ADD_BOOL_PARAMETER(m_showColorMask, "Show Mask", CALLBACK_POINTER(onParamChange, NeedleSegmenter), this, false);
    ADD_FLOAT_PARAMETER(m_threshFrac, "Threshold Fraction", CALLBACK_POINTER(onParamChange, NeedleSegmenter), this, .02, 0.0, 1.0, 0.01);
    ADD_FLOAT_PARAMETER(m_dopplerClusterExpand, "Doppler Cluster Expansion", CALLBACK_POINTER(onParamChange, NeedleSegmenter), this, 2.0f, 0.0f, 100.0f, 0.1f);
    ADD_FLOAT_PARAMETER(m_bmodeClusterExpand, "Bmode Cluster Expansion", CALLBACK_POINTER(onParamChange, NeedleSegmenter), this, 1.1f, 0.0f, 100.0f, 0.1f);
    ADD_INT_PARAMETER(m_initialModelPoints, "Initial Model Points", CALLBACK_POINTER(onParamChange, NeedleSegmenter), this, 30, 1, 100, 1);
    ADD_ENUM_PARAMETER(m_displayMode, "Display Type", CALLBACK_POINTER(onSetDisplayMode, NeedleSegmenter), this, QtEnums::DisplayModality::DM_BPOST32, "DisplayModality");

    if(width > 0 && height > 0) {
      for(s32 i=0; i<2; i++) 
        m_colorMask[i] = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);

      m_disImage = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 4);
    } else {
      for(s32 i=0; i<2; i++) 
        m_colorMask[i] = NULL;

      m_disImage = NULL;
    }


    InitZeroLut();
  }

  NeedleSegmenter::~NeedleSegmenter()
  {
    if(m_colorMask[0])
      cvReleaseImage(&m_colorMask[0]);
    if(m_colorMask[1])
      cvReleaseImage(&m_colorMask[1]);
    if(m_disImage)
      cvReleaseImage(&m_disImage);
  }

  void NeedleSegmenter::Initialize(s32 width, s32 height)
  {
    for(s32 i=0; i<2; i++) {
      if(m_colorMask[i] != NULL)
        cvReleaseImage(&m_colorMask[i]);
      m_colorMask[i] = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
    }

    if(m_disImage != NULL)
      cvReleaseImage(&m_disImage);
    m_disImage = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 4);

  }

  void NeedleSegmenter::onSetDisplayMode()
  {
    if(m_displayMode->GetValue() != QtEnums::DM_BPOST8 && m_displayMode->GetValue() != QtEnums::DM_BPOST32)
      m_displayMode->SetValue(QtEnums::DM_BPOST32);

    onParamChange();
  }

  void NeedleSegmenter::onParamChange()
  {
    if(m_update)
      m_update->onUpdate();
  }

  bool NeedleSegmenter::IsInit()
  {
    return m_colorMask[0] != NULL && m_colorMask[1] != NULL && m_disImage != NULL;
  }


  void NeedleSegmenter::InitZeroLut()
  {
    memset(&m_zeroLut[0], 1, sizeof(m_zeroLut));
    m_zeroLut[256] = 0;

    m_zeroLutBase = m_zeroLut+256;
  }

  //Takes Doppler Image and returns grayscale image with intensity given by color intensity
  void NeedleSegmenter::MaskColor(IplImage *dst, const IplImage *src)
  {
    ASSERT(dst->nChannels == 1);
    ASSERT(src->nChannels == 4);

    u8 k;
    //bgr
    s32 weights[3] = {18, 183, 54};
    for(s32 y=0; y<src->height; y++) {
      u8 *p = (u8 *)src->imageData+src->widthStep*y;
      u8 *d = (u8 *)dst->imageData+dst->widthStep*y;
      for(s32 x=0; x<src->width; x++) {
        k = (*(m_zeroLutBase+(u32)(p[0]-p[1])))|(*(m_zeroLutBase+(u32)(p[0]-p[2])))|(*(m_zeroLutBase+(u32)(p[1]-p[2])));
        d[x] = k*(u8)((weights[0]*p[0]+weights[1]*p[1]+weights[2]*p[2]+127)>>8);
        p += src->nChannels;
      }
    }
  }

  void NeedleSegmenter::ThresholdBmodeROIs(const std::vector < Squarei > rects[], IplImage *colorMask, IplImage *bmode)
  {
    //Now find a threshold value for each region in the b-mode data.
    cvZero(colorMask);

    std::vector < u8 > thresholds((s32)rects[0].size());
    s32 nBins = 256;
    CvHistogram *hist = cvCreateHist(1,&nBins, CV_HIST_ARRAY); 
    for(s32 i=0; i < (s32)rects[0].size(); i++) {
      cvSetImageROI(colorMask, SquareiToCvRect(rects[0][i]));
      cvSetImageROI(bmode, SquareiToCvRect(rects[0][i]));
      cvCalcHist(&bmode, hist, 0);
      thresholds[i] = calcThresh(hist, m_threshFrac->GetValue());
      cvInRangeS(bmode, cvScalar(thresholds[i]), cvScalar(255), colorMask);
      cvResetImageROI(colorMask);
      cvResetImageROI(bmode);
    }
    cvReleaseHist(&hist);
    if(m_showColorMask->GetValue())
      B82BGRA(colorMask, m_disImage);
  }



#define N_BEST 3
#define DOPPLER_SUM_SCALE 100.0
#define BMODE_SUM_SCALE 100.0
#define BMODE_DOPPLER_COMBINE_SCALE 1000.0 //final score computed as DOPPLER_SUM/DOPPLER_SUM_SCALE+

  void NeedleSegmenter::ProcessColor(const IplImage *color, IplImage *bmode, const ImageCoordTransform *transform)
  {
    m_dopplerCentroid.segments.clear();
    m_frame.segments.clear();

    //Mask color
    MaskColor(m_colorMask[0], color);

    //Filter out noise less than 7
    cvInRangeS(m_colorMask[0], cvScalar(7), cvScalar(255), m_colorMask[1]);

    if(m_displayMode->GetValue() == QtEnums::DM_BPOST8 && !m_showColorMask->GetValue())
      B82BGRA(bmode, m_disImage);
    else if(m_displayMode->GetValue() == QtEnums::DM_BPOST32 && !m_showColorMask->GetValue())
      cvCopyImage(color, m_disImage);
    else
      B82BGRA(m_colorMask[0], m_disImage);

    if(false) {
      cvSaveImage("C:/Joey/test.bmp", m_disImage);
    }

    //Compute Doppler Centroid.  We need this for:
    // 1) Initial model
    // 2) Scoring of b-mode candidates
    CvMoments moment;
    cvMoments(m_colorMask[0], &moment);
    if(moment.m00 == 0)
      return;

    Vec2f centroidComp = Vec2f((f32)(moment.m10/moment.m00), (f32)(moment.m01/moment.m00));
    Vec2f centroid = dopplerCentroid(m_colorMask[0]);//Vec2f((f32)(moment.m10/moment.m00), (f32)(moment.m01/moment.m00));
    CvScalar dopplerSum = cvScalar(moment.m00);

    //Store off doppler centroid for initial model
    NeedlePoint npt;
    NeedleSeg segment;
    npt.imagePoint = centroid;
    npt.point = transform->Transform(centroid);
    npt.imageScore = (f32)(dopplerSum.val[0]/DOPPLER_SUM_SCALE);
    segment.pts.push_back(npt);
    m_dopplerCentroid.segments.clear();
    m_dopplerCentroid.segments.push_back(segment);

    if(false) {
      cvSaveImage("C:/test.bmp", bmode);
    }


    //find Expand and Cluster Contour Rects based on Color Doppler
    // this gives a first stage idea of where the needle is
    std::vector < Squarei > rects[2];
    rects[0] = findExpandAndClusterContours(m_colorMask[1], m_dopplerClusterExpand->GetValue(), true, false);

    //big rect is used for later scoring
    Squarei bigRect(Vec2i(0,0), Vec2i(0,0), 0);
    if(rects[0].size() > 0) {
      bigRect = rects[0][0];
      for(s32 i=1; i<(s32)rects[0].size(); i++) {
        bigRect += rects[0][i];
      }
    }


    if(false) {
      cvCopyImage(color, m_disImage);
      for(s32 i=0; i<rects[0].size(); i++) {
        cvDrawRect(m_disImage, cvPoint(rects[0][i].ul.x, rects[0][i].ul.y), cvPoint(rects[0][i].lr.x, rects[0][i].lr.y),
          cvScalar(0, 255, 0));
      }
      cvSaveImage("C:/test.bmp", m_disImage);
    }


    f32 bDists[N_BEST] = {0};
    f32 bSums[N_BEST] = {0};

    IplImage *analyzeImage;
    std::vector < s32 > correspondences;
    if(bmode) {
      //Find "bright" regions in proximity of doppler patches
      ThresholdBmodeROIs(rects, m_colorMask[1], bmode);

      if(false) {
        cvSaveImage("C:/test.bmp", m_colorMask[1]);
      }

      //Now find, expand, and cluster contours in bmode thresholded image
      rects[1] = findExpandAndClusterContours(m_colorMask[1], m_bmodeClusterExpand->GetValue(), false, true);

      analyzeImage = bmode;

      //Choose N best bmode ROIs
      chooseNBestBmodeROIs(rects[1], bSums, bDists, centroid, analyzeImage);

      //match bmode ROIs to doppler ROIs
      //we do this because we need an idea of the doppler response
      //that spawns a particular bmode ROI
      correspondences = makeCorrespondences(rects[1], rects[0]);
    } else {
      analyzeImage = m_colorMask[1];
    }

    cvDrawCircle(m_disImage, cvPoint(npt.imagePoint.x, npt.imagePoint.y), 5, cvScalar(255, 255, 0), 2);

    CvRect rect;

    s32 rStage = 1; //rStage == 1 => bmode ROIs, rStage == 2 => doppler ROIs
    m_frame.segments.clear();

    //for each region of interest...
    for(s32 i=0; i<(s32)rects[rStage].size(); i++) {
      rect = SquareiToCvRect(rects[rStage][i]);
      //ignore ROIs that are too small 
      if(rect.width*rect.height < 8)
        continue;

      cvSetImageROI(analyzeImage, rect);
      cvMoments(analyzeImage, &moment);
      cvResetImageROI(analyzeImage);
      //this shouldn't happen, but just to be safe.
      if(!moment.m00)
        continue;

      Ellipse el = estimateEllipseFromMoments(&moment, rects[rStage][i]);
      el.cenX = (f32)(moment.m10/moment.m00)+rect.x;  el.cenY = (f32)(moment.m01/moment.m00)+rect.y;

      //cvDrawRect(m_disImage, cvPoint(rect.x, rect.y), cvPoint(rect.x+rect.width, rect.y+rect.height), cvScalar(0, 255, 0));
      cvEllipse(m_disImage, cvPoint((s32)(el.cenX+0.5f), (s32)(el.cenY+0.5f)), cvSize((s32)(el.majorLen+0.5f), (s32)(el.minorLen+1.5f)),
        el.orientation, 0, 360, cvScalar(0, 255, 0), 2);

      //calculate the sum of the ROI response 
      if(rStage == 1)
        cvSetImageROI(m_colorMask[0], SquareiToCvRect(rects[0][correspondences[i]]));
      else 
        cvSetImageROI(m_colorMask[0], SquareiToCvRect(rects[0][i]));
      CvScalar sum = cvSum(m_colorMask[0]);
      cvResetImageROI(m_colorMask[0]);

      NeedleSeg seg;
      NeedlePoint npt;

      //break up the ellipse along its major axis...
      f32 elDel = 40.0f;
      s32 nHops = (s32)((el.majorLen)/elDel);
      Vec2f axis(1.0f,tan(el.orientation*PI/180.0f));
      axis = axis.normalized();
      Vec2f startPoint = Vec2f(el.cenX, el.cenY)-axis*el.majorLen;
      Vec2f endPoint = Vec2f(el.cenX, el.cenY)+axis*el.majorLen;
      //cvDrawLine(m_disImage, cvPoint(startPoint.x, startPoint.y), cvPoint(endPoint.x, endPoint.y), cvScalar(0, 0, 255), 1);
      for(s32 hop=-nHops; hop<=nHops; hop++) {
        f32 xx = el.cenX+axis.x*hop*elDel;
        f32 yy = el.cenY+axis.y*hop*elDel;
        cvDrawCircle(m_disImage, cvPoint(xx, yy), 4, cvScalar(255, 0, 0), 2);

		    npt.point = transform->Transform(Vec2f(xx,yy));
        npt.imageScore = (f32)(sum.val[0]/DOPPLER_SUM_SCALE);
        npt.imagePoint = Vec2f(el.cenX, el.cenY);
        if(rStage == 1) {
          npt.imageScore = (npt.imageScore+bSums[i])/(bDists[i] > 1.0f ? bDists[i] : 1.0f);
          npt.cDist = bDists[i];
          npt.bResp = bSums[i];
          npt.dResp = (f32)dopplerSum.val[0]/(f32)((bigRect.ul.x - bigRect.lr.x + 1) * (bigRect.ul.y - bigRect.lr.y + 1));//(f32)(sum.val[0]/DOPPLER_SUM_SCALE);
          npt.dResp = npt.dResp*sqrtf((f32)((bigRect.ul.x - bigRect.lr.x + 1) * (bigRect.ul.y - bigRect.lr.y + 1)));
        }
        npt.used = 0;
        npt.youth = 1;
        npt.modelScore = 1;
        seg.pts.push_back(npt);
      }
      m_frame.segments.push_back(seg);
    }

    if(false) {
      cvSaveImage("C:/Joey/test.bmp", m_disImage);
    }

    m_frame.used = 0;
  }

  void NeedleSegmenter::GetSegmentationResults(NeedleFrame &bmode, NeedleFrame &doppler)
  {
    bmode = m_frame;
    doppler = m_dopplerCentroid;
  }

  IplImage *NeedleSegmenter::GetDisplayImage() const
  {
    return cvCloneImage(m_disImage);
  }

  IplImage * NeedleSegmenter::UpdateModel(PolyCurve *model, IplImage *doppler, IplImage *bmode, 
    const ImageCoordTransform *transform)
  {
	m_dopplerCentroid.segments.clear();
	m_frame.segments.clear();

    ProcessColor(doppler, bmode, transform);
    s32 initSize = (s32)m_initialModel.GetUsedPoints()->size();
    if(!m_mainModelInit) {
      m_initialModel.AddNeedleFrame(m_dopplerCentroid);
    }
    if(initSize >= m_initialModelPoints->GetValue() && !m_mainModelInit) {
      m_model.SetInitialModel(&m_initialModel.GetModel());
      m_mainModelInit = true;
    }
    m_model.AddNeedleFrame(m_frame);
    if(m_mainModelInit && m_model.GetUsedPoints()->size() < INITIAL_MODEL_POINTS) {
      m_mainModelInit = false;
      m_model.ClearPoints();
      m_initialModel.ClearPoints();
    }
    *model = m_model.GetModel();
    return m_disImage;
  }


  PolyCurve::PolyCurve() 
	  : degree(0)
  {
	  memset(coefX, 0, sizeof(f32)*(MAX_POLY_DEGREE+1));
	  memset(coefY, 0, sizeof(f32)*(MAX_POLY_DEGREE+1));
	  memset(coefZ, 0, sizeof(f32)*(MAX_POLY_DEGREE+1));
  }

  PolyCurve PolyCurve::Derivative() const
  {
	  PolyCurve res;
	  for(s32 i=MAX_POLY_DEGREE; i>0; i--) {
		  res.coefX[i-1] = i*this->coefX[i];
		  res.coefY[i-1] = i*this->coefY[i];
		  res.coefZ[i-1] = i*this->coefZ[i];
	  }

	  return res;
  }

  Vec3d PolyCurve::Evaluate(f32 t) const
  {
	  Vec3d res(0,0,0);
	  for(s32 i=MAX_POLY_DEGREE; i>=0; i--) {
		  f64 tPow = pow(t,i);
		  res += Vec3d(this->coefX[i]*tPow, this->coefY[i]*tPow, this->coefZ[i]*tPow);
	  }
	  return res;
  }


  vnl_vector < f64 > PolyCurveToNeedleTip(const PolyCurve *curve)
  {

	  // Find the first derivative at the tip, this is the z-axis of the tip frame
	  Vec3d z_axis = curve->Derivative().Evaluate(curve->dRange.y).normalized();

	  
	  // Find the second derivative at the tip, this is the y-axis and z-axis combined
	  Vec3d y_axis = curve->Derivative().Derivative().Evaluate(curve->dRange.y).normalized();

	  // Remove z-axis component
	  y_axis = y_axis-z_axis*z_axis.dot(y_axis);
	  y_axis = y_axis.normalized();

	  // Finish right-handed coordinate system
	  Vec3d x_axis = y_axis.cross(z_axis);
	  
	  //Now create rotation matrix from these basis vectors
	  vnl_matrix<f64> R(3,3);
 	  R.set_column(0,x_axis);
	  R.set_column(1,y_axis);
	  R.set_column(2,z_axis);


	  // Create quaternion and axis-angle representation
	  vnl_quaternion<f64> q(R.transpose());
	  vnl_vector<f64> r = q.axis();
	  r = r*q.angle();

	  vnl_vector<f64> p = curve->Evaluate(curve->dRange.y);

	  vnl_vector<f64> res(6);
	  // Format the final output
	  for( int i = 0; i < 3; i++)
	  {
		  res[i] = p[i];
		  res[i+3] = r[i];
	  }

	  return res;

  }
  ////////////////////////////////////////////////////////////////////////
  //End Needle Estimator
  ////////////////////////////////////////////////////////////////////////
}