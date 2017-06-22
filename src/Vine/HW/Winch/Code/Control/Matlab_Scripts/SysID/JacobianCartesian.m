% ks = [pressure of sPAMs; spring stiffness of main channel; torsional spring stiffness] \in R^{n_actuators+2}
% leqs = equilibrium lengths of springs \in R^{n_actuators+1}
function [J,theta,l,phi] = JacobianCartesian(ls, ks, alpha, leqs, d, psis)
[JArc,theta,l,phi] = JacobianArcSpace(ls,ks,alpha,leqs,d,psis);


t2 = sin(phi);
t11 = l.*t2;
t3 = abs(t11);
t9 = cos(phi);
t14 = l.*t9;
t4 = abs(t14);
t5 = abs(theta);
t6 = 1.0./t5.^2;
t7 = l.^2;
t8 = 1.0./theta.^2;
t10 = cos(theta);
t12 = t3.^2;
t13 = t6.*t12;
t15 = t4.^2;
t16 = t6.*t15;
t17 = t13+t16;
t18 = 1.0./sqrt(t17);
t19 = 1.0./t5.^3;
t20 = sign(theta);
t21 = 1.0./t17.^(3.0./2.0);
t22 = 1.0./theta;
t23 = sign(t14);
t24 = sign(t11);
t25 = l.*t2.*t4.*t6.*t23.*2.0;
t26 = t25-l.*t3.*t6.*t9.*t24.*2.0;
t27 = 1.0./theta.^3;
t28 = sin(theta);
t29 = t15.*t19.*t20.*2.0;
t30 = t12.*t19.*t20.*2.0;
t31 = t29+t30;
t32 = t4.*t6.*t9.*t23.*2.0;
t33 = t2.*t3.*t6.*t24.*2.0;
t34 = t32+t33;
J3 = reshape([-l.*t2.*t22+t2.*t7.*t8.*t10.*t18-t7.*t8.*t9.*t10.*t21.*t26.*(1.0./2.0),l.*t9.*t22-t7.*t8.*t9.*t10.*t18-t2.*t7.*t8.*t10.*t21.*t26.*(1.0./2.0),0.0,-l.*t8.*t9+t7.*t8.*t9.*t18.*t28+t7.*t9.*t10.*t18.*t27.*2.0-t7.*t8.*t9.*t10.*t21.*t31.*(1.0./2.0),-l.*t2.*t8+t2.*t7.*t8.*t18.*t28+t2.*t7.*t10.*t18.*t27.*2.0-t2.*t7.*t8.*t10.*t21.*t31.*(1.0./2.0),l.*t10.*t22-l.*t8.*t28,t9.*t22-l.*t8.*t9.*t10.*t18.*2.0+t7.*t8.*t9.*t10.*t21.*t34.*(1.0./2.0),t2.*t22-l.*t2.*t8.*t10.*t18.*2.0+t2.*t7.*t8.*t10.*t21.*t34.*(1.0./2.0),t22.*t28,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0],[3, 6]);

J = J3*JArc;

t2 = cos(phi);
t3 = t2.^2;
t4 = cos(theta);
t5 = t4-1.0;
t6 = sin(phi);
t7 = sin(theta);
t8 = t3.*t5;
t9 = t6.^2;
t10 = t8-t5.*t9;
t11 = t6.*t7;
Jtemp = reshape([t2.*t5.*t6.*-2.0,t10,t11,t10,t2.*t5.*t6.*2.0,-t2.*t7,-t11,t2.*t7,0.0,-t3.*t7,-t2.*t6.*t7,-t2.*t4,-t2.*t6.*t7,-t7.*t9,-t4.*t6,t2.*t4,t4.*t6,-t3.*t7-t7.*t9,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0],[9, 6]);

axis = Rz(phi);
axis = axis(:,2)*theta;
R = SO3Exp(axis);


J4 = zeros(3,6);
for i=1:6
    Rid = reshape(Jtemp(:,i),3,3);
    omegai = Rid*R.';
    omegai = SO3HatInverse(omegai);
    J4(:,i) = omegai;
end

J = vertcat(J,J4*JArc);


end