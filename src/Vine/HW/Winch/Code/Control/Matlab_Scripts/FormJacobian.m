function [J,actuatorThetas] = FormJacobian(x, deltaTheta, nActuators)
strengths = StateToStrengths(x,nActuators);
firstAct = StateToTheta0(x);
actuatorThetas = firstAct+deltaTheta;
J = [];
for i=1:nActuators
    J = [J strengths(i)*[cos(actuatorThetas(i)); sin(actuatorThetas(i))]];
end
end