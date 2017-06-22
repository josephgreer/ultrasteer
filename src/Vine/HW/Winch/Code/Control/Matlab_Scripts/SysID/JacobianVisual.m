% ks = [pressure of sPAMs; spring stiffness of main channel; torsional spring stiffness] \in R^{n_actuators+2}
% leqs = equilibrium lengths of springs \in R^{n_actuators+1}
function J = JacobianVisual(cameraParams,ls, ks, alpha, leqs, d, psis)
[JCart,theta,l,phi] = JacobianCartesian(ls,ks,alpha,leqs,d,psis);
[im,Z] = KinematicParametersToImagePoint(phi,theta,l,cameraParams);
tip = kinematicParametersToTipPoint(l,phi,theta,eye(3), zeros(3,1));

 xy = im/cameraParams.focalLength;
 x = xy(1); y = xy(2);
 
 L = cameraParams.focalLength*[...
      -1/Z  0   x/Z  x*y  -(1+x^2)  y;...
      0   -1/Z  y/Z  1+y^2  -x*y    -x];

axis = Rz(phi);
axis = axis(:,2)*theta;
R = SO3Exp(axis);

V_n = [R.'              zeros(3,3);...
       zeros(3,3)       R.'];
   
J = L*V_n*JCart;
end