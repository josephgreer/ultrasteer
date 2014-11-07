% -------------------------------------------------------------------------
% author: Troy Adebar
% date: 2013-02-19
%
% title: Needle_Segmentation
%
% summary: This .m file is based on test.m, which is supplied by ultrasonix
% with the propello program. This file reads in matlab data from the
% filename specified in the script, and applies image analysis methods to
% the raw data to localize a curved needle
% -------------------------------------------------------------------------
%% Load Data
clear all; close all;
% define constants
order = 3; % for curve fitting
% -------------------------------------------------------------------------
% - - - - - - -  Get these values from propello window! - - - - - - - - - -
% -------------------------------------------------------------------------
filename = '1_auto_colorpost.vol';
folder = 'Needle Configuration\straight\3\';
datapath = [folder filename]; % The file saved in propello
whichVol = 1; % Of the volumes saved in datapath, which one will be read in
degPerFr = 0.731; % Angular increment (deg) between frames
imgDepth = 80; % Imaging depth in mm
probeR3D_mm = 27.25; % 3D probe radius in mm
micr_x = 161; % Microns per image pixel in the x direction
micr_y = 161; % Microns per image pixel in the y direction
o_x = 241; % Position of the image origin in the x direction
o_y = 57; % Position of the image origin in the y direction
% -------------------------------------------------------------------------
% - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
% -------------------------------------------------------------------------

% Load the volume information and data from the file
[volume, hdr] = loadvol(datapath, whichVol);
% Check that it's post-scan data as expected
if(hdr.datatype ~= 4)
    display('Error: Data not read in as post-scan B/color')
