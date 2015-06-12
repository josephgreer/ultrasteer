%%%%%%%%%%%%%%%%%% Tests SO(3) functions
clear; clc; close all;
eps = 1e-8;
for i=1:20
    % exp map is injective for vectors of magnitude less than pi
    mag = pi;
    w = 2*rand(3,1)-ones(3,1);
    w = w/norm(w)*mag*rand(1);
    q = AxisAngleToQuat(w);
    R = QuatToRotationMatrix(q);
    
    R1 = SO3Exp(w);
    
    assert(norm(R1-R) < eps);
    
    w1 = SO3HatInverse(SO3Log(R1));
    w;
    norm(w)
    assert(norm(w-w1) < eps);
end



% test SO(3) mean

N = 5000;
orientationNoise = mvnrnd(zeros(3,1), diag(2*[pi/2, pi/2, pi/2]), N);
baseOrientation = AxisAngleToQuat(rand(3,1)*50);
qs = zeros(N, 4);
for i=1:size(orientationNoise,1)
    qs(i,:) = quatmult(baseOrientation, AxisAngleToQuat(orientationNoise(i,:)'))';
end
R = SO3Mean(qs, ones(size(qs,1),1)/size(qs,1), 1e-4)
QuatToRotationMatrix(baseOrientation)


