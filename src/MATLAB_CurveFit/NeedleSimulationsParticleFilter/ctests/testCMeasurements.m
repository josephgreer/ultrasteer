clear; clc; close all;
addpath('../');

params = initParamsForSimulation();
rng('default');
rng(1);

basePath = './data/testCMeasure';
paths = {