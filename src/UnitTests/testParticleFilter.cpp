#include "CppUnitLite/Test.h"
#include "RTCore.h"
#include "RPFileReader.h"
#include "RPUlterius.h"
#include "ParticleFilter.h"

#include <iostream>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <Windows.h>
#include <armadillo>

using namespace std;
using namespace arma;
using ::s32;
using namespace Nf;

TEST(ParticleFilter, PropagateNeedleTip)
{
  using ::s32;

  f64 Rin1[] = {-0.39641421, 0.80836579, 0.43520171,
    0.80850395, 0.53196766, -0.25165803,
    -0.43494498, 0.25210149, -0.86444647};

  f64 posin1[] = {-353.24410918,
    -407.66140523,
    -313.73978862};

  f64 rhoin1 = 50.11662863;

  f64 dt1 = 0.34556073;

  f64 dtheta1 = 2.49296356;

  f64 v1 = 26.94083670;

  f64 Rout1[] = {0.80423683, -0.47814165, 0.35296415,
    -0.32294834, -0.85016888, -0.41583318,
    0.49890630, 0.22043917, -0.83815218};

  f64 posout1[] = {-349.56473081,
    -410.77744067,
    -321.68799468};

  f64 rhoout1 = 50.11662863;

  f64 Rin2[] = {-0.38349697, 0.65088380, 0.65519490,
    0.90348913, 0.41144287, 0.12009228,
    -0.19140914, 0.63801650, -0.74585353};

  f64 posin2[] = {-472.61240680,
    170.46751018,
    -82.69519763};

  f64 rhoin2 = 78.90587182;

  f64 dt2 = 0.55868983;

  f64 dtheta2 = 0.88207715;

  f64 v2 = 9.90507445;

  f64 Rout2[] = {0.25879073, 0.66209382, 0.70332008,
    0.89187032, -0.44339992, 0.08924041,
    0.37093759, 0.60417571, -0.70524962};

  f64 posout2[] = {-468.85194655,
    171.04695708,
    -86.71194814};

  f64 rhoout2 = 78.90587182;

  f64 Rin3[] = {0.01146674, 0.93351110, -0.35836510,
    0.54643762, 0.29429202, 0.78409064,
    0.83742130, -0.20481514, -0.50673101};

  f64 posin3[] = {376.38915230,
    394.60666350,
    -414.95578863};

  f64 rhoin3 = 69.61613078;

  f64 dt3 = 0.03905478;

  f64 dtheta3 = 1.06707600;

  f64 v3 = 43.90712517;

  f64 Rout3[] = {0.82309721, 0.44924399, -0.34740584,
    0.52149725, -0.35572488, 0.77556459,
    0.22483683, -0.81953624, -0.52707566};

  f64 posout3[] = {375.78399960,
    395.94396646,
    -415.84221064};

  f64 rhoout3 = 69.61613078;

  f64 Rin4[] = {-0.96980168, 0.21985430, 0.10558779,
    -0.07083659, -0.66816605, 0.74063237,
    0.23338139, 0.71078704, 0.66356229};

  f64 posin4[] = {191.87711395,
    -184.48436899,
    186.50092768};

  f64 rhoin4 = 61.65826425;

  f64 dt4 = 0.83462567;

  f64 dtheta4 = 0.11490864;

  f64 v4 = 37.50721575;

  f64 Rout4[] = {-0.93819850, 0.23668823, 0.25251190,
    -0.14697864, -0.93301431, 0.32845331,
    0.31333825, 0.27104055, 0.91014074};

  f64 posout4[] = {197.60576465,
    -167.38181691,
    211.67602659};

  f64 rhoout4 = 61.65826425;

  f64 Rin5[] = {0.34502726, 0.92510862, -0.15852519,
    0.59296536, -0.08392034, 0.80084296,
    0.72756324, -0.37031260, -0.57751217};

  f64 posin5[] = {-396.77399342,
    -52.10647382,
    408.59550309};

  f64 rhoin5 = 74.46396642;

  f64 dt5 = 0.29361415;

  f64 dtheta5 = 1.80814578;

  f64 v5 = 6.50142861;

  f64 Rout5[] = {0.81804753, -0.54862743, -0.17264464,
    -0.22098990, -0.57695416, 0.78631251,
    -0.53100066, -0.60508829, -0.59321705};

  f64 posout5[] = {-397.09009767,
    -50.59152125,
    407.47803270};

  f64 rhoout5 = 74.46396642;

  f64 eps = 1e-4;

  //tests
  TipState t,o;
  NSCommand u;

  //test 1
  t.pos = vec3(posin1);
  t.R = (mat33)(mat33(Rin1).t());
  t.rho = rhoin1;

  u.dtheta = dtheta1;
  u.dutyCycle = 0;
  u.v = v1;

  o = t.Propagate(u, dt1, NULL);
  mat33 Rcomp1 = (mat33)mat33(Rout1).t();
  vec3 pcomp1 = vec3(posout1);

  Rcomp1.print("Rcomp1:  ");
  o.R.print("Rreal1:  ");

  pcomp1.print("poscomp1:  ");
  o.pos.print("posreal1:  ");


  assert(sum(sum(abs(o.R-Rcomp1))) < eps);
  assert(sum(abs(o.pos-pcomp1)) < eps);
  assert(abs(rhoout1-o.rho) < eps);

  
  //test 2
  t.pos = vec3(posin2);
  t.R = (mat33)(mat33(Rin2).t());
  t.rho = rhoin2;

  u.dtheta = dtheta2;
  u.dutyCycle = 0;
  u.v = v2;

  o = t.Propagate(u, dt2, NULL);
  mat33 Rcomp2 = (mat33)mat33(Rout2).t();
  vec3 pcomp2 = vec3(posout2);

  Rcomp2.print("Rcomp2:  ");
  o.R.print("Rreal2:  ");

  pcomp2.print("poscomp2:  ");
  o.pos.print("posreal2:  ");

  assert(sum(sum(abs(o.R-Rcomp2))) < eps);
  assert(sum(abs(o.pos-pcomp2)) < eps);
  assert(abs(rhoout2-o.rho) < eps);

  
  //test 3
  t.pos = vec3(posin3);
  t.R = (mat33)(mat33(Rin3).t());
  t.rho = rhoin3;

  u.dtheta = dtheta3;
  u.dutyCycle = 0;
  u.v = v3;

  o = t.Propagate(u, dt3, NULL);
  mat33 Rcomp3 = (mat33)mat33(Rout3).t();
  vec3 pcomp3 = vec3(posout3);

  Rcomp3.print("Rcomp3:  ");
  o.R.print("Rreal3:  ");

  pcomp3.print("poscomp3:  ");
  o.pos.print("posreal3:  ");

  assert(sum(sum(abs(o.R-Rcomp3))) < eps);
  assert(sum(abs(o.pos-pcomp3)) < eps);
  assert(abs(rhoout3-o.rho) < eps);

  
  //test 4
  t.pos = vec3(posin4);
  t.R = (mat33)(mat33(Rin4).t());
  t.rho = rhoin4;

  u.dtheta = dtheta4;
  u.dutyCycle = 0;
  u.v = v4;

  o = t.Propagate(u, dt4, NULL);
  mat33 Rcomp4 = (mat33)mat33(Rout4).t();
  vec3 pcomp4 = vec3(posout4);

  Rcomp4.print("Rcomp4:  ");
  o.R.print("Rreal4:  ");

  pcomp4.print("poscomp4:  ");
  o.pos.print("posreal4:  ");

  assert(sum(sum(abs(o.R-Rcomp4))) < eps);
  assert(sum(abs(o.pos-pcomp4)) < eps);
  assert(abs(rhoout4-o.rho) < eps);

  
  //test 5
  t.pos = vec3(posin5);
  t.R = (mat33)(mat33(Rin5).t());
  t.rho = rhoin5;

  u.dtheta = dtheta5;
  u.dutyCycle = 0;
  u.v = v5;

  o = t.Propagate(u, dt5, NULL);
  mat33 Rcomp5 = (mat33)mat33(Rout5).t();
  vec3 pcomp5 = vec3(posout5);

  Rcomp5.print("Rcomp5:  ");
  o.R.print("Rreal5:  ");

  pcomp5.print("poscomp5:  ");
  o.pos.print("posreal5:  ");

  assert(sum(sum(abs(o.R-Rcomp5))) < eps);
  assert(sum(abs(o.pos-pcomp5)) < eps);
  assert(abs(rhoout5-o.rho) < eps);
}

