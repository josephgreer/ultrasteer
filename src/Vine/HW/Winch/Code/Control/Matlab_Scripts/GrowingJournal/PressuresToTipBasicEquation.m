function [tip, RR, theta, l, phi] = PressuresToTipBasicEquation(pressures, kmults, kmain, ktor, leq, radius, R0, pos0, n_actuators)
ks = kmults.*pressures;
ks(4) = kmain;
ks(5) = ktor;

system = @(l)(SpringSystemEquations(l, ks, leq, radius));
[ls, ~] = fsolve(system, leq(1:n_actuators));
ls = real(ls);
phi = lengthsToPhi(ls);
l = mean(ls);
kappa = lengthsToKappa(ls,radius);
kappa = real(kappa);
theta = kappa*l;


RR = Rz(phi);
RR = R0*QuatToRotationMatrix(AxisAngleToQuat(RR(:,2)));

tip = real(kinematicParametersToTipPoint(l,phi,theta,R0,pos0));
 end