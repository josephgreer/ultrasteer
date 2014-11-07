% -------------------------------------------------------------------------
% author: Troy Adebar date: 2012-06-03
% title: Evaluate_and_Plot
%
% summary: This .m file compares the ITK segmentations of a number of
% needles with manual segmentations
% -------------------------------------------------------------------------
clear all; close all; clc;

cats ={'Curve','Lateral Position','Needle Diameter','Vibration Frequency'};
vals = {{'curved','straight'},{'center','lateral'},...
    {'380 microns','480 microns','580 microns'},{'low','med','high'}};
val_vec = [2 2 3 3];

errorSC = [];
for cat = 1:4
    t_vals = vals{cat};
    for val = 1:val_vec(cat)
        for trial = 1:4
            % load data for the specific trial
            folder = [cats{cat} '/' t_vals{val} '/' num2str(trial) '/'];
            [px_itk,py_itk] = readPolyFile(folder);
            ITK_SCpts = readSCPts(folder);
            ITK_impts = readimPts(folder);
            load([folder 'settings'])
            [mat_x,mat_y,mat_slice] = Segment_Needle(folder);
            % scan convert Matlab image points
            matlab_pts = scanconvertpoints([mat_x,mat_y],...
                mat_slice,degPerFr,fpV,probeR3D_mm,...
                micr_x,micr_y,o_x,o_y,m,n);
            % fit polynomial curves to the Matlab points
            px_mat = polyfit(matlab_pts(:,3),matlab_pts(:,1),3);
            py_mat = polyfit(matlab_pts(:,3),matlab_pts(:,2),3);
            % determine the 2D error between ITK im pts and Matlab im pts
            errorIM = ITK_impts(:,1:2) - [mat_x m-mat_y]
            errorIM2D = sqrt(sum(errorIM.^2,2));
            display(mean(errorIM2D))
            % determine the 3D error between ITK SC pts and Matlab SC pts
            errorSC = ITK_SCpts-matlab_pts;
            errorSC3D = sqrt(sum(errorSC.^2,2));
            display(mean(errorSC3D))
            % display both sets of polynomials
            display([px_mat; px_itk; py_mat; py_itk])
        end
    end
end