TEST(ParticleFilter, PropagateNeedleBack)
{
  using ::s32;

  f64 eps = 1e-4;

  char path[150] = "C:/Joey/ultrasteer/src/MATLAB_CurveFit/NeedleSimulationsParticleFilter/ctests/data";

  //load exported data from matlab
  mat vs, dthetas, pos, rhos, Rs;
  vs.load(std::string(path)+std::string("/propagateNeedleTipBackV.dat"));
  dthetas.load(std::string(path)+std::string("/propagateNeedleTipBackdThetas.dat"));
  pos.load(std::string(path)+std::string("/propagateNeedleTipBackPos.dat"));
  rhos.load(std::string(path)+std::string("/propagateNeedleTipBackRhos.dat"));
  Rs.load(std::string(path)+std::string("/propagateNeedleTipBackRs.dat"));

  std::vector < NSCommand > us;
  for(s32 i=0; i<vs.n_rows;i++) {
    NSCommand uc;
    uc.v = vs(i);
    uc.dtheta = dthetas(i);
    uc.dutyCycle = 0;

    us.push_back(uc);
  }

  TipState t;
  t.pos = vec3(pos.colptr(0));
  t.R = mat33(Rs.colptr(0));
  t.rho = rhos(0);

  vec dts = ones(pos.n_cols)*1.0/10.0;
  std::vector < TipState > xs = t.PropagateBack(us, dts, NULL);
  for(s32 i=0; i<vs.n_rows; i++) {
    char txt[50]; sprintf(txt, "R%d:  ", i);
    xs[i].R.print(txt);
    sprintf(txt, "pos%d:  ", i);
    xs[i].pos.print(txt);
    assert(sum(sum(abs(xs[i].R-mat33(Rs.colptr(i))))) < eps);
    assert(sum(abs(xs[i].pos-vec3(pos.colptr(i)))) < eps);
    assert(abs(xs[i].rho-rhos(i)) < eps);
  }
}

