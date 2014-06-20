% Portable Needle Steering Project
% DICOM File Reader
% Author: Ann Majewicz


clear all
close all
clc
addpath('./Functions');

folder = 'C:\Users\charm\Desktop\Needle Steering\06_11_14 Cadaver Study\Data\DYNACT_HEAD_NAT_FILL_HU_NORMAL_[INSPACE3D]_0054';

file = dir(folder);
for i = 3:length(file)
    x = dicomread(strcat(folder,'/',file(i).name));
    data(:,:,i) = x;
   % image = x;
  %  figure;
  %  imshow(image);
end
%figure
isosurface(data)
%figure
%movie(M)

