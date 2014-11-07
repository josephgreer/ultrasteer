% -------------------------------------------------------------------------
% author: Troy Adebar date: 2012-05-31
%
% title: Manual_Segmentation
%
% summary: This .m file allows a user to manually segment needle cross
% sections from an ultrasound volume, in order to validate the automatic
% segmentation
% -------------------------------------------------------------------------
%% Load Data
clear all; clc; close all;
% define constants
order = 3; % for curve fitting
% -------------------------------------------------------------------------
% - - - - - - -  Get these values from propello window! - - - - - - - - - -
% -------------------------------------------------------------------------
for test = 1:4
basepath = 'Needle Configuration\curved\';
% basepath = 'Needle Diameter\580 microns\';
% basepath = 'Vibration Frequency\med\';
% basepath = 'Lateral Position\lateral\';
specificpath = [num2str(test),'\'];
pathname = [basepath specificpath];
filename = '1_auto_postscan.vol';
Dfilename = '1_auto_colorpost.vol';
datapath = [pathname,filename]; % The file saved in propello
whichVol = 1; % Of the volumes saved in datapath, which one will be read in
degPerFr = 0.731707; % Angular increment (deg) between frames
imgDepth = 80; % Imaging depth in mm
probeR3D_mm = 27.5; % 3D probe radius in mm
micr_x = 161; % Microns per image pixel in the x direction
micr_y = 161; % Microns per image pixel in the y direction
o_x = 241; % Position of the image origin in the x direction
o_y = 55; % Position of the image origin in the y direction
% -------------------------------------------------------------------------
% - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
% -------------------------------------------------------------------------
% load the volume information and data from the file
[volume, hdr] = loadvol(datapath, whichVol);
[Dvolume, Dhdr] = loadvol([pathname,Dfilename], whichVol);
%	volume = the image data in a 3D matrix
%	datatype = type of data (0 = pre-scan, 1 = post-scan, 2 = rf,
%       3 = color velocity/variance, 4 = color post-scan)
%	numVols = volumes inside the file
% 	fpv = frames per volume
%   w = width of a frame (pixels for post-scan, scanlines for pre-scan or
%       rf data)
%	h = height of frame (pixels for post-scan, samples for pre-scan or rf
%       data)
% 	ss = sample size in bits

if(hdr.datatype ~= 1) % Check that it's post-scan data as expected
    display('Error: Data not read in as post-scan B')
else
    man_x_pix = []; man_y_pix = []; man_slices = [];
    % Loop through slices in the volume and search for needle
    for slice = 5:hdr.fpV
        %% Display the images
        % Display the color image for reference
        [m_RGB_image, m_Color_image] = separatecolor(Dvolume(:,:,slice)');
        figure(2); imagesc(m_RGB_image); axis equal; axis off; hold on;
        titlestr = sprintf('Doppler data'); 
        t = title(titlestr); set(t,'Color','k')
        % Display the grayscale image
        m_Gray_image = volume(:,:,slice)';
        figure(1); hold off; imagesc(m_Gray_image);
        set(gcf,'Color','k');axis equal; axis off; hold on; colormap(gray);
        %% Let user manually locate needle
        % Ask them to select it
        titlestr = sprintf('Locate needle in slice %d, or click black',slice);
        t = title(titlestr);
        set(t,'Color','w');
        % Take the selection and save it
        [usr_x, usr_y] = getpts;
        % If that's not a black spot, save it into the vector
        if(usr_y)
            if(m_Gray_image(round(usr_y(1)),round(usr_x(1))))
                man_x_pix = [man_x_pix usr_x(1)];
                man_y_pix = [man_y_pix usr_y(1)];
                man_slices = [man_slices slice];
            end
        end
    end
    save([pathname 'manual_segmentation'],'man_y_pix','man_x_pix','man_slices')
end
end

