#include "Estimator.h"
using namespace arma;


namespace Nf
{
  /// \brief		Constructor: Initializes filter	
  Estimator::Estimator(void)
  {
    Init =  false;
    OBS.clear();
    INS.clear();
    YAW.clear();
  }

  /// \brief		Destructor
  Estimator::~Estimator(void)
  {
  }

  void Estimator::setEstimator(float ins,float appoff,float yaw)
  {
    Init =  true;

    ROT_base = Matrix33d::I();
    off = Vec3d(0,0,appoff+ins);
    prev_l = ins;
    prev_th = yaw;
    tip_mm_t = ROT_base*off;
    ROT_t = ROT_base;
    rho = 85; //mm 

    TIP_t.clear();
    TIP_t.push_back(Matrix44d::FromOrientationAndTranslation(ROT_t, tip_mm_t));
    e_roll = e_pitch = e_yaw = 0;
  }
  void Estimator::resetEstimator(void)
  {
    Init =  false;
  }
  bool Estimator::isInitialized(void)
  {
    return Init;
  }
  Matrix44d Estimator::getCurrentEstimate()
  {
    return TIP_t.back();
  }
  void Estimator::addPOINT(Vec3d p)
  {
    OBS.push_back(p);
  }

  
  Matrix33d Estimator::rotx(double roll)
  {
     double ct;
     double st;

     ct=cos(roll);
     st=sin(roll);

     return Matrix33d(1,0,0,0,ct,-st,0,st,ct);
  }
  
  Matrix33d Estimator::roty(double pitch)
  {
     double ct;
     double st;

     ct=cos(pitch);
     st=sin(pitch);

     return Matrix33d(ct,0,st,0,1,0,-st,0,ct);
  }

  Matrix33d Estimator::rotz(double yaw)
  {
     double ct;
     double st;

     ct=cos(yaw);
     st=sin(yaw);

     return Matrix33d(ct,-st,0,st,ct,0,0,0,1);
  }


  void Estimator::updateInput(double m_l, double m_th)
  {
    double diff_l,diff_th;
    Vec3d app_d;

    diff_l = m_l - prev_l;
    diff_th = m_th - prev_th;
    
    // Gradi?
    if ((abs(diff_l)>0.0001)&&(abs(diff_l)>0.0001))
    {
       
       INS.push_back(diff_l);
       YAW.push_back(diff_th);

       app_d = Vec3d(0,rho*(1-cos(diff_l/rho)),rho*sin(diff_l/rho));

       tip_mm_t = tip_mm_t + ROT_t * rotz(diff_th*(PI/180)) * app_d;
       ROT_t = ROT_t * rotz(diff_th*(PI/180)) * rotx(-(diff_l/rho));

       TIP_t.push_back(Matrix44d::FromOrientationAndTranslation(ROT_t, tip_mm_t));

       prev_l = m_l;
       prev_th = m_th;
    }

  }

  void Estimator::saveDataOpt()
  {
    mat dyaw = zeros(1,YAW.size()); 
    mat dins = zeros(1,INS.size());
    mat point = zeros(3,OBS.size());
    mat R0 = zeros(3,3);
    mat offset = zeros(3,1);
    mat mrho = zeros(1,1);
    mrho(1,1)=rho;

    for (int i=0;i<YAW.size();i++)
    {
       dyaw(0,i) = YAW[i];
       dins(0,i) = INS[i];
    }

    Vec3d app;
    for (int i=0;i<OBS.size();i++)
    {
       app = OBS[i];
       point(0,i) = app.x;
       point(1,i) = app.y;
       point(2,i) = app.z;
    }

    offset(0,0) = off.x;
    offset(1,0) = off.y;
    offset(2,0) = off.z;


    for (int i=0;i<3;i++)
    {
        for (int j=0;j<3;j++)
        {
            R0(i,j)=ROT_base.m_data[i][j];
        }
    }


    char path[150] = {0};
    char *basePath = "C:/Joey/ultrasteer/src/MATLAB_CurveFit/NeedleSimulationsParticleFilter/ctests/data/"; 

    sprintf(path, "%sYAW.dat", basePath);
    dyaw.save(path, raw_ascii);
    sprintf(path, "%sINS.dat", basePath);
    dins.save(path, raw_ascii);
    sprintf(path, "%sPOINT.dat", basePath);
    point.save(path, raw_ascii);
    sprintf(path, "%sR0.dat", basePath);
    R0.save(path, raw_ascii);
    sprintf(path, "%sOFF.dat", basePath);
    offset.save(path, raw_ascii);
    sprintf(path, "%sRHO.dat", basePath);
    offset.save(path, raw_ascii);

  }
}