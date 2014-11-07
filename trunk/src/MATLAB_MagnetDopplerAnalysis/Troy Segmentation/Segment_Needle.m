% -------------------------------------------------------------------------
% author: Troy Adebar
% date: 2013-02-19
%
% title: Segment_Needle(folder)
% summary: This .m file scans the color volume located in the specified
% folder, and returns the localized image points
% -------------------------------------------------------------------------
function [im_x,im_y,slices] = Segment_Needle(folder)
filename = '1_auto_colorpost.vol';
datapath = [folder filename]; % The file saved in propello
load([folder 'settings'])
% Load the volume information and data from the file
[volume, hdr] = loadvol(datapath, 1);
% Check that it's post-scan data as expected
if(hdr.datatype ~= 4)
    display('Error: Data not read in as post-scan B/color')
else
    % Initialize some matrices
    pts = [];
    % Loop through slices in the volume and search for needle
    for slice = 5:fpV
        % Isolate the color data
        [m_RGB_image, m_Col_image] = separatecolor(volume(:,:,slice)');
        m_Gray_image = rgb2gray(m_Col_image); % grayscale Doppler
        % Process Doppler data
        % Theshold Doppler data at 5/255 intensity level
        m_BW_image = im2bw(m_Col_image,5/255);
        %m_BW_image([1:186],:) = 0;
        m_BW_image([250:size(m_BW_image,1)],:) = 0;
        %m_BW_image(:,[1:215]) = 0;
        % Remove pixel groups with less than 300 pixels total area
        m_BW_image2 = bwareaopen(m_BW_image,150);
        CC = bwconncomp(m_BW_image2);
        numPixels = cellfun(@numel,CC.PixelIdxList);
        [biggest,idx] = max(numPixels);
        m_BW_image2(:) = 0;
        m_BW_image2(CC.PixelIdxList{idx}) = 1;      
        m_BW_image = m_BW_image2;
        m_Gray_image(~m_BW_image) = 0; m_Gray_image = double(m_Gray_image);
%         figure(1);
%         imshow(m_RGB_image);
%         title(strcat(num2str(slice), ' of ', num2str(fpV)));
%         pause;
%         imshow(m_BW_image);
%         pause;
%         Use centroid of the Doppler data as the needle
        if(sum(sum(m_Gray_image)))
            figure(2);
            [rows,cols] = size(m_Gray_image);
            y = 1:rows; x = 1:cols; [X Y] = meshgrid(x,y);
            x_bar = sum(sum(X.*m_Gray_image))/sum(sum(m_Gray_image));
%             y_bar = sum(sum(Y.*m_Gray_image))/sum(sum(m_Gray_image));
            y_bar = new_find_quarter_point(m_Gray_image,0.25);
            [m,n] = size(m_BW_image);
            pts = [pts; x_bar,y_bar,slice];
          
            corners = [0 0; cols 0; 0 rows];
            slices = repmat(slice, 3, 1);
            pts3D = scanconvertpoints(corners,...
                slices,degPerFr,fpV,probeR3D_mm,...
                micr_x,micr_y,o_x,o_y,m,n);
            PlotPlane(pts3D(1,:), pts3D(2,:)-pts3D(1,:), pts3D(3,:)-pts3D(1,:),m_RGB_image,max(double(rgb2gray(m_RGB_image))/1400,double(m_BW_image))*255);
            hold on;
            axis equal;
        end
    end
end
im_x = pts(:,1); im_y = pts(:,2); slices = pts(:,3);
end