else
    % Initialize some matrices
    pts = []; guess = []; needle_loc = []; errors = [];
    % Loop through slices in the volume and search for needle
    for slice = 1:hdr.fpV
        figure(1); set(gcf,'Color','w');
        %% Isolate the color data
        [m_RGB_image, m_Col_image] = separatecolor(volume(:,:,slice)');
        m_Gray_image = rgb2gray(m_Col_image); % grayscale Doppler
        % Display the B+color image
        subplot(131); hold off; imagesc(m_RGB_image); axis image; axis off;
        hold on; title(sprintf('Slice number %d',slice));
        % Display the color only image
        subplot(132); hold off; imagesc(m_Gray_image); axis image; axis off;
        hold on; title('Doppler data');
        %% Process Doppler data
        % Theshold Doppler data at 13/255 intensity level
        m_BW_image = im2bw(m_Col_image,5/255);
        % Remove pixel groups with less than 10 pixels total area
        m_BW_image2 = bwareaopen(m_BW_image,300);
        m_BW_image = m_BW_image2;
        m_Gray_image(~m_BW_image) = 0; m_Gray_image = double(m_Gray_image);
        % Display the processed Doppler data
        subplot(133); hold off; imagesc(m_Gray_image);axis image; axis off;
        hold on; title('Processed Doppler data'); colormap(gray);
        %% Use centroid of the Doppler data as the needle
        if(sum(sum(m_Gray_image)))
            [rows cols] = size(m_Gray_image);
            y = 1:rows; x = 1:cols; [X Y] = meshgrid(x,y);
            x_bar = sum(sum(X.*m_Gray_image))/sum(sum(m_Gray_image));
            y_bar = sum(sum(Y.*m_Gray_image))/sum(sum(m_Gray_image));
%             y_bar = new_find_quarter_point(m_Gray_image,0.25);
            plot(x_bar, y_bar, 'b+');
            [m,n] = size(m_BW_image);
            pts = [pts; x_bar,y_bar,slice,1];
            
            %% Compare to manual segmentation
            load([folder 'manual_segmentation']);
            i = find(man_slices == slice);
            if( i ) % If we have a manual segmentation for that slice
                error = (micr_x/1000*(man_x_pix(i)-x_bar))^2 +...
                    (micr_x/1000*(man_y_pix(i)-y_bar))^2;
                error = sqrt(error); errors = [errors error];
                % Plot manual points
                plot(man_x_pix(i), man_y_pix(i), 'r*');
            end
            
            % Compare to ITK points
            rawitkPts = csvread([folder 'CDIimpts.txt']);
            nn = size(rawitkPts(1:end-1),2)/3;
            itkPts = [reshape(rawitkPts(1:end-1),3,nn)]';
            itkPts(:,2) = m-itkPts(:,2);
            % Plots ITK points
            itk_ind = find(itkPts(:,3) == slice-1);
            if (itk_ind)
                plot(itkPts(itk_ind,1), itkPts(itk_ind,2), 'm^');
%                 plot(itkPts(itk_ind,1), m - itkPts(itk_ind,2), 'm^');
            end
            
            %% Plot crosshairs
            plot([1 cols],[rows/2 rows/2],'w');
            plot([cols/2 cols/2],[1 rows],'w');
         
            %% Scan convert points
            pts_SC = scanconvertpoints(pts(:,1:2),pts(:,3),...
                degPerFr,hdr.fpV,probeR3D_mm,micr_x,micr_y,o_x,o_y,m,n);
            
            man_SC = scanconvertpoints([man_x_pix',man_y_pix'],...
                man_slices',degPerFr,hdr.fpV,probeR3D_mm,micr_x,micr_y,o_x,o_y,m,n);
            
            itk_SC = scanconvertpoints(itkPts(:,1:2),itkPts(:,3)+1,...
                degPerFr,hdr.fpV,probeR3D_mm,micr_x,micr_y,o_x,o_y,m,n);
            
            set(gca,'ZDir','reverse')
            pause; % Pause at the end of each image
        end
    end
end

%% Plot the SC points
axisbounds = [-15 15 -40 40 0 80 ];
figure(2);
subplot(1,5,1);
scatter3(pts_SC(:,1),pts_SC(:,3),pts_SC(:,2),'b+');
drawnow; hold off; axis equal; view(-120,25); set(gca,'ZDir','reverse');
axis(axisbounds)
xlabel('x'); ylabel('z'); zlabel('y');

subplot(1,5,2);
scatter3(man_SC(:,1),man_SC(:,3),man_SC(:,2),'r*');
drawnow; hold off; axis equal; view(-120,25); set(gca,'ZDir','reverse');
axis(axisbounds)
xlabel('x'); ylabel('z'); zlabel('y');

subplot(1,5,3);
scatter3(itk_SC(:,1),itk_SC(:,3),itk_SC(:,2),'m^');
drawnow; hold off; axis equal; view(-120,25); set(gca,'ZDir','reverse');
axis(axisbounds)
xlabel('x'); ylabel('z'); zlabel('y');

rawitk3DPts = csvread([folder 'CDIscpts.txt']);
            nn = size(rawitk3DPts(1:end-1),2)/3;
            itk3DPts = [reshape(rawitk3DPts(1:end-1),3,nn)]';
subplot(1,5,4);
scatter3(itk3DPts(:,1),itk3DPts(:,3),itk3DPts(:,2),'cs');
drawnow; hold off; axis equal; view(-120,25); set(gca,'ZDir','reverse');
axis(axisbounds)
xlabel('x'); ylabel('z'); zlabel('y');

subplot(1,5,5);
scatter3(pts_SC(:,1),pts_SC(:,3),pts_SC(:,2),'b+'); hold on;
scatter3(man_SC(:,1),man_SC(:,3),man_SC(:,2),'r*');
scatter3(itk_SC(:,1),itk_SC(:,3),itk_SC(:,2),'m^');
scatter3(itk3DPts(:,1),itk3DPts(:,3),itk3DPts(:,2),'cs');
drawnow; hold off; axis equal; view(-120,25); set(gca,'ZDir','reverse');
axis(axisbounds)
xlabel('x'); ylabel('z'); zlabel('y');

% Save the scan converted points
scX = pts_SC(:,1);
scY = pts_SC(:,2);
scZ = pts_SC(:,3);
save([folder 'MatlabSCpts'],'scX','scY','scZ');

% % Save the image points
% imgX = pts(:,1);
% imgY = pts(:,2);
% save([folder 'MatlabIMGpts'],'imgX','imgY');

% Mean error
mean_error = mean(errors)