function [tip, RR, theta, l, phi] = PressuresToTip(pressures, kmults, kmain, ktor, leqs, gammas, torGamma, radius, psis, R0, pos0)
gammas = gammas.';
pressures = pressures.^gammas;
ks = kmults.*pressures.';
ks(4) = kmain;
ks(5) = ktor;
systemFull = @(x)(FullSpringSystemEquations(x, ks, leqs, radius, psis, torGamma));
[ls, fval] = fsolve(systemFull, leqs(1:3));
[theta,l,phi] = lengthsToLThetaPhiNonUniform(ls,radius,psis);

RR = Rz(phi);
RR = R0*QuatToRotationMatrix(AxisAngleToQuat(RR(:,2)));

tip = kinematicParametersToTipPoint(l,phi,theta,R0,pos0);
 end