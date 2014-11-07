% -------------------------------------------------------------------------
% author: Troy Adebar
%
% -------------------------------------------------------------------------
%% Initialize
clear all; clc;
%% Process data
fol = 'Needle Diameter/480 microns/';
% fol = '../Radius of Curvature/480mic_med_bend/'
for trial = 3
    % load data for the specific trial
    datapath = [fol num2str(trial) '/' '1_auto_postscan.vol']
    Ddatapath = [fol num2str(trial) '/' '1_auto_colorpost.vol']
    % load the volume information and data from the file
    [volume, hdr] = loadvol(datapath, 1);
    [Dvolume, Dhdr] = loadvol(Ddatapath, 1);
    for slice = 30:hdr.fpV    
        display(slice);
        % Display the grayscale image
        m_Gray_image = volume(:,:,slice)';
        figure(1); imagesc(m_Gray_image); axis equal; axis off; hold on;
        colormap(gray);
        % Display the color image for reference
        [m_RGB_image, m_Color_image] = separatecolor(Dvolume(:,:,slice)');
        figure(2); imshow(m_RGB_image); axis equal; axis off; hold on;
        titlestr = sprintf('Doppler data'); 
        pause();
    end
end

%% Save the images for the figure
Doppler = imresize(m_RGB_image,10);
BMode = imresize(m_Gray_image./max(m_Gray_image(:)),10);
imwrite(Doppler,'Doppler.jpg');
imwrite(BMode,'BMode.jpg');
