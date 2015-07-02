#include "RTCore.h"
#include <math.h>

using namespace arma;
namespace Nf
{
  mat33 SO3Hat(const vec3 &x)
  {
    mat33 res;
    res << 0      << -x(2)  << x(1)   << endr
        << x(2)   << 0      << -x(0)  << endr
        << -x(1)  << x(0)   << 0      << endr;
    return res;
  }

  vec3 SO3HatInverse(const mat33 &x)
  {
    vec3 res;
    res << x(2, 1) << endr << -x(2,0) << endr << -x(0,1) << endr;
    return res;
  }

  mat33 SO3Exp(const vec3 &x)
  {
    f64 theta = norm(x);
    if(theta < 1e-5)
      return eye(3,3);
    mat33 K = SO3Hat(x/theta);
    return eye(3,3)+sin(theta)*K+(1-cos(theta))*K*K;
  }

  mat33 Rx(f64 theta)
  {
    f64 ct = cos(theta);
    f64 st = sin(theta);

    mat33 res;
    res <<  1   <<  0   <<  0   << endr
        <<  0   <<  ct  <<  -st << endr
        <<  0   <<  st  <<   ct << endr;
    return res;
  }

  mat33 Ry(f64 theta)
  {
    f64 ct = cos(theta);
    f64 st = sin(theta);

    mat33 res;
    res <<  ct  <<  0   <<  -st << endr
        <<  0   <<  1   <<  0   << endr
        <<  st  <<  0   <<  ct  << endr;
    return res;
  }

  mat33 Rz(f64 theta)
  {
    f64 ct = cos(theta);
    f64 st = sin(theta);

    mat33 res;
    res <<  ct  <<  -st <<  0   << endr
        <<  st  <<  ct  <<  0   << endr
        <<  0   <<  0   <<  1   << endr;
    return res;
  }

  //taken from http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToAngle/
  vec3 SO3Log(const mat33 &m)
  {
    f64 angle,x,y,z; // variables for result
    f64 epsilon = 0.01; // margin to allow for rounding errors
    f64 epsilon2 = 0.1; // margin to distinguish between 0 and 180 degrees
    // optional check that input is pure rotation, 'isRotationMatrix' is defined at:
    // http://www.euclideanspace.com/maths/algebra/matrix/orthogonal/rotation/
    DEBUG_ASSERT(abs(det(m) - 1) < epsilon);
    if ((abs(m(0,1)-m(1,0))< epsilon) && (abs(m(0,2)-m(2,0))< epsilon)
      && (abs(m(1,2)-m(2,1))< epsilon)) {
        // singularity found
        // first check for identity matrix which must have +1 for all terms
        //  in leading diagonaland zero in other terms
        if ((abs(m(0,1)+m(1,0)) < epsilon2) && (abs(m(0,2)+m(2,0)) < epsilon2)
          && (abs(m(1,2)+m(2,1)) < epsilon2) && (abs(m(0,0)+m(1,1)+m(2,2)-3) < epsilon2)) {
            // this singularity is identity matrix so angle = 0
            zeros<vec>(3,1);
        }

        // otherwise this singularity is angle = 180
        angle = PI;
        f64 xx = (m(0,0)+1)/2;
        f64 yy = (m(1,1)+1)/2;
        f64 zz = (m(2,2)+1)/2;
        f64 xy = (m(0,1)+m(1,0))/4;
        f64 xz = (m(0,2)+m(2,0))/4;
        f64 yz = (m(1,2)+m(2,1))/4;
        if ((xx > yy) && (xx > zz)) { // m(0,0) is the largest diagonal term
          if (xx< epsilon) {
            x = 0;
            y = 0.7071;
            z = 0.7071;
          } else {
            x = sqrt(xx);
            y = xy/x;
            z = xz/x;
          }
        } else if (yy > zz) { // m(1,1) is the largest diagonal term
          if (yy< epsilon) {
            x = 0.7071;
            y = 0;
            z = 0.7071;
          } else {
            y = sqrt(yy);
            x = xy/y;
            z = yz/y;
          }	
        } else { // m(2,2) is the largest diagonal term so base result on this
          if (zz< epsilon) {
            x = 0.7071;
            y = 0.7071;
            z = 0;
          } else {
            z = sqrt(zz);
            x = xz/z;
            y = yz/z;
          }
        }
        vec3 res;
        res << x << endr << y << endr << z << endr;
        return angle*res; // return 180 deg rotation
    }
    // as we have reached here there are no singularities so we can handle normally
    f64 s = sqrt((m(2,1) - m(1,2))*(m(2,1) - m(1,2))
      +(m(0,2) - m(2,0))*(m(0,2) - m(2,0))
      +(m(1,0) - m(0,1))*(m(1,0) - m(0,1))); // used to normalise
    if (abs(s) < 0.001) s=1; 
    // prevent divide by zero, should not happen if matrix is orthogonal and should be
    // caught by singularity test above, but I've left it in just in case
    angle = acos(( m(0,0) + m(1,1) + m(2,2) - 1)/2);
    x = (m(2,1) - m(1,2))/s;
    y = (m(0,2) - m(2,0))/s;
    z = (m(1,0) - m(0,1))/s;
    vec3 res;
    res << x << endr << y << endr << z << endr;
    return angle*res;
  }

  static f64 g_normpdfconst = 1/sqrt(2*PI);
  static f64 g_erfconst = 1/sqrt(2.0);

  //taken from http://stackoverflow.com/questions/6281020/error-function-erfx-not-found-in-math-h-for-visual-studio-2005
  static double erf(double x)
  {
    // constants
    double a1 =  0.254829592;
    double a2 = -0.284496736;
    double a3 =  1.421413741;
    double a4 = -1.453152027;
    double a5 =  1.061405429;
    double p  =  0.3275911;

    // Save the sign of x
    int sign = 1;
    if (x < 0)
      sign = -1;
    x = fabs(x);

    // A&S formula 7.1.26
    double t = 1.0/(1.0 + p*x);
    double y = 1.0 - (((((a5*t + a4)*t) + a3)*t + a2)*t + a1)*t*exp(-x*x);

    return sign*y;
  }

  f64 normpdf(vec2 x, vec2 mu, vec2 sigma)
  {
    vec2 d12 = square(x-mu);
    vec2 isigma = pow(sigma,-1);
    return g_normpdfconst*g_normpdfconst*prod(isigma(1))*exp(-0.5*dot(d12,isigma));
  }

  f64 normcdf(vec2 x, vec2 mu, vec2 sigma)
  {
    vec2 arg = g_erfconst*(x-mu)/sigma;
    return 0.5*0.5*(1+erf(arg(0)))*(1+erf(arg(1)));
  }

  f64 TruncatedIndependentGaussianPDF2(vec2 x, vec2 mu, vec2 sigma, vec2 a, vec2 b)
  {
    f64 num = normpdf(x,mu,sigma);
    f64 den = normcdf(b,mu,sigma)-normcdf(a,mu,sigma);
    return num/den;
  }
}