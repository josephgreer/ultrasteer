function [tip,T,v1needle,v2needle,v1target,v2target,n_needle_c,...
    n_target_c,theta,R,poserr_c,negerr_c,n_radial_c] = readVectorFile(dir)
%READVECTORFILE Reads the vector and point data saved by Ultrasteer, as
%well as the resulting angle and duty-cycle commands

rawVectors = csvread([dir 'vectors.txt']);
% display(size(rawVectors))
tip = rawVectors(1:3)';
T = rawVectors(4:6)';
v1target = rawVectors(7:9)';
v2target = rawVectors(10:12)';
v1needle = rawVectors(13:15)';
v2needle = rawVectors(16:18)';
n_needle_c = rawVectors(19:21)';
n_target_c = rawVectors(22:24)';
n_radial_c = rawVectors(25:27)';
poserr_c = rawVectors(28:30)';
negerr_c = rawVectors(31:33)';
theta = rawVectors(34)';
R = rawVectors(35)';

end

