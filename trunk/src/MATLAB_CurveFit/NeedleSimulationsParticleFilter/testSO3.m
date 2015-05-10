%%%%%%%%%%%%%%%%%% Tests SO(3) functions

eps = 1e-8;
for i=1:20
    % exp map is injective for vectors of magnitude less than pi
    w = rand(3,1)*2*pi-pi/2
    q = AxisAngleToQuat(w);
    R = QuatToRotationMatrix(q)
    
    R1 = SO3Exp(w)
    
    assert(norm(R1-R) < eps)
    
    w1 = SO3HatInverse(SO3Log(R1))
    w
end



% test SO(3) mean

orientationNoise = mvnrnd(zeros(3,1), diag([pi/2, pi/2, pi/2]), 2000);
baseOrientation = AxisAngleToQuat(rand(3,1)*50);
qs = zeros(500, 4);
for i=1:size(orientationNoise,1)
    qs(i,:) = quatmult(baseOrientation, AxisAngleToQuat(orientationNoise(i,:)'))';
end
R = SO3Mean(qs, 1e-6)
QuatToRotationMatrix(baseOrientation)


