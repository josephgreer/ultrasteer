#include "NeedleFitter.h"

using namespace std;
using namespace alglib;

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

NeedleFitter::NeedleFitter(s32 polyDegree, f32 timeDecay, f32 minYouth, f32 imageWeight, f32 modelWeight, f32 youthWeight, f32 modelThresh)
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

NeedleFitter::~NeedleFitter()
{
}


void NeedleFitter::SetInitialModel(const PolyCurve *src)
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

void NeedleFitter::AddNeedleFrame(NeedleFrame &frame)
{
  PolyCurve last = m_lastModel;
  fitPointsToNeedle(&m_lastModel, &m_data.pParams, m_usedPoints, m_usedNPoints, m_pts, frame, &last);
  m_data.pParams.degree = nPointsToDegree(m_maxDegree, (s32)m_usedPoints.size());
}

PolyCurve NeedleFitter::GetModel()
{
  return m_lastModel;
}

void NeedleFitter::ClearPoints()
{
  m_pts.clear();
}

std::vector < NeedlePoint > * NeedleFitter::GetPoints()
{
  return &m_pts;
}

std::vector < Vec3f > * NeedleFitter::GetUsedPoints()
{
  return &m_usedPoints;
}


std::vector < NeedlePoint > NeedleFitter::GetUsedNeedlePoints()
{
#ifndef NO_THREAD
  ASSERT(0);
  //NOT THREAD SAFE TO RETURN A POINTER
#endif
  return m_usedNPoints;

}