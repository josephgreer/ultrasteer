clear; clc; close all;

addpath('../../../../../../../MATLAB_CurveFit/NeedleSimulationsParticleFilter/');
syms alpha d psi_1 psi_2 psi_3 phi l l_1 l_2 l_3 l_eq l_1eq l_2eq l_3eq kappa p_1 p_2 p_3 k_1 k_2 k_3 k_main theta real

% input order
% alpha,p_1,p_2,p_3
%output order
% \phi, \theta, l, l_1, l_2, l_3

outputs = [phi; theta; l; l_1; l_2; l_3];
inputs = [alpha; p_1; p_2; p_3];

f = sym('f',[6 1]);

psis = [psi_1; psi_2; psi_3];
phis = psis-phi;
ls = [l_1; l_2; l_3];
leqs = alpha*[l_1eq; l_2eq; l_3eq; l_eq];
ks = [k_1*p_1; k_2*p_2; k_3*p_3; k_main]/alpha;

% geometric constraints
f(1:3) = ls+theta*d*cos(phis)-l;

l_minus_leq = [ls;l]-leqs;

% sum of forces equal 0
f(4) = ks.'*l_minus_leq;

turn_dir = Rz(phi);
turn_dir = turn_dir(1:2,2);

% sum of moments
f(5:6) = -(kappa/alpha*theta)*turn_dir;

for i=1:3
    act_dir = Rz(psis(i)) ;
    act_dir = act_dir(:,1);
    r = d*act_dir;
    moment = cross(r,ks(i)*l_minus_leq(i)*[0; 0; -1]);
    f(5:6) = f(5:6)+moment(1:2);
end

J1 = [];
for i=1:length(outputs)
    J1 = horzcat(J1,diff(f,outputs(i)));
end

J2 = [];
for i=1:length(inputs)
    J2 = horzcat(J2,diff(f,inputs(i)));
end


% l = alpha*l;
% theta = alpha*theta;
r = l/theta;
cen_of_curve = r*[cos(phi); sin(phi); 0];
delta = -cen_of_curve; delta = delta/norm(delta);
tip = r*(cos(theta)*delta+sin(theta)*[0;0;1])+cen_of_curve;

J3 = [];
for i=1:length(outputs)
    J3 = horzcat(J3,diff(tip,outputs(i)));
end

axis = Rz(phi);
axis = axis(:,2)*theta;
hat = @(x)([0 -x(3) x(2); x(3) 0 -x(1); -x(2) x(1) 0]);
K = hat(axis/theta);
RR = eye(3)+sin(theta)*K+(1-cos(theta))*K*K;
RR = RR(:);
J4 = [];
for i=1:length(outputs)
    J4 = horzcat(J4,diff(RR,outputs(i)));
end




