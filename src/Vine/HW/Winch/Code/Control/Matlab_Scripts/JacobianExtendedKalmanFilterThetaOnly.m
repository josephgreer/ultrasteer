% state xt = [theta0]
function [xt, Et] = JacobianExtendedKalmanFilter(zt, xt1, dq, Et1, R, Q, nActuators, deltaTheta)
% propagate state
xtbar = xt1(end);
s = xt1(1:nActuators);
theta0 = xtbar(end);

Et1 = Et1(end,end);
R = R(end,end);

Etbar = Et1+R;

Ht = zeros(2,1);

cosSin = [cos(theta0+deltaTheta.'); sin(theta0+deltaTheta.')];
Ht(1,end) = -cosSin(2,:)*(s.*dq);
Ht(2,end) = cosSin(1,:)*(s.*dq);


xt = zeros(nActuators+1,1);

hxtbar = FormJacobian(xt1,deltaTheta,nActuators)*dq;
Kt = Etbar*Ht.'*inv(Ht*Etbar*Ht.'+Q);
xt(end) = xtbar+Kt*(zt-hxtbar);
xt(end) = wrapToPi(xt(end));
xt(1:nActuators) = s;
Et = (eye(size(Etbar))-Kt*Ht)*Etbar;
end