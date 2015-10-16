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

TEST(Scratch, Armadillo)
{
  cout << "Armadillo version: " << arma_version::as_string() << endl;
  
  mat A(2,3);  // directly specify the matrix size (elements are uninitialised)
  
  cout << "A.n_rows: " << A.n_rows << endl;  // .n_rows and .n_cols are read only
  cout << "A.n_cols: " << A.n_cols << endl;
  
  A(1,2) = 456.0;  // directly access an element (indexing starts at 0)
  A.print("A:");
  
  
  A = 5.0;         // scalars are treated as a 1x1 matrix
  A.print("A:");
  
  
  A.set_size(4,5); // change the size (data is not preserved)
  
  A.fill(5.0);     // set all elements to a particular value
  A.print("A:");
  
  // endr indicates "end of row"
  A << 0.165300 << 0.454037 << 0.995795 << 0.124098 << 0.047084 << endr
    << 0.688782 << 0.036549 << 0.552848 << 0.937664 << 0.866401 << endr
    << 0.348740 << 0.479388 << 0.506228 << 0.145673 << 0.491547 << endr
    << 0.148678 << 0.682258 << 0.571154 << 0.874724 << 0.444632 << endr
    << 0.245726 << 0.595218 << 0.409327 << 0.367827 << 0.385736 << endr;
  
  A.print("A:");
  
  // determinant
  cout << "det(A): " << det(A) << endl;
  
  // inverse
  cout << "inv(A): " << endl << inv(A) << endl;
  
  // save matrix as a file
  A.save("A.txt", raw_ascii);
  
  // load from file
  mat B;
  B.load("A.txt");
  
  // submatrices
  cout << "B( span(0,2), span(3,4) ):" << endl << B( span(0,2), span(3,4) ) << endl;
  
  cout << "B.row(0): " << endl << B.row(0) << endl;
  
  cout << "B.col(1): " << endl << B.col(1) << endl;
  
  // transpose
  cout << "B.t(): " << endl << B.t() << endl;
  
  // maximum from each column (traverse along rows)
  cout << "max(B): " << endl << max(B) << endl;
  
  // maximum from each row (traverse along columns)
  cout << "max(B,1): " << endl << max(B,1) << endl;
  
  // maximum value in B
  cout << "max(max(B)) = " << max(max(B)) << endl;
  
  // sum of each column (traverse along rows)
  cout << "sum(B): " << endl << sum(B) << endl;
  
  // sum of each row (traverse along columns)
  cout << "sum(B,1) =" << endl << sum(B,1) << endl;
  
  // sum of all elements
  cout << "accu(B): " << accu(B) << endl;
  
  // trace = sum along diagonal
  cout << "trace(B): " << trace(B) << endl;
  
  // generate the identity matrix
  mat C = eye<mat>(4,4);
  
  // random matrix with values uniformly distributed in the [0,1] interval
  mat D = randu<mat>(4,4);
  D.print("D:");
  
  // row vectors are treated like a matrix with one row
  rowvec r;
  r << 0.59119 << 0.77321 << 0.60275 << 0.35887 << 0.51683;
  r.print("r:");
  
  // column vectors are treated like a matrix with one column
  colvec q;
  q << 0.14333 << 0.59478 << 0.14481 << 0.58558 << 0.60809;
  q.print("q:");
  
  // dot or inner product
  cout << "as_scalar(r*q): " << as_scalar(r*q) << endl;
  
  // outer product
  cout << "q*r: " << endl << q*r << endl;
  
  // multiply-and-accumulate operation (no temporary matrices are created)
  cout << "accu(A % B) = " << accu(A % B) << endl;
  
  // example of a compound operation
  B += 2.0 * A.t();
  B.print("B:");
  
  // imat specifies an integer matrix
  imat AA;
  imat BB;
  
  AA << 1 << 2 << 3 << endr << 4 << 5 << 6 << endr << 7 << 8 << 9;
  BB << 3 << 2 << 1 << endr << 6 << 5 << 4 << endr << 9 << 8 << 7;
  
  // comparison of matrices (element-wise); output of a relational operator is a umat
  umat ZZ = (AA >= BB);
  ZZ.print("ZZ:");
  
  // cubes ("3D matrices")
  cube Q( B.n_rows, B.n_cols, 2 );
  
  Q.slice(0) = B;
  Q.slice(1) = 2.0 * B;
  
  Q.print("Q:");
  
  // 2D field of arbitrary length row vectors (fields can also store abitrary objects, eg. instances of std::string)
  field<rowvec> xyz(3,2);
  
  xyz(0,0) = randu(1,2);
  xyz(1,0) = randu(1,3);
  xyz(2,0) = randu(1,4);
  xyz(0,1) = randu(1,5);
  xyz(1,1) = randu(1,6);
  xyz(2,1) = randu(1,7);
  
  cout << "xyz:" << endl;
  cout << xyz << endl;
}

