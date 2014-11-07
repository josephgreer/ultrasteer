% -------------------------------------------------------------------------
% author: Troy Adebar date: 2012-06-03
% title: Generate_Settings_File
%
% summary: This .m file saves a .mat parameter file with all the settings
% needed to scan convert points.
% -------------------------------------------------------------------------
clear all; close all; clc;
% Define the values
degPerFr = 1.46341; % Angular increment (deg) between frames
fpV = 47;% Frames in a volume
imgDepth = 100; % Imaging depth in mm
probeR3D_mm = 27.5; % 3D probe radius in mm
micr_x = 305; % Microns per image pixel in the x direction
micr_y = 305; % Microns per image pixel in the y direction
o_x = 241; % Position of the image origin in the x direction
o_y = 30; % Position of the image origin in the y direction
m = 448; % Rows in the color image
n = 480; % Cols in the color image

% Save into all the subfolders
cat = {'Curve','Lateral Position','Needle Diameter','Vibration Frequency'};
val = {'curved','straight',[];
    'center','lateral',[];
    '380 microns','480 microns','580 microns';
    'low','med','high'};
% for c = 1:4
%     for v = 1:3
%         for trial = 1:4
%             if(val{c,v})
%                 folder = [cat{c} '/' val{c,v} '/' num2str(trial) '/']
%                 save([folder 'settings'],'degPerFr','fpV','imgDepth',...
%                     'probeR3D_mm','micr_x','micr_y','o_x','o_y','m','n')
%             end
%         end
%     end
% end

folder = 'V:\NeedleScan\9_26_14_MagnetScan\Scan 11\';
save([folder 'settings'],'degPerFr','fpV','imgDepth',...
    'probeR3D_mm','micr_x','micr_y','o_x','o_y','m','n')