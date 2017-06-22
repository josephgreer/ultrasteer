function R = KinematicParametersToTipFrame(phi,theta,l)
axis = Rz(phi);
axis = axis(:,2)*theta;
R = SO3Exp(axis);
end