TEST(Armadillo, RandSeed)
{
  using ::s32;
  srand(1);

 
  vec samples = randn(500);
  samples.save("C:/Joey/ultrasteer/src/MATLAB_CurveFit/NeedleSimulationsParticleFilter/ctests/randn.dat", raw_ascii);
}

TEST(Math, Gaussian)
{
  using ::s32;
  vec3 mu;
  mu << -1 << endr
     << 2  << endr
     << 3  << endr;

  mat33 sigma;
  sigma << 18.3983  << -25.2563 << 25.2667  << endr
        << -25.2563 <<  44.3550 << -41.0474 << endr
        << 25.2667  << -41.0474 << 42.2467  << endr;

  s32 N = 1000;
  Gaussian<vec3,mat33> gauss(mu, sigma);

  mat samples = gauss.Sample(N);
  samples = samples.t();

  samples.save("C:/Joey/ultrasteer/src/MATLAB_CurveFit/NeedleSimulationsParticleFilter/ctests/data/bulksamples.dat", raw_ascii);

  mat samplesInd;
  for(s32 i=0; i<N; i++) {
    samplesInd = join_vert(samplesInd,gauss.Sample().t());
  }
  samplesInd.save("C:/Joey/ultrasteer/src/MATLAB_CurveFit/NeedleSimulationsParticleFilter/ctests/data/individualsamples.dat", raw_ascii);

  std::vector < vec3 > xs;
  vec3 xc;
  
  xc << 1 << 2 << 3 << endr;
  xs.push_back(xc);
  xc << 1.5 << -1 << 4 << endr;
  xs.push_back(xc);
  xc << 4 << 2 << 1 << endr;
  xs.push_back(xc);

  std::vector < f64 > answers;
  answers.push_back(0.00138068842225824);
  answers.push_back(0.0010908275975957);
  answers.push_back(6.88609511352968e-06);
  f64 eps = 1e-5;
  for(s32 i=0; i<xs.size(); i++) {
    xs[i].print("x:  ");
    f64 e = gauss.Eval(xs[i]);
    printf("gauss(x):  %.8f\n", gauss.Eval(xs[i]));
    assert(abs(e-answers[i]) < eps);
  }
}

TEST(Math, SO3)
{
  using ::s32; 

  char dir[100] = "C:/Joey/ultrasteer/src/MATLAB_CurveFit/NeedleSimulationsParticleFilter/ctests/data";
  char path[150] = "";

  //Generate matlab data to test
  s32 nTests = 5;

  f64 eps = 1e-4;

  //SO3Exp 
  printf("SO3Exp\n");
  for(s32 i=0; i<nTests; i++) {
    printf("i=%d\n", i);
    sprintf(path, "%s/SO3ExpIn%d.dat", dir, i+1);
    vec3 v;
    assert(v.load(path, auto_detect));
    v.print("v: ");
    
    sprintf(path, "%s/SO3ExpOut%d.dat", dir, i+1);
    mat33 mm;
    assert(mm.load(path));
    mm.print("mm: ");

    mat33 tmm = SO3Exp(v);
    tmm.print("tmm: ");
    f64 sm = sum(sum(abs(tmm-mm)));
    assert(sm < eps);
  }

  //SO3Log
  printf("SO3Log\n");
  for(s32 i=0; i<nTests; i++) {
    printf("i = %d\n", i);
    sprintf(path, "%s/SO3ExpIn%d.dat", dir, i+1);
    vec3 v;
    assert(v.load(path, auto_detect));
    v.print("v: ");

    mat33 mm = SO3Exp(v);

    vec3 vv = SO3Log(mm);
    
    vec3 vn = v/norm(v);
    vec3 vvn = vv/norm(vv);
    vn.print("vn: ");
    vvn.print("vvn: ");

    f64 diff = sum(abs(vn-vvn));
    f64 negDiff = sum(abs(-vn-vvn));
    assert(min(diff, negDiff) < eps);

    f64 a1 = fmodf(norm(v),2*PI);
    f64 a2 = fmodf(norm(vv), 2*PI);
    if(diff < negDiff)
      assert(abs(a1-a2) < eps);
    else
      assert(abs(a1+a2-2*PI) < eps);
  }

}

