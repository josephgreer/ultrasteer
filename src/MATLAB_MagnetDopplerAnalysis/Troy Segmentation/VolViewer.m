% -------------------------------------------------------------------------
% author:   Troy Adebar
% brief:    Scrolls through volume and shows the 2D image data
%
%% -Doppler-----------------------------------------------------------
fol = 'V:\NeedleScan\9_26_14_MagnetScan\Scan 4\';
filename = '1_auto_colorpost.vol';
datapath = [fol filename]; % The file saved in propello
% Load the volume information and data from the file
[volume, hdr] = loadvol(datapath, 1);
% Check that it's post-scan data as expected
if(hdr.datatype ~= 4)
    display('Error: Data not read in as post-scan B/color')
else
    % Initialize some matrices
    pts = [];
    % Loop through slices in the volume and search for needle
    for slice = 1:size(volume,3)
        % Isolate the color data
        [m_RGB_image, m_Col_image] = separatecolor(volume(:,:,slice)');
        m_Gray_image = rgb2gray(m_Col_image); % grayscale Doppler
        figure(1); imshow(m_RGB_image);
        title(strcat('slice  ', num2str(slice), '  of  ', num2str(size(volume,3))));
        pause();
    end
end

%% -B-Mode-----------------------------------------------------------
fol = 'Needle Diameter/380 microns/3/';
filename = '1_auto_postscan.vol';
datapath = [fol filename]; % The file saved in propello
% Load the volume information and data from the file
[volume, hdr] = loadvol(datapath, 1);
    % Initialize some matrices
    pts = [];
    % Loop through slices in the volume and search for needle
    for slice = 5:61
        m_Gray_image = volume(:,:,slice)'; % grayscale Doppler
        figure(1); imagesc(m_Gray_image); colormap(gray);
        pause();
    end


%% Create 2D image of raw Doppler data
rawI = m_RGB_image;
lgI = imresize(rawI,5);
imwrite(lgI,'Raw US.png');

%% Create 2D image of raw US data
rawI = m_Gray_image;
lgI = imresize(rawI,5);
imwrite(lgI./max(lgI(:)),'Raw US.png');

