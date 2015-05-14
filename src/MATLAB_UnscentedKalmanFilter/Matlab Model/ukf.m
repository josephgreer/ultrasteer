function [xnew,Pnew,K]=ukf(x,P,Q,R,u,z)
% UKF   Unscented Kalman Filter for nonlinear dynamic functions of
% orientation [x, P] = ukf(f,x,P,h,z,Q,R) returns state estimate, x and
% state covariance, P for nonlinear dynamic system:
%           x_k+1 = f(x_k,u_k,w_k) 
%           z_k   = g(x_k,v_k)
% where w ~ N(0,Q) meaning w is gaussian noise with covariance Q
%       v ~ N(0,R) meaning v is gaussian noise with covariance R
% Inputs:   x: previous estimate mean (4x4)
%           P: previous estimate covariance (6x6)
%           Q: process noise covariance (6x6)
%           R: measurement noise covariance (6x6)
%           u: current control input (3x1)
%           z: current measurement (4x4)
% Output:   x: updated estimate mean (4x4)
%           P: updated estimate covariance (6x6)

% define number of states
n=6;                    
% identify sigma points around x; include process covariance
[X] = sigmas(x,P+Q,n); 
% unscented transformation of sigma points (process)
[x_,~,P_,Ex_] = utf(X,u);  
% resample sigma points; include measurement covariance
[X_] = sigmas(x_,P_+R,n); 
% unscented transformation sigma points (measurement)
[z_,~,Pzz,Ez_] = utg(X_);               
% transformed cross-covariance
Pxz = zeros(6,6);
for i = 1:2*n
    Pxz = Pxz + (1/2/n).*Ex_(:,:,i)*Ez_(:,:,i)';
end
% Kalman gain
K = Pxz*inv(Pzz); 
% covariance update
Pnew = P_-K*Pzz*K';  
% state update
zd = [  z(1:3,4) - z_(1:3,4);
        mat2vec( z(1:3,1:3)*inv(z_(1:3,1:3)) ) ];
xd = K*zd;
xnew(1:3,4) = x_(1:3,4) + xd(1:3);
xnew(1:3,1:3) = x_(1:3,1:3) * vec2mat( xd(4:6) );
xnew(4,4) = 1;
end


function [y,Y,P,E]=utf(X,u)
%Unscented Transformation of Process
%Inputs:
%        X: sigma points
%        u: input vector
%Outputs:
%        y: transformed mean
%        Y: transformed sigma points
%        P: transformed covariance
%        E: transformed deviations

n=size(X,3);
Y=zeros(4,4,n);
for k=1:n                   
    Y(:,:,k)=f(X(:,:,k),u,zeros(6,1));       
end
[y,P,E] = meancov(Y);
end


function [y,Y,P,E]=utg(X)
%Unscented Transformation
%Inputs:
%        X: sigma points
%Outputs:
%        y: transformed mean
%        Y: transformed sigma points
%        P: transformed covariance
%        E: transformed deviations

n=size(X,3);
Y=zeros(4,4,n);
for k=1:n                   
    Y(:,:,k)=g(X(:,:,k),zeros(6,1));       
end
[y,P,E] = meancov(Y);
end


function [X]=sigmas(x,P,n)
%Sigma points around reference point
%Inputs:
%       x: reference point      (4 x 4)
%       P: covariance           (6 x 6)
%       n: dimension      
%Outputs:
%       X: Sigma points         (4 x 4 x 2n+1)

P = validateCovMatrix(P);
[A,p] = chol(P);
if( p )
    display('Error, P is not positive definite')
end
A = sqrt(n)*[A' -A'];
for i = 1:2*n
    px = x(1:3,4) + A(1:3,i);
    Rx = x(1:3,1:3)*vec2mat(A(4:6,i));
    X(:,:,i) = homoT(Rx,px);    
end
end