TEST(Math, LogNormal)
{
  using ::s32;

  f64 mu[] = {2.08511002,1.51166286,0.93130106,2.69408367,1.02226125,3.35233755,0.70193469,4.84130788,4.38194576,0.19527392,0.49173417,2.66582642,3.43250464,3.75072157,1.40221996,2.23946763,1.43887669,3.39417766,2.45786580,0.73364287};
  f64 sigma[] = {7.20324493,1.46755891,3.45560727,4.19194514,8.78117436,4.17304802,1.98101489,3.13424178,8.94606664,1.69830420,4.21107625,6.91877114,8.34625672,9.88861089,7.89279328,9.08595503,1.30028572,2.11628116,0.53362545,5.89305537};
  f64 x[] = {2.96044257,2.00639595,2.06439252,21.42423313,1.08008143,32.12765571,3.68188125,186.03840110,34.88420626,2.58974373,4.08578236,9.94288964,11.81158929,69.89910768,1.83798888,6.21332344,1.61221758,18.63927630,13.54613149,3.10545299};
  f64 answer[] = {0.01852858,0.11611146,0.05582365,0.00442480,0.04182009,0.00297444,0.05223167,0.00067905,0.00127286,0.08214399,0.02264499,0.00579096,0.00401991,0.00057644,0.02736160,0.00705939,0.14480092,0.00986841,0.05310088,0.02174936};


  f64 eps = 1e-6;

  for(s32 i=0; i<sizeof(mu)/sizeof(mu[0]); i++) {
    f64 canswer = lognpdf(x[i], mu[i], sigma[i]);
    printf("answer %.8f canswer %.8f\n", answer[i], canswer);
    assert(abs(answer[i]-canswer)<eps);
  }
}

TEST(Math, TruncatedIndependentGaussians)
{
  using ::s32;

  f64 mu[20][2] = {{2.08511002, 3.60162247},{2.09597257, 3.42609750},{4.00372284, 4.84130788},{0.49173417, 2.10553813},{4.94430544, 3.74082827},{0.09683479, 3.39417766},{0.51167214, 2.07027994},{4.51700958, 0.68737352},{4.41653046, 3.11836104},{0.57372986, 4.74744629},{1.63322451, 2.63529051},{0.86170254, 0.68567875},{0.09940067, 0.13105493},{2.92879636, 4.84797874},{2.78120117, 0.68227613},{0.35987140, 4.83638165},{1.19923880, 2.46884857},{1.58681205, 4.94308077},{3.14858754, 1.05087005},{1.31648385, 0.32980545}};
  f64 sigma[20][2] = {{0.00114375, 3.02332573},{2.04452250, 8.78117436},{3.13424178, 6.92322616},{9.57889530, 5.33165285},{2.80443992, 7.89279328},{2.11628116, 2.65546659},{6.94400158, 4.14179270},{1.39276347, 8.07391289},{7.50942434, 3.48898342},{4.49912133, 5.78389614},{8.85942099, 3.57269760},{9.32595463, 6.96818161},{0.28306488, 2.46211068},{5.61030219, 0.18647289},{0.59917690, 1.21343456},{5.68100462, 2.03293235},{6.19955718, 8.28980900},{5.79745219, 3.80141173},{7.52755554, 0.66536481},{7.35065963, 7.72178030}};
  f64 x[20][2] = {{1.37862181, 2.78629966},{1.15074776, 3.76703252},{4.75650115, 5.63052121},{0.87548840, 1.73656939},{4.15075746, 3.63661532},{0.07998111, 2.50090275},{-0.38842094, 2.14207275},{4.31236325, 0.01808191},{3.95638624, 3.91013347},{0.39000347, 4.22150025},{2.45029481, 2.88201074},{-0.00629711, 1.19660485},{0.81945657, 0.20871706},{3.53006170, 4.31392729},{1.87030493, -0.10273561},{-0.13547711, 5.32403336},{0.51282159, 1.50600098},{1.68870848, 5.43374963},{2.66921773, 1.66037918},{2.13211556, 1.19374959}};
  f64 a[20][2] = {{-3.42480829, -0.63389172},{-1.56558901, 0.89561613},{-2.31592347, -1.84625386},{-1.01625841, 1.78254350},{5.21306947, -0.91025854},{-2.31022437, -2.43199374},{-1.17797069, -0.81060716},{4.93707822, -3.53049960},{0.84125997, 3.83708141},{0.80076603, 2.33688219},{-5.24020555, 3.07078838},{-0.10728795, 1.06987503},{-2.47802350, -0.13269793},{2.38563792, 0.95086389},{-2.41312412, -0.61381203},{-5.07669275, 2.48725307},{-5.24058405, -0.64039054},{-0.05932481, 0.06243723},{-2.30393820, -0.83344290},{-5.57190356, -4.79529786}};
  f64 b[20][2] = {{4.51054120, 10.14244296},{1.24214976, 4.85764591},{1.08068493, 15.71659621},{-0.65049286, 16.78542980},{10.96857624, 1.69031290},{9.47588637, 11.56317346},{17.71392443, 10.92049365},{19.95332028, 10.98946011},{14.11008993, 16.27099582},{0.85817257, 14.67978046},{8.57773280, 23.01724539},{14.12320722, 3.55529427},{0.00544280, 5.45097565},{19.65647501, 15.89329675},{8.78121552, -0.36269243},{14.32370703, 19.42382910},{6.88600518, 10.73663820},{1.26737188, 7.46412119},{8.18946798, 17.66271650},{6.76366358, 14.18502856}};
  f64 answer[] = {0.00000000,0.14823013,0.05947609,0.39503055,0.12942174,0.03115197,0.01257156,0.06113295,0.02416225,1.93579097,0.01959804,0.03805289,0.04603243,0.00466167,1.07857614,0.01848454,0.00935699,0.12328050,0.04064620,0.00653939};

  f64 eps = 1e-4;

  s32 n = sizeof(mu)/sizeof(mu[0]);
  for(s32 i=0; i<sizeof(mu)/sizeof(mu[0]); i++) {
    vec2 cx(x[i]);
    vec2 cmu(mu[i]);
    vec2 ca(a[i]);
    vec2 cb(b[i]);
    vec2 csigma(sigma[i]);

    f64 canswer = TruncatedIndependentGaussianPDF2(cx, cmu, csigma, ca, cb);
    printf("answer %.8f canswer %.8f\n", answer[i], canswer);
    assert(abs(answer[i]-canswer)<eps);
  }
}

