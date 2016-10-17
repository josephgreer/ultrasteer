clear; clc; close all;

fname = '../SysID/Data/Pouch/PouchData_5_8.txt';

data = load(fname);

l0 = 5.75;

plot(data(:,1)/l0, 1-(data(:,2)/l0));