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

#define CLAMP(x,y,z) ((x) < (y) ? (y) : ( (x) > (z) ? (z) : (x) ))
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define MAX(x,y) ((x) > (y) ? (x) : (y))
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
            return zeros<vec>(3,1);
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
    f64 mag = ( m(0,0) + m(1,1) + m(2,2) - 1)/2;
    angle = acos(CLAMP(mag,-1,1));
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
  f64 erf(f64 x)
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

  f64 normpdf(f64 x, f64 mu, f64 sigma)
  {
    f64 dx = x-mu;
    return g_normpdfconst*exp(-dx*dx/(2*sigma*sigma));
  }

  f64 normpdf2(vec2 x, vec2 mu, vec2 sigma)
  {
    vec2 d12 = square(x-mu);
    vec2 isigma = pow(sigma,-1);
    return g_normpdfconst*g_normpdfconst*prod(isigma)*exp(-0.5*dot(d12,square(isigma)));
  }

  vec2 normcdf2(vec2 x, vec2 mu, vec2 sigma)
  {
    vec2 arg = g_erfconst*(x-mu)/sigma;
    vec2 eerf; eerf << 1+erf(arg(0)) << 1+erf(arg(1)) << endr;
    return 0.5*eerf;
  }

  f64 TruncatedIndependentGaussianPDF2(vec2 x, vec2 mu, vec2 sigma, vec2 a, vec2 b)
  {
    f64 num = normpdf2(x,mu,sigma);
    f64 den = prod(normcdf2(b,mu,sigma)-normcdf2(a,mu,sigma));
    return num/den;
  }

  f64 lognpdf(f64 x, f64 mu, f64 sigma)
  {
    f64 dx = log(x)-mu;
    return 1/(x*sigma)*g_normpdfconst*exp(-dx*dx/(2*sigma*sigma));
  }

  mat33 SO3Mean(const std::vector < mat33 > & Rs, const mat & ws, f64 eps)
  {
    using ::s32;
    mat33 res,ires;
    vec3 A;
    res = Rs[0];
    ires = (mat33)res.t();
    f64 error = eps+1;
    s32 nIt = 0;

    s32 n = Rs.size();

    while(error > eps && nIt < 100) {
      A.zeros();
      for(s32 i=0; i<n; i++)
        A = A+ws(0,i)*SO3Log(ires*Rs[i]);
      res = (mat33)(res*SO3Exp(A));
      ires = (mat33)(res.t());
      error = norm(A);
      nIt++;
    }

    return res;
  }

  static f64 g_invSqrt2 = 1/std::sqrt(2.0);
  f64 SO3Distance(const arma::mat33 &R1, const arma::mat33 &R2)
  {
    return g_invSqrt2*norm(SO3Hat(SO3Log(R1.t()*R2)),"fro");
  }

  uvec Sample(const vec &dist, s32 n)
  {
    vec samps = randu(n,1);
    vec sm = cumsum(dist);

    uvec res(n,1);
    for(s32 i=0; i<n; i++)
      res(i) = ((uvec)find(samps(i) <= sm, 1, "first"))(0);
    return res;
  }

  ////////////////////////////////////////////////////////////
  //BEGIN POLYNOMIAL
  ////////////////////////////////////////////////////////////
  Polynomial::Polynomial(const arma::mat &coeffs)
  {
    m_coeffs = coeffs;
  }

  Polynomial::Polynomial(const arma::mat &pts, s32 degree, s32 coord)
  {
    mat vander;
    vec pps = pts.col(coord);
    for(s32 deg=degree; deg>=0; deg--) {
      vander = arma::join_horiz(vander,pow(pps,deg));
    }
    for(s32 i=0; i<coord; i++) 
      m_coeffs = join_vert(m_coeffs, solve(vander, pts.col(i)).t());
    m_coeffs = join_vert(m_coeffs, zeros(1,degree+1));
    for(s32 i=coord+1; i<pps.n_cols; i++) 
      m_coeffs = join_vert(m_coeffs, solve(vander, pts.col(i)).t());

    m_coeffs(coord, degree-1) = 1;
  }

  arma::vec Polynomial::operator()(f64 t) const
  {
    arma::vec evalPt = zeros(m_coeffs.n_cols, 1);

    f64 curr = 1;
    for(s32 i=0; i<m_coeffs.n_cols; i++)  {
      evalPt(m_coeffs.n_cols-1-i) = curr;
      curr = curr*t;
    }

    return m_coeffs*evalPt;
  }
  
  void Polynomial::Print(std::string preamble)
  {
    m_coeffs.print(preamble);
  }

  arma::mat Polynomial::UniformlySpacedPoints(f64 a, f64 b, f64 ds, s32 nTries) const
  {
    mat res;

    vec lastPt = (*this)(a);

    res = join_vert(res, lastPt.t());

    
    f64 currT = (b-a)/2+a;
    f64 lastT = a;
    f64 dist = norm((*this)(currT)-lastPt);
    f64 delta = 0;

    f64 tol = 1.0/10.0;

    s32 it = 0;
    while(currT < b) {
      while(it++ < nTries && abs(dist-ds)/ds > tol) {
        currT = ds/dist*(currT-lastT)+lastT;
        dist = norm((*this)(currT)-lastPt);
      }
      lastPt = (*this)(currT);
      res = join_vert(res, lastPt.t());
      delta = currT-lastT;
      currT = currT+delta;
      lastT = lastT+delta;
      dist = norm((*this)(currT)-lastPt);
      it = 0;
    }

    return res;
  }
  ////////////////////////////////////////////////////////////
  //END POLYNOMIAL
  ////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////
  // BEGIN PLANE
  ////////////////////////////////////////////////////////////
  Plane::Plane(arma::mat abcd)
  {
    m_abcd = abcd;
    m_abcd = m_abcd/norm(m_abcd);
  }

  f64 Plane::PlanePointDistance(Vec3d point)
  {
    return PlanePointDistance(point.ToArmaVec());
  }

  f64 Plane::PlanePointDistance(arma::vec3 point)
  {
    m_abcd = m_abcd/norm(m_abcd.submat(span(0,2),span(0,0)));
    return dot(m_abcd.submat(span(0,2),span(0,0)), point)+m_abcd(3,0);
  }

  Vec3d Plane::ProjectPointOntoPlane(Vec3d point)
  {
    arma::mat n = m_abcd.submat(span(0,2), span(0,0));
    n = n/norm(n);

    arma::vec center = Vec3d(0,0,-m_abcd(3,0)/m_abcd(2,0)).ToArmaVec();

    arma::vec pt = point.ToArmaVec();

    f64 dist = dot((pt-center),n);
    arma::vec result = pt-dist*n;
    return Vec3d::FromArmaVec(result);
  }

  void Plane::GetCenterAndAxisVectors(Vec3d &center, Vec3d &axis1, Vec3d &axis2, Vec3d corner1, Vec3d corner2, Vec3d corner3)
  {
    center = Vec3d(0,0, -m_abcd(3,0)/m_abcd(2,0));

    corner1 = ProjectPointOntoPlane(corner1);
    corner2 = ProjectPointOntoPlane(corner2);
    corner3 = ProjectPointOntoPlane(corner3);
    
    axis1 = corner2-corner1;
    axis2 = corner3-corner1;

    Vec3d n = Vec3d::FromArmaVec((vec3)(m_abcd.rows(arma::span(0,2))));
    n = n.normalized();

    axis2 = n.cross(axis1.normalized());
    axis2 = axis2*(corner3-corner1).magnitude();
    if((corner3-(corner1+axis2)).magnitude() > (corner3-(corner1-axis2)).magnitude())
      axis2 = axis2*-1.0;
    center = corner1+(axis1)/2.0+(axis2)/2.0;
  }

  void Plane::GetCornerAndAxisVectors(Vec3d &corner, Vec3d &axis1, Vec3d &axis2, Vec3d corner1, Vec3d corner2, Vec3d corner3)
  {
    corner1 = ProjectPointOntoPlane(corner1);
    corner2 = ProjectPointOntoPlane(corner2);
    corner3 = ProjectPointOntoPlane(corner3);
    
    axis1 = corner2-corner1;
    axis2 = corner3-corner1;
    
    axis1 = corner2-corner1;
    axis2 = corner3-corner1;

    Vec3d n = Vec3d::FromArmaVec((vec3)(m_abcd.rows(arma::span(0,2))));
    n = n.normalized();

    axis2 = n.cross(axis1.normalized());
    axis2 = axis2*(corner3-corner1).magnitude();
    if((corner3-(corner1+axis2)).magnitude() > (corner3-(corner1-axis2)).magnitude())
      axis2 = axis2*-1.0;
    corner = corner1;
  }
  
  bool Plane::IsZero()
  {
    return norm(m_abcd) < 1e-5;
  }

  ////////////////////////////////////////////////////////////
  // END PLANE
  ////////////////////////////////////////////////////////////


}