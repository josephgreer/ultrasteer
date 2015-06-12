% clear; clc; close all;
% %P = peaks(20);
% P = meshgrid([-1:0.05:1], [-1:0.05:1]);
% sphere3d(P,-pi,pi,-pi/2,pi/2,20,.4,'surf','spline', 1e-3);
% colormap jet
% grid off;
% 

clear; clc; close all;

sigma = 0.2*diag([1 1 1]);%Rx(pi/8)*diag(1e-0*[0.2 0.01 0.2])*(Rx(pi/8)');

N = SO3Gaussian(Ry(-pi/2), sigma);
N.drawit(1,100);
view([-90 0]);