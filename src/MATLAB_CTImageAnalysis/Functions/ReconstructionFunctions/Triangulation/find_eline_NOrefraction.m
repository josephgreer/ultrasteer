function [eline] = find_eline_NOrefraction (pixel,cam1_name, stereo, isGraphicsOn,index)
% A funcion that returns the epipolar line in the corresponding camera 
% intput
% selected_pixel
% the name of the selected camera
% the stereo geometry describing the camera pair

% output:
% the epipolar line (format TBD)

%Ensure that the stereo_pair has the cameras set up correctly, otherwise
%either reverse the stereo pair or declare an error. 

%% Define the stereo and refraction geometry
    
%% Project the pixels to the gel
% Use the selected pixel in the camera to generate a ray to the surface of
% the gel (at point p); form a ray that is // to the ray from the point on
% the gel to the COP

 if (strcmp(cam1_name, stereo.left_name))
     % Select the source camera

%......................................................................
% DEFINE CAMERA PARAMETERS
%......................................................................
    camera1.ratio = [0.45; 0.45];               
    camera1.origin = stereo.left.cc';
    camera1.focal = -1*stereo.left.fc(1);

    camera2.ratio = [0.45; 0.45];
    camera2.origin = stereo.right.cc';
    camera2.focal = -1*stereo.right.fc(1);

    cameras{1} = camera1;
    cameras{2} = camera2;

    poses{1} = stereo.G_world2left;
    poses{2} = stereo.G_world2right; 
    
    % For 6/22
    if index == 1 
        offset1 = 95;
        offset2 = -75;
    end
    
    % For 8/18
    if index == 2
        offset1 = 110;
        offset2 = -50;
    end
    
    % For 8/1
    if index == 3 
        offset1 = 70;
        offset2 = -50;
    end
    
      % For 8/8
    if index == 4 || index == 5
        offset1 = 75;
        offset2 = -55;
    end

%     % For 9/11
%     if index == 5 
%         offset1 = 80;
%         offset2 = -60;
%     end
         
 elseif (strcmp(cam1_name, stereo.right_name))

%......................................................................
% DEFINE CAMERA PARAMETERS
%......................................................................
    camera1.ratio = [0.45; 0.45];               
    camera1.origin = stereo.right.cc';
    camera1.focal = -1*stereo.right.fc(1);

    camera2.ratio = [0.45; 0.45];
    camera2.origin = stereo.left.cc';
    camera2.focal = -1*stereo.left.fc(1);

    cameras{1} = camera1;
    cameras{2} = camera2;

    poses{1} = stereo.G_world2right;
    poses{2} = stereo.G_world2left; 
    
    % For 6/22
    if index == 1 
        offset1 = -75;
        offset2 = 95;
    end
    
    % For 8/18
    if index == 2 
        offset1 = -50;
        offset2 = 110;
    end
    
    % For 8/1
    if index == 3 
        offset1 = -50;
        offset2 = 70;
    end

    % For 8/8
    if index == 4 || index == 5
        offset1 = -55;
        offset2 = 75;
    end
    
%     % For 9/11
%     if index == 5 
%         offset1 = -60;
%         offset2 = 80;
%     end
    
 else
     Disp('ERROR: The source camera name for eline does not match either camera name in the stereo pair');
     stop
 end


% reformating camera parameters
for i = 1:2
      ex(i,1) = cameras{i}.ratio(1);
      ey(i,1) = cameras{i}.ratio(2);
      xo(i,1) = cameras{i}.origin(1);
      yo(i,1) = cameras{i}.origin(2);
      focal(i,1) = cameras{i}.focal;
end

K1=[-1*focal(1)/ex(1),0,xo(1),0;0,-1*focal(1)/ey(1),yo(1),0;0,0,1,0];
K2=[-1*focal(2)/ex(2),0,xo(2),0;0,-1*focal(2)/ey(2),yo(2),0;0,0,1,0];

P1 = K1*poses{1};
P2 = K2*poses{2};

% Compute F from P's
F = vgg_F_from_P(P1,P2);

line1 = F*[pixel(1);pixel(2);1];

% Display
%vgg_gui_F(LeftI, RightI , F');  % WHY F TRANSPOSE??
%disp('Computed epipolar geometry. Move the mouse to verify')


% Getting the epipolar line on the RIGHT image:
eline = zeros(2,2);
eline(1,:)= [pixel(1)-offset1 pixel(1)+offset2];

% Using the eqn of line: ax+by+c=0; y = (-c-ax)/b
eline(2,:) = (-line1(3)-line1(1)*eline(1,:))/line1(2);