static void saveOrientations(const std::vector < mat33 > &ors, const char *path)
{
  using ::s32;
  mat res;    //res = [R1 R2 ... Rn] \in R^9xn, Ri = unrolled R \in R^9
  for(s32 i=0; i<ors.size(); i++) {
    mat curr = (mat)ors[i];
    res.insert_cols(i, reshape(curr, 9, 1));
  }
  res.save(path, raw_ascii);
}

std::vector < mat33 > loadOrientations(const char *path)
{
  using ::s32;
  std::vector < mat33 > res;
  mat l;
  bool rv = l.load(path);
  assert(rv);
  for(s32 i=0; i<l.n_cols; i++) {
    res.push_back(reshape(l.col(i), 3, 3));
  }
  return res;
}


TEST(ParticleFilter, PropagateParticles)
{
  using ::s32;

  f64 eps = 1e-4;

  char path[150] = "C:/Joey/ultrasteer/src/MATLAB_CurveFit/NeedleSimulationsParticleFilter/ctests/data";
  char fpath[150];

  f64 dt = 1/10.0;
  NSCommand u;
  u.dtheta = PI/3;
  u.v = 500;

  TipState t;
  t.pos << -98.8337136840801 << -176.622054591443 << -203.830702615601 << endr;
  t.rho = 53.7252042275534;
  t.R <<         -0.396414206629243   <<      0.808365786570174   <<      0.435201714019249 << endr
      <<          0.808503950509501   <<      0.531967664602427   <<     -0.251658033505728 << endr
      <<         -0.434944983639354   <<      0.25210148535902    <<     -0.864446471614488 << endr;

  

  std::vector < TipState > hist; 
  hist.push_back(t);

  s32 nParticles = 200;

  // First use full state particle filter
  PFFullStateParams pfs;
  ParticleFilterFullState pffs(nParticles, &pfs);
  pffs.InitializeParticleFilter(hist, &pfs);

  std::vector < mat33 > Rs = pffs.GetParticleOrientations(&pfs);
  mat rhos = pffs.GetParticleRhos(&pfs);
  mat pos = pffs.GetParticlePositions(&pfs);

  sprintf(fpath, "%s/propagateParticlesInit1Pos.dat", path);
  pos.save(fpath, raw_ascii);
  sprintf(fpath, "%s/propagateParticlesInit1Rhos.dat", path);
  rhos.save(fpath, raw_ascii);
  sprintf(fpath, "%s/propagateParticlesInit1Rs.dat", path);
  saveOrientations(Rs, fpath);

  pffs.Propagate(&u, dt, &pfs);

  Rs = pffs.GetParticleOrientations(&pfs);
  rhos = pffs.GetParticleRhos(&pfs);
  pos = pffs.GetParticlePositions(&pfs);

  sprintf(fpath, "%s/propagateParticlesProp1Pos.dat", path);
  pos.save(fpath, raw_ascii);
  sprintf(fpath, "%s/propagateParticlesProp1Rhos.dat", path);
  rhos.save(fpath, raw_ascii);
  sprintf(fpath, "%s/propagateParticlesProp1Rs.dat", path);
  saveOrientations(Rs, fpath);
  

  //Now use marginalized particle filter
  PFMarginalizedParams ppfm;
  ParticleFilterMarginalized pfm(nParticles, &ppfm);
  pfm.InitializeParticleFilter(hist, &ppfm);

  Rs = pfm.GetParticleOrientations(&ppfm);
  rhos = pfm.GetParticleRhos(&ppfm);
  pos = pfm.GetParticlePositions(&ppfm);

  sprintf(fpath, "%s/propagateParticlesInit3Pos.dat", path);
  pos.save(fpath, raw_ascii);
  sprintf(fpath, "%s/propagateParticlesInit3Rhos.dat", path);
  rhos.save(fpath, raw_ascii);
  sprintf(fpath, "%s/propagateParticlesInit3Rs.dat", path);
  saveOrientations(Rs, fpath);
  std::vector < mat33 > sigmas = pfm.GetParticleOrientationSigmas(&ppfm);
  sprintf(fpath, "%s/propagateParticlesInit3Sigmas.dat", path);
  saveOrientations(sigmas, fpath);

  pfm.Propagate(&u, dt, &ppfm);

  Rs = pfm.GetParticleOrientations(&ppfm);
  rhos = pfm.GetParticleRhos(&ppfm);
  pos = pfm.GetParticlePositions(&ppfm);
  sigmas = pfm.GetParticleOrientationSigmas(&ppfm);

  sprintf(fpath, "%s/propagateParticlesProp3Pos.dat", path);
  pos.save(fpath, raw_ascii);
  sprintf(fpath, "%s/propagateParticlesProp3Rhos.dat", path);
  rhos.save(fpath, raw_ascii);
  sprintf(fpath, "%s/propagateParticlesProp3Rs.dat", path);
  saveOrientations(Rs, fpath);
  sprintf(fpath, "%s/propagateParticlesProp3Sigmas.dat", path);
  saveOrientations(sigmas, fpath);

}

