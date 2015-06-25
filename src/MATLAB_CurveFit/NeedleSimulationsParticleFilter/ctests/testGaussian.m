clear; clc; close all;
addpath('../');

% tests C++ generation of multivariate gaussian samples
% first run Math, Gaussian unit test to generate
% bulksamples and individualsamples.dat
% bulksamples tests ability to generate N samples at once
% individual sampels tests ability to generate 1 sample N times
% all 3 plots should look similar.


V = SO3Exp(SO3Hat([1; 3; 4]));
sigma = diag([100; 3; 2]);

N = 1000;

mu = [-1; 2; 3];

sigma = V*sigma*V';


samps = mvnrnd(mu, sigma, N);

scatter3(samps(:,1),samps(:,2), samps(:,3));
axis equal;
hold on;

load('./data/bulksamples.dat');
load('./data/individualsamples.dat');
scatter3(bulksamples(:,1),bulksamples(:,2), bulksamples(:,3),'r');
scatter3(individualsamples(:,1),individualsamples(:,2), individualsamples(:,3),'g');


