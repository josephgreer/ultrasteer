% state xt = [s_1,...,s_n,nActuators,theta0]
function [xt, Et] = JacobianExtendedKalmanFilter(zt, xt1, dq, Et1, R, Q, nActuators, deltaTheta)
% propagate state
xtbar = xt1;
s = xtbar(1:nActuators);
theta0 = xtbar(end);

Etbar = Et1+R;

Ht = zeros(2,nActuators+1);

cosSin = [cos(theta0+deltaTheta.'); sin(theta0+deltaTheta.')];
Ht(:,1:nActuators) = bsxfun(@times,cosSin,dq.');
Ht(1,end) = -cosSin(2,:)*(s.*dq);
Ht(2,end) = cosSin(1,:)*(s.*dq);

hxtbar = FormJacobian(xtbar,deltaTheta,nActuators)*dq;
Kt = Etbar*Ht.'*inv(Ht*Etbar*Ht.'+Q);
xt = xtbar+Kt*(zt-hxtbar);
xt(end) = wrapToPi(xt(end));
Et = (eye(size(Etbar))-Kt*Ht)*Etbar;
end