// Assumes one point per measurement
static std::vector < Measurement > loadMeasurements(const char *basePath, bool reverse=true)
{
  using ::s32;
  std::vector < Measurement > res;

  char path[150] = {0};

  sprintf(path, "%sMeasDoppler.dat", basePath);
  mat doppler; doppler.load(path);
  sprintf(path, "%sMeasPos.dat", basePath);
  mat pos; pos.load(path);

  sprintf(path, "%sMeasFul.dat", basePath);
  mat ful; ful.load(path);
  sprintf(path, "%sMeasFbl.dat", basePath);
  mat fbl; fbl.load(path);
  sprintf(path, "%sMeasFbr.dat", basePath);
  mat fbr; fbr.load(path);
  sprintf(path, "%sMeasFur.dat", basePath);
  mat fur; fur.load(path);
  
  sprintf(path, "%sMeasBx.dat", basePath);
  mat bx; bx.load(path);
  sprintf(path, "%sMeasBy.dat", basePath);
  mat by; by.load(path);
  
  sprintf(path, "%sMeasUv.dat", basePath);
  mat uv; uv.load(path);

  for(s32 i=0; i<doppler.n_cols; i++) {
    Measurement m;
    m.doppler = doppler.col(i);
    
    m.pos = (vec)pos.col(i);

    m.ful = (vec)ful.col(i);
    m.fbl = (vec)fbl.col(i);
    m.fbr = (vec)fbr.col(i);
    m.fur = (vec)fur.col(i);

    m.fbx = (vec)bx.col(i);
    m.fby = (vec)by.col(i);

    m.uv = (mat)uv.col(i);

    res.push_back(m);
  }
  if(reverse)
    std::reverse(res.begin(), res.end());
  return res;
}

