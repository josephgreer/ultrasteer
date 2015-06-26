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