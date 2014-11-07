% -------------------------------------------------------------------------
% author: Troy Adebar
% date: 2013-05-20
%
% title: Doppler_Visualization
%
% summary: This .m file is based on Needle_Segmenation.m. It reads a
% specified propello color volume, segments the needle based on the Color
% Doppler w/ vibration method, then produces a 3D plot of the Doppler data
% outlines. A skeleton outline of the volume can also be shown
% -------------------------------------------------------------------------
%% Load Data
clear all; close all;
% define constants
order = 3; % for curve fitting
% -------------------------------------------------------------------------
% - - - - - - -  Get these values from propello window! - - - - - - - - - -
% -------------------------------------------------------------------------
filename = '1_auto_colorpost.vol';
folder = 'data/';
datapath = [folder filename]; % The file saved in propello
whichVol = 1; % Of the volumes saved in datapath, which one will be read in
degPerFr = 0.731; % Angular increment (deg) between frames
imgDepth = 100; % Imaging depth in mm
probeR3D_mm = 27.25; % 3D probe radius in mm
probeR2D_mm = 40; % 2D image radius in mm
micr_x = 238; % Microns per image pixel in the x direction
micr_y = 238; % Microns per image pixel in the y direction
o_x = 241; % Position of the image origin in the x direction
o_y = 40; % Position of the image origin in the y direction
sect_ang = 75.85; % Arc of 2D image in degrees
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
    Col = zeros(hdr.h,hdr.w,hdr.fpV); BW = Col;
    % Loop through slices in the volume and search for needle
    SS = [];
    for slice = 1:hdr.fpV
        %% Isolate the color data
        [m_RGB_image, m_Col_image] = separatecolor(volume(:,:,slice)');
        m_Gray_image = rgb2gray(m_Col_image); % grayscale Doppler
        %% Process Doppler data
        % Theshold Doppler data at 5/255 intensity level
        m_BW_image = im2bw(m_Col_image,5/255);
        % Remove pixel groups with less than 300 pixels total area
        m_BW_image2 = bwareaopen(m_BW_image,300);
        s = regionprops(m_BW_image2, 'area');
        SS = [SS; cat(1,s.Area)];       
        m_BW_image = m_BW_image2;
        m_Gray_image(~m_BW_image) = 0; m_Gray_image = double(m_Gray_image);
        % Save Doppler data for visualization
        Col(:,:,slice) = m_Gray_image;
        BW(:,:,slice) = m_BW_image;
        %% Use centroid of the Doppler data as the needle
        if(sum(sum(m_Gray_image)))
            [rows cols] = size(m_Gray_image);
            y = 1:rows; x = 1:cols; [X Y] = meshgrid(x,y);
            x_bar = sum(sum(X.*m_Gray_image))/sum(sum(m_Gray_image));
            y_bar = new_find_quarter_point(m_Gray_image,0.25);
            [m,n] = size(m_BW_image);
            pts = [pts; x_bar,y_bar,slice,1];
            
            %% Compare to manual segmentation
            load([folder 'manual_segmentation']);
            i = find(man_slices == slice);
            if( i ) % If we have a manual segmentation for that slice
                error = (micr_x/1000*(man_x_pix(i)-x_bar))^2 +...
                    (micr_x/1000*(man_y_pix(i)-y_bar))^2;
                error = sqrt(error); errors = [errors error];
            end
            
            %% Compare to ITK points
            rawitkPts = csvread([folder 'CDIimpts.txt']);
            nn = size(rawitkPts(1:end-1),2)/3;
            itkPts = [reshape(rawitkPts(1:end-1),3,nn)]';
            itkPts(:,2) = m-itkPts(:,2);
                        
            %% Scan convert points
            pts_SC = scanconvertpoints(pts(:,1:2),pts(:,3),...
                degPerFr,hdr.fpV,probeR3D_mm,micr_x,micr_y,o_x,o_y,m,n);
            man_SC = scanconvertpoints([man_x_pix',man_y_pix'],...
                man_slices',degPerFr,hdr.fpV,probeR3D_mm,micr_x,micr_y,o_x,o_y,m,n);
            itk_SC = scanconvertpoints(itkPts(:,1:2),itkPts(:,3)+1,...
                degPerFr,hdr.fpV,probeR3D_mm,micr_x,micr_y,o_x,o_y,m,n);

            %% Display segmentation across images
            % Display the B+color image
            subplot(131); hold off; imagesc(m_RGB_image); axis image; axis off;
            hold on; title(sprintf('Slice number %d',slice));
            % Display the color only image
            subplot(132); hold off; imagesc(m_Col_image); axis image; axis off;
            hold on; title('Doppler data');
            % Display the processed Doppler data
            subplot(133); hold off; imagesc(m_Gray_image);axis image; axis off;
            hold on; title('Processed Doppler data'); colormap(gray);
            set(gca,'ZDir','reverse')
            % plot the centroid point
            plot(x_bar, y_bar, 'b+');
            % plot ITK point if we have one
            itk_ind = find(itkPts(:,3) == slice);
            if (itk_ind)
                plot(itkPts(itk_ind,1), itkPts(itk_ind,2), 'm^');
            end
            % plot manual point if we have one
            i = find(man_slices == slice);
            if( i )
                plot(man_x_pix(i), man_y_pix(i), 'r*'); 
            end            
        end
    end
end

%% Generate the 3D plot
figure(2);
% Plot the volume boundaries
sliceInt = 60;
doppInt = 5;
p_vol = find_vol_boundaries(imgDepth,probeR2D_mm,micr_x,micr_y,o_x,o_y,sect_ang,sliceInt);
for k = 1:length(p_vol)
    pts_vol = p_vol{k};
    pts_vol_SC = scanconvertpoints(pts_vol(:,1:2),pts_vol(:,3),...
            degPerFr,hdr.fpV,probeR3D_mm,...
            micr_x,micr_y,o_x,o_y,m,n);
    plot3(pts_vol_SC(:,1),pts_vol_SC(:,3),pts_vol_SC(:,2),'-k','Linewidth',1.5); hold on;
end

% Plot the SC points
% scatter3(pts_SC(:,1),pts_SC(:,3),pts_SC(:,2),'b');
drawnow; hold off; axis equal; view(-120,25); set(gca,'ZDir','reverse');
grid on;
set(gca,'FontName','Times New Roman');
% axis([-15 15 -40 40 60 100 ])
xlab = xlabel('x dir. (mm)'); zlab = ylabel('z dir. (mm)'); zlabel('y dir. (mm)');
set(xlab,'position',[-700 750 -335]);
set(zlab,'position',[-780 685 -325]);
hold on;

% Plot the Doppler data
coloring = 1; % 0 for outline, 1 for actual Doppler, 2 for solid color
for slice = 1:doppInt:hdr.fpV
    if(coloring == 2)
        % Create outline of Doppler data
        B = bwboundaries(Col(:,:,slice));
        P = size(B);
        if(P)
            Doppler = [B{1}(:,2) B{1}(:,1) slice*ones(size(B{1}(:,1)))];
            Doppler_SC{slice} = scanconvertpoints(Doppler(:,1:2),...
                Doppler(:,3),degPerFr,hdr.fpV,probeR3D_mm,...
                micr_x,micr_y,o_x,o_y,m,n);
            if(~isempty(Doppler_SC{slice}))
                hpol = fill3(Doppler_SC{slice}(:,1),Doppler_SC{slice}(:,3),Doppler_SC{slice}(:,2),'Color',[0 0 1]);
                alpha(hpol,0.25);
                set(hpol,'EdgeColor','None');
            end
        end
    end
    if(coloring == 1)
        Doppler_SC = scanconvertpoints([y' y'],slice*ones(size(y')),...
            degPerFr,hdr.fpV,probeR3D_mm,...
            micr_x,micr_y,o_x,o_y,m,n);
        
        Doppler_X = ((1:cols)-o_x).*micr_x/1000;
        Doppler_Y = repmat(Doppler_SC(:,2),1,cols);
        Doppler_Z = Doppler_SC(:,3);
        
        Doppler_Y(~BW(:,:,slice)) = nan;
        h = surf(Doppler_X,Doppler_Z,Doppler_Y,Col(:,:,slice),'LineStyle','none'); colormap(copper)
        alpha(h,0.5)
        y = 1:rows; x = 1:cols; [X Y] = meshgrid(x,y);
    end
    if(coloring == 0)
        % Create outline of Doppler data
        B = bwboundaries(Col(:,:,slice));
        P = size(B);
        if(P)
            Doppler = [B{1}(:,2) B{1}(:,1) slice*ones(size(B{1}(:,1)))];
            Doppler_SC{slice} = scanconvertpoints(Doppler(:,1:2),...
                Doppler(:,3),degPerFr,hdr.fpV,probeR3D_mm,...
                micr_x,micr_y,o_x,o_y,m,n);
            if(~isempty(Doppler_SC{slice}))
                ds = 5;
                hpol = plot3(Doppler_SC{slice}([1:ds:end 1],1),Doppler_SC{slice}([1:ds:end 1],3),Doppler_SC{slice}([1:ds:end 1],2),'r','Linewidth',2);
            end
        end
    end    
end

% Plot the polynomial curve
Px = polyfit(pts_SC(:,3),pts_SC(:,1),order);
Py = polyfit(pts_SC(:,3),pts_SC(:,2),order);
pts_fit = [polyval(Px,pts_SC(:,3)),polyval(Py,pts_SC(:,3)),pts_SC(:,3)];
plot3(pts_fit(:,1),pts_fit(:,3),pts_fit(:,2),'Color',[0 0 1],'Linewidth',2); hold on;

view(-135,25);
axis equal;
hold off;
export_fig Doppler3D.pdf -transparent