mat loadTimes(const char *basePath)
{
  char path[150] = {0};
  sprintf(path, "%sdts.dat", basePath);

  mat res; res.load(path);
  res = res.t();
  return res;
}

static std::vector < NSCommand > loadCommands(const char *basePath)
{
  using ::s32;
  std::vector < NSCommand > res;

  char path[150] = {0};

  sprintf(path, "%sV.dat", basePath);
  mat vs; vs.load(path);
  sprintf(path, "%sThetas.dat", basePath);
  mat dthetas; dthetas.load(path);

  for(s32 i=0; i<vs.n_cols; i++) {
    NSCommand u;
    u.dtheta = dthetas(0,i);
    u.v = vs(0,i);
    u.dutyCycle = 0;

    res.push_back(u);
  }
  return res;
}

struct PartMethod1
{
  mat pos;
  mat rhos;
  std::vector < mat33 > Rs;
  mat ws;
};

struct PartMethod3
{
  mat pos;
  mat rhos;
  std::vector < OrientationKF > Rs;
  mat ws;
};

static PartMethod1 loadParticlesMethod1(const char *basePath)
{
  using ::s32;

  char path[150] = {0};

  PartMethod1 res;

  sprintf(path, "%sPos.dat", basePath);
  res.pos.load(path);
  sprintf(path, "%sRho.dat", basePath);
  res.rhos.load(path);
  sprintf(path, "%sws.dat", basePath);
  res.ws.load(path);
  sprintf(path, "%sRs.dat", basePath);
  res.Rs = loadOrientations(path);
  return res;
}

static PartMethod3 loadParticlesMethod3(const char *basePath)
{
  using ::s32;

  char path[150] = {0};

  PartMethod3 res;

  sprintf(path, "%sPos.dat", basePath);
  res.pos.load(path);
  sprintf(path, "%sRho.dat", basePath);
  res.rhos.load(path);
  sprintf(path, "%sws.dat", basePath);
  res.ws.load(path);
  sprintf(path, "%sRs.dat", basePath);
  std::vector < mat33 > Rs = loadOrientations(path);
  sprintf(path, "%ssigmas.dat", basePath);
  std::vector < mat33 > sigmas = loadOrientations(path);

  for(s32 i=0; i<Rs.size(); i++) {
    res.Rs.push_back(OrientationKF(Rs[i], sigmas[i]));
  }

  return res;
}

