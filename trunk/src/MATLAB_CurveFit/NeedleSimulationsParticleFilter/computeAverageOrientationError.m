% compute Average Orientation Error
% x = needle tip state
%   x.pos % position of needle tip frame n timesteps back
%   x.q  % orientation of needle tip frame n timesteps back
%   x.rho radius of curvature (mm)
%   x.w = particle weights
% xcurr = current needle state
% params = simulation parameters
% see ../NeedleSimulation.m for description of parameters
function error = computeAverageOrientationError(x, xcurr, params)
q = particleOrientations(x, params);
pw = particleWeights(x, params);

error = 0;
Rc = QuatToRotationMatrix(xcurr.q);
for i=1:size(q,1)
R = QuatToRotationMatrix(q(i,:)');
error = error + SO3Metric(Rc,R)*pw(i);
end
end