TEST(Math, Erf)
{
  using ::s32;
  f64 x[] = {-0.16595599,0.44064899,-0.99977125,-0.39533485,-0.70648822,-0.81532281,-0.62747958,-0.30887855,-0.20646505,0.07763347,-0.16161097,0.37043900,-0.59109550,0.75623487,-0.94522481,0.34093502,-0.16539040,0.11737966,-0.71922612,-0.60379702};
  f64 answer[] = {-0.18555625,0.46682835,-0.84260582,-0.42389828,-0.68226596,-0.75110627,-0.62513110,-0.33775827,-0.22970242,0.08742432,-0.18078320,0.39963790,-0.59680868,0.71514549,-0.81869566,0.37030411,-0.18493532,0.13184297,-0.69091285,-0.60683846};


  f64 eps = 1e-6;

  for(s32 i=0; i<sizeof(x)/sizeof(x[0]); i++) {
    f64 canswer = erf(x[i]);
    printf("answer %.8f canswer %.8f\n", answer[i], canswer);
    assert(abs(answer[i]-canswer)<eps);
  }
}

TEST(Math, SO3Mean)
{
  using ::s32;

  char dir[100] = "C:/Joey/ultrasteer/src/MATLAB_CurveFit/NeedleSimulationsParticleFilter/ctests/data";
  char pathBase[150] = "";

  sprintf(pathBase, "%s/testSO3MeanRs.dat", dir);

  std::vector < mat33 > Rs = loadOrientations(pathBase);

  std::vector < mat33 > Rss(Rs.size()-1);
  for(s32 i=0; i<Rs.size()-1; i++) {
    Rss[i] = Rs[i];
  }

  mat33 R = SO3Mean(Rss, ones(1,Rss.size())/(f64)Rss.size(), 1e-3);
  R.print("R:  ");
  Rs[Rs.size()-1].print("Rcomp:  ");

  assert(norm(R-Rs[Rs.size()-1]) < 1e-2);
}