TEST(ParticleFilter, ApplyMeasurement)
{
  using ::s32;

  char basePath[] = "C:/Joey/ultrasteer/src/MATLAB_CurveFit/NeedleSimulationsParticleFilter/ctests/data/testMeasure";
  char path[150] = {0};

  f64 eps = 1e-2;

  f64 dt = 1/10.0;
  //Method 1, iteration 1
  sprintf(path, "%s11", basePath);
  std::vector < NSCommand > us = loadCommands(path);
  std::vector < Measurement > meas = loadMeasurements(path);
  PartMethod1 ps = loadParticlesMethod1(path);
  sprintf(path, "%sAfter11", basePath);
  PartMethod1 psa = loadParticlesMethod1(path);

  PFFullStateParams pfp;
  ParticleFilterFullState pffs(ps.pos.n_cols, &pfp);
  pffs.SetOrientations(ps.Rs);
  pffs.SetPositions(ps.pos);
  pffs.SetRhos(ps.rhos);
  pffs.SetWeights(ps.ws);
  pffs.ApplyMeasurement(meas, us, ones(pfp.n)*dt, &pfp);
  
  printf("Method 1 iteration 1:\n");
  mat tw = pffs.GetWeights();
  for(s32 i=0; i<tw.n_cols; i++) {
    printf("Calculated weight %.8f matlab weight %.8f\n", tw(0,i), psa.ws(0,i));
    assert(abs(tw(0,i)-psa.ws(0,i))/abs(psa.ws(0,i)) < eps);
  }

  //Method 1, iteration 2
  sprintf(path, "%s12", basePath);
  us = loadCommands(path);
  meas = loadMeasurements(path);
  ps = loadParticlesMethod1(path);
  sprintf(path, "%sAfter12", basePath);
  psa = loadParticlesMethod1(path);

  pffs = ParticleFilterFullState(ps.pos.n_cols, &pfp);
  pffs.SetOrientations(ps.Rs);
  pffs.SetPositions(ps.pos);
  pffs.SetRhos(ps.rhos);
  pffs.SetWeights(ps.ws);
  pffs.ApplyMeasurement(meas, us, ones(pfp.n)*dt, &pfp);

  printf("Method 1 iteration 2:\n");
  tw = pffs.GetWeights();
  for(s32 i=0; i<tw.n_cols; i++) {
    printf("Calculated weight %.8f matlab weight %.8f\n", tw(0,i), psa.ws(0,i));
    if(abs(tw(0,i)-psa.ws(0,i))/abs(psa.ws(0,i)) > eps)
      throw std::runtime_error("");
  }

  //Method 3, iteration 1
  sprintf(path, "%s31", basePath);
  us = loadCommands(path);
  meas = loadMeasurements(path);
  PartMethod3 ps3 = loadParticlesMethod3(path);
  sprintf(path, "%sAfter31", basePath);
  PartMethod3 ps3a = loadParticlesMethod3(path);

  PFMarginalizedParams pfmp;
  pfmp.subsetSize = 2000;
  pfmp.distanceThreshSq = 50*50;
  ParticleFilterMarginalized pfmm(ps3.pos.n_cols, &pfmp);
  pfmm.SetOrientationKFs(ps3.Rs);
  pfmm.SetPositions(ps3.pos);
  pfmm.SetRhos(ps3.rhos);
  pfmm.SetWeights(ps3.ws);
  pfmm.ApplyMeasurement(meas, us, ones(pfmp.n)*dt, &pfmp);
  
  printf("Method 3 iteration 1:\n");
  tw = pfmm.GetWeights();
  std::vector < mat33 > Rmus = pfmm.GetParticleOrientations(&pfmp);
  std::vector < mat33 > sigmas = pfmm.GetParticleOrientationSigmas(&pfmp);
  for(s32 i=0; i<tw.n_cols; i++) {
    printf("Calculated weight %.8f matlab weight %.8f\n", tw(0,i), ps3a.ws(0,i));
    ps3a.Rs[i].mu.print("Matlab mu:  ");
    Rmus[i].print("Calculated mu:  ");
    ps3a.Rs[i].sigma.print("Matlab sigma:  ");
    sigmas[i].print("Calculated sigma:  ");
    if(abs(tw(0,i)-ps3a.ws(0,i))/abs(ps3a.ws(0,i)) > eps)
      throw std::runtime_error("");
    if(max(max(abs(ps3a.Rs[i].mu-Rmus[i])))/max(max(abs(ps3a.Rs[i].mu))) > eps)
      throw std::runtime_error("");
    if(max(max(abs(ps3a.Rs[i].sigma-sigmas[i])))/max(max(abs(ps3a.Rs[i].sigma))) > eps)
      throw std::runtime_error("");
  }

  //Method 3, iteration 2
  sprintf(path, "%s32", basePath);
  us = loadCommands(path);
  meas = loadMeasurements(path);
  ps3 = loadParticlesMethod3(path);
  sprintf(path, "%sAfter32", basePath);
  ps3a = loadParticlesMethod3(path);

  pfmm = ParticleFilterMarginalized(ps3.pos.n_cols, &pfmp);
  pfmm.SetOrientationKFs(ps3.Rs);
  pfmm.SetPositions(ps3.pos);
  pfmm.SetRhos(ps3.rhos);
  pfmm.SetWeights(ps3.ws);
  pfmm.ApplyMeasurement(meas, us, ones(pfmp.n)*dt, &pfmp);

  Rmus = pfmm.GetParticleOrientations(&pfmp);
  sigmas = pfmm.GetParticleOrientationSigmas(&pfmp);

  printf("Method 3 iteration 2:\n");
  tw = pfmm.GetWeights();
  for(s32 i=0; i<tw.n_cols; i++) {
    printf("Calculated weight %.8f matlab weight %.8f\n", tw(0,i), ps3a.ws(0,i));
    ps3a.Rs[i].mu.print("Matlab mu:  ");
    Rmus[i].print("Calculated mu:  ");
    ps3a.Rs[i].sigma.print("Matlab mu:  ");
    sigmas[i].print("Calculated mu:  ");
    if(abs(tw(0,i)-ps3a.ws(0,i))/abs(ps3a.ws(0,i)) > eps)
      throw std::runtime_error("");
    if(max(max(abs(ps3a.Rs[i].mu-Rmus[i])))/max(max(abs(ps3a.Rs[i].mu))) > eps)
      throw std::runtime_error("");
    if(max(max(abs(ps3a.Rs[i].sigma-sigmas[i])))/max(max(abs(ps3a.Rs[i].sigma))) > eps)
      throw std::runtime_error("");
  }

}

