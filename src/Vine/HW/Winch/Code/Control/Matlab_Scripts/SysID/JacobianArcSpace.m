% ks = [pressure of sPAMs; spring stiffness of main channel; torsional spring stiffness] \in R^{n_actuators+2}
% leqs = equilibrium lengths of springs \in R^{n_actuators+1}
function [J,theta,l,phi] = JacobianArcSpace(ls, ks, alpha, leqs, d, psis)
[theta,l,phi] = lengthsToLThetaPhiNonUniform(ls,d,psis);
psi_1 = psis(1);
psi_2 = psis(2);
psi_3 = psis(3);

l_1 = ls(1);
l_2 = ls(2);
l_3 = ls(3);

p_1 = ks(1);
p_2 = ks(2);
p_3 = ks(3);

k_main = ks(4);
kappa = ks(5);

l_1eq = leqs(1);
l_2eq = leqs(2);
l_3eq = leqs(3);
l_eq = leqs(4); 


J1 = [...
    -d*theta*sin(phi - psi_1),      d*cos(phi - psi_1),           -1,                             1,                             0,                             0;...
    -d*theta*sin(phi - psi_2),      d*cos(phi - psi_2),           -1,                             0,                             1,                             0;...
    -d*theta*sin(phi - psi_3),      d*cos(phi - psi_3),           -1,                             0,                             0,                             1;...
                            0,                       0, k_main/alpha,               (p_1)/alpha,               (p_2)/alpha,               (p_3)/alpha;...
 (kappa*theta*cos(phi))/alpha,  (kappa*sin(phi))/alpha,            0, -(d*p_1*sin(psi_1))/alpha, -(d*p_2*sin(psi_2))/alpha, -(d*p_3*sin(psi_3))/alpha;...
 (kappa*theta*sin(phi))/alpha, -(kappa*cos(phi))/alpha,            0,  (d*p_1*cos(psi_1))/alpha,  (d*p_2*cos(psi_2))/alpha,  (d*p_3*cos(psi_3))/alpha];
  
  
J2 = [...
                                                                                                                                                                                                                                                                                                            0,                                             0,                                             0,                                             0;...
                                                                                                                                                                                                                                                                                                            0,                                             0,                                             0,                                             0;...
                                                                                                                                                                                                                                                                                                            0,                                             0,                                             0,                                             0;...
                                                    - (k_main*(l - alpha*l_eq))/alpha^2 - (k_main*l_eq)/alpha - (l_1eq*p_1)/alpha - (l_2eq*p_2)/alpha - (l_3eq*p_3)/alpha - (p_1*(l_1 - alpha*l_1eq))/alpha^2 - (p_2*(l_2 - alpha*l_2eq))/alpha^2 - (p_3*(l_3 - alpha*l_3eq))/alpha^2,               ((l_1 - alpha*l_1eq))/alpha,               ((l_2 - alpha*l_2eq))/alpha,               ((l_3 - alpha*l_3eq))/alpha;...
 (d*p_1*sin(psi_1)*(l_1 - alpha*l_1eq))/alpha^2 - (kappa*theta*sin(phi))/alpha^2 + (d*p_2*sin(psi_2)*(l_2 - alpha*l_2eq))/alpha^2 + (d*p_3*sin(psi_3)*(l_3 - alpha*l_3eq))/alpha^2 + (d*l_1eq*p_1*sin(psi_1))/alpha + (d*l_2eq*p_2*sin(psi_2))/alpha + (d*l_3eq*p_3*sin(psi_3))/alpha, -(d*sin(psi_1)*(l_1 - alpha*l_1eq))/alpha, -(d*sin(psi_2)*(l_2 - alpha*l_2eq))/alpha, -(d*sin(psi_3)*(l_3 - alpha*l_3eq))/alpha;...
 (kappa*theta*cos(phi))/alpha^2 - (d*p_1*cos(psi_1)*(l_1 - alpha*l_1eq))/alpha^2 - (d*p_2*cos(psi_2)*(l_2 - alpha*l_2eq))/alpha^2 - (d*p_3*cos(psi_3)*(l_3 - alpha*l_3eq))/alpha^2 - (d*l_1eq*p_1*cos(psi_1))/alpha - (d*l_2eq*p_2*cos(psi_2))/alpha - (d*l_3eq*p_3*cos(psi_3))/alpha,  (d*cos(psi_1)*(l_1 - alpha*l_1eq))/alpha,  (d*cos(psi_2)*(l_2 - alpha*l_2eq))/alpha,  (d*cos(psi_3)*(l_3 - alpha*l_3eq))/alpha];

J = -inv(J1)*J2;
end