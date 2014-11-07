function [C_impts] = readimPts(dir)
%READSCPTS Reads the scan-converted points segmented using C++ ITK/VTK on
%the ultrasound machine

rawSC = csvread([dir 'CDIimpts.txt']);
n = size(rawSC(1:end-1),2)/3;
C_impts = [reshape(rawSC(1:end-1),3,n)]';
end