static f64 computeAveragePositionError(const mat &delta)
{
  mat d = delta;
  d = square(d);
  d = sum(d, 0);
  d = sqrt(d);
  d.save("C:/Joey/ultrasteer/src/MATLAB_CurveFit/NeedleSimulationsParticleFilter/ctests/data/testWholeSystemCPosError.dat", raw_ascii);
  d = mean(d, 1);
  return d(0,0);
}

static f64 computeAverageOrientationError(std::vector < mat33 > R1, std::vector < mat33 > R2)
{
  using ::s32;
  f64 error = 0;
  for(s32 i=0; i<R1.size(); i++) {
    error += SO3Distance(R1[i], R2[i]);
  }
  error = error/(f64)R1.size();
  return error;
}

TEST(ParticleFilter, WholeSystem)
{
  using ::s32;
  using ::u32;

#if 1
  char basePath[] = "C:/Joey/ultrasteer/src/MATLAB_CurveFit/NeedleSimulationsParticleFilter/ctests/data/testWholeSystem";
#else
  char basePath[] = "C:/Users/Joey/Documents/ultrasteer/src/MATLAB_CurveFit/NeedleSimulationsParticleFilter/ctests/data/testWholeSystem";
#endif
  char path[150] = {0};

  f64 dt = 1/10.0;

  s32 nTests = 1;

                      //method 1        //Method 3     
  s32 nParticles[] = {500,              200};

  std::vector < f64 > avePosErrors;
  std::vector < f64 > aveOrErrors;

  PFFullStateParams *pfp;
  ParticleFilterFullState *pffs;

  PFMarginalizedParams *pmp;
  ParticleFilterMarginalized *pfms;

  for(s32 mth=0; mth<2; mth++) {
    //Method 1
    for(s32 i=0; i<nTests; i++) {
      printf("\nMethod %d Trial %d\n\n", mth+1,i);
      s32 initTime = 2;
      sprintf(path, "%s1%d", basePath, i+1);
      std::vector < NSCommand > us = loadCommands(path);
      std::vector < Measurement > meas = loadMeasurements(path, false);
      mat dts = loadTimes(path);
      PartMethod1 ps = loadParticlesMethod1(path);

      ParticleFilter *pf;
      PFParams *pp;

      if(mth == 0) {
        pfp = new PFFullStateParams();
        pfp->particleSigmaRho = 0;
        pf =  new ParticleFilterFullState(nParticles[mth], pfp);
        pp = pfp;
      } else {
        pmp = new PFMarginalizedParams();
        pmp->particleSigmaRho = 0;
        pfms = new ParticleFilterMarginalized(nParticles[mth], pmp);
        pf = pfms;
        pp = pmp;
      }

      std::vector < TipState > estStates;
      mat estPos;
      mat truePos;
      std::vector < mat33 > estRs;
      std::vector < mat33 > trueRs;
      bool started = false;
      s32 dj = 0;
      s32 jj = 0;
      for(s32 j=0; j<dts.n_cols; j++) {
        if(dts(0,j) <= initTime) {
          dj = j;
          continue;
        }
        jj = j-dj;
        printf("\rframe %d of %d", j, dts.n_cols);
        if(jj >= pp->minimumMeasurements && !started)  {
          TipState t; 
          t.pos = ps.pos.col(j);
          t.R = ps.Rs[j];
          t.rho = ps.rhos(0,j);
          std::vector < TipState > hist;
          hist.push_back(t);

          pf->InitializeParticleFilter(hist, pp);

          started = true;
          continue;
        }
        if(!started)
          continue;


        pf->Propagate(&us[j], dts(0,j)-dts(0,j-1), pp);

        std::vector < Measurement > cmeas(jj);
        std::vector < NSCommand > cu(j+1);
        mat cdts(1,j+1);
        for(s32 k=0; k<=j; k++) {
          cu[k] = us[k];
          if(k < j)
            cdts(0,k) = dts(0,k+1)-dts(0,k);
          else
            cdts(0,k) = dts(0,k)-dts(0,k-1);
        }
        for(s32 k=0; k<jj; k++) {
          cmeas[k] = meas[k];
        }

        std::reverse(cmeas.begin(), cmeas.end());
        std::reverse(cu.begin(), cu.end());
        cdts = fliplr(cdts);
        while(cu.size() < pp->n) {
          NSCommand u; memset(&u, 0, sizeof(NSCommand));
          cu.push_back(u);
        }
        while(cu.size() > pp->n) {
          cu.pop_back();
        }
        while(cmeas.size() > pp->n*1.5)
          cmeas.pop_back();
        
        BEGIN_TIMING(ApplyMeasurement, 450)
        pf->ApplyMeasurement(cmeas, cu, cdts.t(), pp);
        END_TIMING(ApplyMeasurement, 450)
        TipState est = pf->GetExpectedValue(pp);
        estStates.push_back(est);
        estPos = join_horiz(estPos, est.pos);
        estRs.push_back(est.R);
        truePos = join_horiz(truePos, ps.pos.col(j));
        trueRs.push_back(ps.Rs[j]);

        f64 nParts = pf->EffectiveSampleSize();
        if(nParts < pp->neff*nParticles[mth])
          pf->Resample(nParticles[mth], pp);
      }

      f64 avePosError = computeAveragePositionError(estPos-truePos);
      NTrace("Ave Pos Error %f\n", avePosError);
      avePosErrors.push_back(avePosError);
      f64 aveOrError = computeAverageOrientationError(estRs,trueRs);
      NTrace("Ave Orientation Error %f\n", aveOrError);
      aveOrErrors.push_back(aveOrError);

      delete pf;
      delete pp;
    }
  }

}


