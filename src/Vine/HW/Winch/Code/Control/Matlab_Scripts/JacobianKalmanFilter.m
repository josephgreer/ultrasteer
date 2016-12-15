% state xt = [s_1,...,s_n,nActuators,theta0]
function [xt, Et] = JacobianKalmanFilter(zt, xt1, dq, Et1, R, Q, nActuators)
% propagate state
xtbar = xt1;

Etbar = Et1+R;

Ct = zeros(2,2*nActuators);
for i=1:nActuators
    Ct(1:2,2*(i-1)+1:2*i) = eye(2,2)*dq(i);
end

Kt = Etbar*Ct.'*inv(Ct*Etbar*Ct.'+Q);

xt = xtbar+Kt*(zt-Ct*xtbar);
Et = (eye(size(Etbar))-Kt*Ct)*Etbar;
end