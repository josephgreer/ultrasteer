function [corres_pixels, correspondance_metric] = find_corres_pix2  (centers_1, cam1_name,...
    needle_image_2, cam2_name, stereo_pair, refraction, isGraphicsOn,folderindex)
% input:
% center locations in camera 1 size: 2 x n
% name of camera 1
% needle image from camera 2
% name of camera 2
% stereo pair
% refraction
% isGraphicsOn

% output:
% corresponding pixels in needle image2
% some correspondance metric to evaluate the "goodness of correspondance"

%isGraphicsOn = false;

% This function  determines the corresponding pixel (along an epipolar
% line), associated with a selected pixel in an image

%major steps
% call a function that generates the epipolar line
% Using the epipolar line, determine corresponding pixel
% Return the corresponding pixel and some notion of "goodness of
% correspondance"


% Given the center locations from camera 1 (source_camera), locate the corresponding pixels
% in camera 2

% Source camera is so that the code knows what camera parameters to use
% Describes the plane fit to the tip locations: tip_normal, tip_mean_data

% Should probably ensure that the cameras are acceptable

% initialize the variavbles
corres_pixels = zeros(size(centers_1));
correspondance_metric = zeros(size(centers_1,1),2);
%world_locations = [];
%pixels_2 = [];

% Ensure that there are pixels to consider
if ~(isempty(centers_1))

    for index = 1 : size (centers_1,2)


        % Using the motion in the right image, determine the location
                % (s?) of the needle in three-d
                % parse the input
                pixel = double(centers_1(:,index))';

                  
                % Determine the epipolar line
               % eline = find_eline(pixel, cam1_name, stereo_pair, refraction, isGraphicsOn);
                eline = find_eline_NOrefraction(pixel, cam1_name, stereo_pair, false,folderindex);

                isGraphicsOn = 0;
                if (isGraphicsOn)
                    
                    %figure(10)
                    %subplot(1,2,1)
                    %imshow(stereo_pair.left.
                    
                    figure(10)
                   % subplot(1,2,1)
                    imshow(needle_image_2,[])
                   hold on
                   % subplot(1,2,2)

                    %plot(eline(:,1),eline(:,2),'y') % ANN CHANGED 3/25/11 originally plots rows not columns. 
                    plot(eline(2,:),eline(1,:),'y') 

                    hold off
                    
                    
                    %figure(12)
                end
    
                
                % Determine the corresponding pixel and the "goodness of
                % correspondance"
                [pix_match, goodness, goodness2] = locate_match(eline, needle_image_2);
                
                corres_pixels(:,index) =  pix_match;
                correspondance_metric (index,1:2) = [goodness,goodness2];
                
                % Determine the "goodness of correspondance"
               
                
                %y1 = double(centers_1(index,1));
                %x1 = double(centers_1(index,2));                
                %[world_location pixel_2 pixel_1] = location3d2 ([x1;y1], needle_image_2, source_camera, isGraphicsOn, tip_normal, tip_mean_data);

                %world_locations = [world_locations; world_location'];
                %pixels_2 = [pixels_2; pixel_2'];
%                 
%                 if (isGraphicsOn)
%                     figure (5)
%                     subplot (2,2,3)
%                     hold on
%                     plot3(world_location(1),world_location(2),world_location(3) , 'bx')
%                     axis ('equal')
%                     hold off
%                 end

    end
else
    disp('No matchaes on the E-line were found')
end

             