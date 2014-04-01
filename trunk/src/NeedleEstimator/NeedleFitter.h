#pragma once

#include <vector>
#include <deque>

//alglib
#include "ap.h"
#include "interpolation.h"


#include "NeedleEstimatorCore.h"

#define INITIAL_MODEL_POINTS 30

typedef struct {
  Vec3f point;
  Vec2f imagePoint;
  f32 imageScore;       //image analysis score.  assumed to be roughly correlated with probability of being a needle point based on image analysis alone
  f32 dResp;            //Dopler response
  f32 bResp;            //B-mode response
  f32 cDist;            //Distance from b-mode response to doppler centroid
  f32 modelScore;       //time history of how well point has been described by models in the past.
  f32 youth;            //for lack of a better term.  1 = brand new, 0 = infinitely old
  u8 used;
} NeedlePoint;

typedef struct {
  std::vector < NeedlePoint > pts;
} NeedleSeg;

typedef struct {
  std::vector < NeedleSeg > segments;
  u8 used;
} NeedleFrame;

//#define USE_BARYCENTRIC

#define MAX_POLY_DEGREE 10
typedef struct {
#ifdef USE_BARYCENTRIC
  alglib::barycentricinterpolant x;
  alglib::barycentricinterpolant y;
  alglib::barycentricinterpolant z;
#else
  f32 coefX[MAX_POLY_DEGREE+1];
  f32 coefY[MAX_POLY_DEGREE+1];
  f32 coefZ[MAX_POLY_DEGREE+1];
#endif

  s32 degree;
  f32 rmsError;
  Vec3f descriptor;
  Vec2f dRange;
} PolyCurve;

typedef struct {
  u8 enforceFrameConsistency;                   //do we choose one needle segment per frame?  
  u8 ransacTrialN;                              //number of probe inliers for RANSAC
} RansacParams;

#define INIT_MODEL_THRESH_FACTOR 2.0f

typedef struct {
  u8 min_points;
  s32 degree;
  f32 imageWeight;
  f32 modelWeight;
  f32 youthWeight;
  f32 timeDecay;                                //needle point youth decays as youth*timeDecay^(num_steps)
  f32 min_youth;                                //if needle point youth < min_youth remove from list'
  Vec3f descriptor;                             //descriptor axis
  f32 modelThresh;
  f32 minModelThresh;
} PolyCurveParams;

typedef struct {
  PolyCurve res;                                //polynomial curve representing our needle
  PolyCurveParams pParams;                      //weighted polynomial curve paramters

  std::vector < Vec3f > usedPoints;
  std::vector < NeedlePoint > usedNpoints;
  std::vector < NeedleFrame > incomingPts;      //new points to add to our list of points to fit to
} NeedleData; 


Vec3f EvaluatePoly(const PolyCurve *src, f32 t);

class NeedleFitter
{
protected:
  NeedleData m_data;
  PolyCurve m_lastModel;
  std::vector < NeedlePoint > m_pts;
  std::vector < Vec3f > m_usedPoints;
  std::vector < NeedlePoint > m_usedNPoints;
  s32 m_maxDegree;

public:
  NeedleFitter(s32 polyDegree, f32 timeDecay, f32 minYouth, f32 imageWeight, f32 modelWeight, f32 youthWeight, f32 modelThresh);
  ~NeedleFitter();
  void AddNeedleFrame(NeedleFrame &frame);
  PolyCurve GetModel();
  void SetInitialModel(const PolyCurve *src);
  void ClearPoints();
  std::vector < NeedlePoint > * GetPoints();
  std::vector < Vec3f > * GetUsedPoints();
  std::vector < NeedlePoint > GetUsedNeedlePoints();
};