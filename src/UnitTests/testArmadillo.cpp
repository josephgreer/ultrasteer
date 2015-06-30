#include "CppUnitLite/Test.h"
#include "RTCore.h"
#include "RPFileReader.h"
#include "RPUlterius.h"

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