TEST(Math, Sample)
{
  using ::s32;

  s32 npts = 50;
  vec dist = (vec)randu(npts,1);
  f64 sm = ((mat)sum(dist,0))(0,0);
  dist = dist/sm;
  sm = ((mat)sum(dist,0))(0,0);
  assert(abs(sm-1) < 1e-4);

  s32 n = 1e6;
  uvec samples = Sample(dist, n);

  vec weights = zeros(npts,1);
  uvec idxs;
  for(s32 i=0; i<npts; i++) {
    idxs = find(samples == i);
    weights(i) = (f64)idxs.n_rows/(f64)n;
    assert(abs(weights(i)-dist(i)) < 1e-3);
    if(abs(weights(i)-dist(i)) > 1e-3)
      throw std::runtime_error("stupid");
  }

}

TEST(Math, SO3Distance)
{
  using ::s32;

  mat33 R1,R2;

  char dir[100] = "C:/Joey/ultrasteer/src/MATLAB_CurveFit/NeedleSimulationsParticleFilter/ctests/data";
  char pathBase[150] = "";

  sprintf(pathBase, "%s/testSO3DistanceR1.dat", dir);
  R1.load(pathBase);
  
  sprintf(pathBase, "%s/testSO3DistanceR2.dat", dir);
  R2.load(pathBase);

 f64 dist = SO3Distance(R1,R2);
 NTrace("Dist %f\n", dist);
}

TEST(Math, LUTDist)
{
  using ::s32;
  LUTDist pDopOverNeedle(PATH_CAT("Trial3/Insertion/pdopoverneedle.dat"));
  LUTDist pDopNotOverNeedle(PATH_CAT("Trial3/Insertion/pdopnotoverneedle.dat"));

  vec xs = linspace(-200, 1000, 10000);
  vec poverneedle = zeros(xs.n_elem);
  vec pnotoverneedle = zeros(xs.n_elem);
  for(s32 i=0; i<xs.n_elem; i++) {
    poverneedle(i) = pDopOverNeedle.P(xs(i));
    pnotoverneedle(i) = pDopNotOverNeedle.P(xs(i));
  }

  xs.save("C:/Joey/Data/probDist/testXs.mat", raw_ascii);
  poverneedle.save("C:/Joey/Data/probDist/testPOverNeedle.mat", raw_ascii);
  pnotoverneedle.save("C:/Joey/Data/probDist/testPNotOverNeedle.mat", raw_ascii);
}

TEST(Math, CurveFit)
{
  using ::s32; 
  //std::string dir[100] = "C:/Joey/ultrasteer/src/MATLAB_CurveFit/NeedleSimulationsParticleFilter/ctests/data/";
  std::string dir = "C:/Users/Joey/Documents/ultrasteer/src/MATLAB_CurveFit/NeedleSimulationsParticleFilter/ctests/data/";

  arma::mat xpt,ypt,zpt;
  xpt.load((dir+"xpt.dat").c_str());
  ypt.load((dir+"ypt.dat").c_str());
  zpt.load((dir+"zpt.dat").c_str());

  arma::mat vander;
  s32 degree = 3;
  for(s32 deg=degree; deg>=0; deg--) {
    vander = arma::join_horiz(vander,pow(zpt,deg));
  }

  arma::vec xcoeff = solve(vander, xpt);
  arma::vec ycoeff = solve(vander, ypt);

  xcoeff.print("xcoeff:  ");
  ycoeff.print("ycoeff:  ");
}

TEST(Math, Polynomial)
{
  using ::s32; 
  //std::string dir[100] = "C:/Joey/ultrasteer/src/MATLAB_CurveFit/NeedleSimulationsParticleFilter/ctests/data/";
  std::string dir = "C:/Users/Joey/Documents/ultrasteer/src/MATLAB_CurveFit/NeedleSimulationsParticleFilter/ctests/data/";

  arma::mat xpt,ypt,zpt;
  xpt.load((dir+"xpt.dat").c_str());
  ypt.load((dir+"ypt.dat").c_str());
  zpt.load((dir+"zpt.dat").c_str());

  arma::mat pts = join_horiz(xpt, join_horiz(ypt, zpt));

  Polynomial p(pts, 3, 2);
  p.Print("Coefficients:  ");

  p(1).print("p(1): ");
  p(-1).print("p(-1):  ");
  p(5e-3).print("p(5e-3):  ");

  mat points = p.UniformlySpacedPoints(-10, 10, 0.33, 6);
  for(s32 i=0; i<points.n_rows-1; i++) {
    f64 dist = norm(points.row(i+1)-points.row(i));
    printf("Dist %d: %f\n", i, dist);
  }

  points.save((dir+"uniform_pts.dat").c_str(), arma::raw_ascii);
}