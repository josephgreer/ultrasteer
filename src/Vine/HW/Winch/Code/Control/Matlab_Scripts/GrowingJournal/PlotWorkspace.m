clear; clc; close all;

addpath('../../../../../../../MATLAB_CurveFit/NeedleSimulationsParticleFilter/');
addpath('../SysID');

n_actuators = 3;
leq = [65; 65; 65; 100];
ks = [0; 0; 0; 1; 10];
d = 5;
ll = leq;
psis = actuatorThetas();

handles = [];
figure;
hold on;
daspect([1 1 1]);
xlabel('x');
ylabel('y');
zlabel('z');
grid on;

npoints = 10;
pts = linspace(0,1,npoints);

[ps1, ps2, ps3] = meshgrid(pts,pts,pts);
ps1 = ps1(:);
ps2 = ps2(:);
ps3 = ps3(:);


kmults = [2.5; 2.5; 2.5];
kmain = 2.5;
ktor = 32;

for i=1:length(ps1)
    display(i);
    ps = [ps1(i); ps2(i); ps3(i)];
    [pos, ~, ~, ~, ~] = PressuresToTipBasicEquation(ps, kmults, kmain, ktor, leq, d, eye(3,3), zeros(3,1), n_actuators);
    scatter3(pos(1), pos(2), pos(3), 'b');
end
