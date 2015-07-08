clear; clc; close all;

addpath('../');

rng('default');
rng(1);

R1 = QuatToRotationMatrix(AxisAngleToQuat(rand(3,1)*pi));
R2 = QuatToRotationMatrix(AxisAngleToQuat(rand(3,1)*pi));

save('./data/testSO3DistanceR1.dat', 'R1', '-ascii');
save('./data/testSO3DistanceR2.dat', 'R2', '-ascii');

SO3Metric(R1,R2)