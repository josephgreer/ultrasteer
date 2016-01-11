clear; clc; close all;

addpath('../');
params = initParamsForSimulation;

mu = SO3HatInverse(SO3Log(Rz(0*pi/180)*Rx(90*pi/180)*Rz(100*pi/180)));
priorGaussian = SO3Gaussian(mu, diag([0.075 0.075 0.075]));
muMeasurement = SO3HatInverse(SO3Log(Rz(0*pi/180)*Rx(90*pi/180)*Rz(45*pi/180)*Rx(-30*pi/180)));
measMeasGaussian = SO3Gaussian(mu, diag([0.01 0.01 0.5]));
measGaussian = SO3Gaussian(muMeasurement, diag([0.01 0.01 0.5]));

figHandle = figure(1);
h1 = subplot(1,4,1);
drawdensity(priorGaussian, 1, 120);
h2 = subplot(1,4,2);
drawdensity(measMeasGaussian, 1, 120);
h3 = subplot(1,4,3);
drawdensity(measGaussian, 1, 0);

K = priorGaussian.sigma*inv(priorGaussian.sigma+measGaussian.sigma);
sigmaC = (eye(3)-K)*priorGaussian.sigma;

v = SO3HatInverse(SO3Log(SO3Exp(mu)'*SO3Exp(muMeasurement)));
Rc = SO3Exp(mu)*SO3Exp(K*v);

postGaussian = SO3Gaussian(SO3HatInverse(SO3Log(Rc)), sigmaC);
h4 = subplot(1,4,4);
drawdensity(postGaussian, 1, 120);

set(figHandle, 'Position', [100 100 720 150]);
tightfig;

zfactor = 1.9;
zoom(h1, zfactor);
zoom(h2, zfactor);
zoom(h3, zfactor);
zoom(h4, zfactor);
export_fig -transparent SO3Density.pdf