
% test SO(3) mean
clear; clc; close all;
N = 5000;
orientationNoise = mvnrnd(zeros(3,1), diag(1/2*[pi/2, pi/2, pi/2]), N);
baseOrientation = AxisAngleToQuat(rand(3,1)*50);
qs = zeros(N, 4);
for i=1:size(orientationNoise,1)
    qs(i,:) = quatmult(baseOrientation, AxisAngleToQuat(orientationNoise(i,:)'))';
end


Rs = zeros(9, N+1);
for i=1:N
    Rc = QuatToRotationMatrix(qs(i,:)');
    Rs(:,i) = Rc(:);
end

R = SO3Mean(qs, ones(size(qs,1),1)/size(qs,1), 1e-4)
Rs(:,N+1) = R(:);
save('./data/testSO3MeanRs.dat', 'Rs', '-ascii');
QuatToRotationMatrix(baseOrientation)