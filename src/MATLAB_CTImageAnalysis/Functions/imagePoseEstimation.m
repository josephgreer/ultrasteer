function [] = imagePoseEstimation(startIDX,fiducialDirectory,poseDirectory,StereoXrayParameters)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
if ~exist([poseDirectory 'angles.mat'])
    theta = zeros(1,9);
    w = zeros(9,3);
else
    load([poseDirectory 'angles.mat']);
    load([poseDirectory 'axis.mat'])
end
for i=startIDX:length(dir([fiducialDirectory '/85/BBs_test*.txt']))
    imagePrefix = ['test00' int2str(i)];
    
    % CONVENTION: "CAMERA" at 85deg is the LEFT camera, "CAMERA" at 95deg is the RIGHT CAMERA  
    params = [StereoXrayParameters(2,:)+240; StereoXrayParameters(1,:)+240; StereoXrayParameters(3,:)]; %% TEST THIS!!!
    stereo.left.fc = repmat(params(3,1),2,1);           % Focal Length - no uncertainity known so elements are same  
    stereo.left.cc = params(1:2,1);                      % Origin is defined wrt upper left edge so need to add offset
    stereo.left.kc = [0,0,0,0];                                            % Assume no distortion because images are dewarped. 
    stereo.left.alpha_c = 0;                                               % Assume zero skew

    stereo.right.fc = repmat(params(3,2),2,1);          % Focal Length - no uncertainity known so elements are same 
    stereo.right.cc = params(1:2,2);                     % Origin is defined wrt upper left edge so need to add offset
    stereo.right.kc = [0,0,0,0];                                           % Assume no distortion because images are dewarped. 
    stereo.right.alpha_c = 0;                                              % Assume zero skew

    stereo.left_name = 'left';                                             %should be the name of the left camera
    stereo.right_name = 'right';                                           %should be the name of the right camera THIS SHOULD BE PASSED IN
    
    [pose85, error85, BBs85] = poseEstimateForAngle(fiducialDirectory,'85/',imagePrefix,stereo.left.fc(1),stereo.left.cc);
    [pose95, error95, BBs95] = poseEstimateForAngle(fiducialDirectory,'95/',imagePrefix,stereo.right.fc(1),stereo.right.cc);
    
    stereoPose(:,:,1) = pose85;         % POSE OF LEFT CAMERA                         
    stereoPose(:,:,2) = pose95;         % POSE OF RIGHT CAMERA
   
    [RL,pL] = deframe(stereoPose(:,:,1)); % POSE OF FIDUCIAL IN LEFT CAMERA
    [RR,pR] = deframe(stereoPose(:,:,2)); % POSE OF FIDUCIAL IN RIGHT CAMERA

    RRinv = inv(RR);                    % Inverse rotation...
    pRinv = -1*inv(RR)*pR;              % Inverse translation...
    
    
    RLinv = inv(RL);                    % Inverse rotation...
    pLinv = -1*inv(RL)*pL;              % Inverse translation...
    
% *** ASSUMING GET POSE OF FTRAC IN IMAGE COORDINATES
%     R = RR*RLinv;                       % ROTATION FROM RIGHT TO LEFT CAMERA
%     p = RR*pLinv+pR;              % TRANSLATION FROM RIGHT TO LEFT CAMER
% 
%     stereoPose(:,:,3) = frame(R,p);     % UPDATE STEREO POSE
%     theta(i) = acosd((trace(R)-1)/2);  % ANGLE BETWEEN RIGHT AND LEFT CAMERA
%     w(i,:) = 1/sin(theta(i))*[R(3,2)-R(2,3) R(1,3)-R(3,1) R(2,1)-R(1,2)]; % AXIS OF ROTATION FOR RIGHT AND LEFT CAMERA
%     
%     
%    
%     stereo.om = rodrigues(R);     % rotation  vector between right and left cameras (output of stereo calibration), position of right camera wrt left camera
%     stereo.T = p;                 % position of left camera in right camera coordinate frame
%     stereo.R = R;                 % rotation between rigth and left camera
% 
%     stereo.R_world2left = RLinv;
%     stereo.T_world2left = pLinv;
%     stereo.G_world2left = frame(stereo.R_world2left,stereo.T_world2left);
%     
% 
%     stereo.R_left2world = RL;
%     stereo.T_left2world = pL;
%     stereo.G_left2world = frame(stereo.R_left2world,stereo.T_left2world); %Homogeneous transform from world frame to the left frame
% 
%     stereo.R_world2right = RRinv;
%     stereo.T_world2right = pRinv;
%     stereo.G_world2right = frame(stereo.R_world2right,stereo.T_world2right);
% 
%     stereo.R_right2world = RR;
%     stereo.T_right2world = pR;
%     stereo.G_right2world = frame(stereo.R_right2world,stereo.T_right2world); 
 

% *** ASSUMING GET POSE OF CAMERA IN FTRAC COORDINATES
    R = RRinv*RL;                       % ROTATION FROM RIGHT TO LEFT CAMERA
    p = RRinv*pL+pRinv;              % TRANSLATION FROM RIGHT TO LEFT CAMER

  %  stereoPose(:,:,3) = frame(R,p);     % UPDATE STEREO POSE
    theta(i) = acosd((trace(R)-1)/2);  % ANGLE BETWEEN RIGHT AND LEFT CAMERA
    w(i,:) = 1/sin(theta(i))*[R(3,2)-R(2,3) R(1,3)-R(3,1) R(2,1)-R(1,2)]; % AXIS OF ROTATION FOR RIGHT AND LEFT CAMERA
   
  %  stereo.om = rodrigues(R);     % rotation  vector between right and left cameras (output of stereo calibration), position of right camera wrt left camera
  %  stereo.T = p;                 % position of left camera in right camera coordinate frame
  %  stereo.R = R;                 % rotation between rigth and left camera

    stereo.R_world2left = RL;
    stereo.T_world2left = pL;
    stereo.G_world2left = frame(stereo.R_world2left,stereo.T_world2left);
    

 %   stereo.R_left2world = RLinv;
  %  stereo.T_left2world = pLinv;
  %  stereo.G_left2world = frame(stereo.R_left2world,stereo.T_left2world); %Homogeneous transform from world frame to the left frame

    stereo.R_world2right = RR;
    stereo.T_world2right = pR;
    stereo.G_world2right = frame(stereo.R_world2right,stereo.T_world2right); 
    
   % stereo.R_right2world = RRinv;
   % stereo.T_right2world = pRinv;
   % stereo.G_right2world = frame(stereo.R_right2world,stereo.T_right2world);

  
% ************


    stereo.R_right2left = R;
    stereo.T_right2left = p;
    stereo.G_right2left = frame(R,p);
    
    [stereo.E, stereo.F] = fundamentalMatrixEstimate(BBs85,BBs95); % ESTIMATE FUNDAMENTAL MATRIX USING THE EIGHT POINT ALGORITHM
    
    
    % SAVE VARIABLES!
    save([poseDirectory 'stereoPair_test00' int2str(i) '.mat'],'stereo');
   % save([poseDirectory 'stereoPose_test00' int2str(i) '.mat'], 'stereoPose');  % SAVE POSE INFORMATION


end
    save([poseDirectory 'angles.mat'],'theta');
    save([poseDirectory 'axis.mat'],'w');

end

function [anglePose, angleError, BBs] = poseEstimateForAngle(fiducialDirectory,angle,imagePrefix,focalLength,sourcePosition)
   
   % pixelSize = [480 480]
    
    pixelResolution = [0.45 0.45]; % Or.44 .44?
    pixel_ratio = [1 / pixelResolution(1), 1 / pixelResolution(2)];   


    BBsFile = strcat(fiducialDirectory,angle, 'BBs_', imagePrefix, '.txt');
    Line1_EqFile = strcat(fiducialDirectory,angle ,'Line1_', imagePrefix, '.txt');
    Line2_EqFile = strcat(fiducialDirectory,angle, 'Line2_', imagePrefix, '.txt');
    Line3_EqFile = strcat(fiducialDirectory,angle, 'Line3_', imagePrefix, '.txt');
    Ellipse1_EqFile = strcat(fiducialDirectory,angle, 'Ellipse1_', imagePrefix, '.txt');
    Ellipse2_EqFile = strcat(fiducialDirectory,angle, 'Ellipse2_', imagePrefix, '.txt');

    BBs = load(BBsFile, '-ascii');
    Line1_Eq = load(Line1_EqFile, '-ascii');
    Line2_Eq = load(Line2_EqFile, '-ascii');
    Line3_Eq = load(Line3_EqFile, '-ascii');
    Ellipse1_Eq = load(Ellipse1_EqFile, '-ascii');
    Ellipse2_Eq = load(Ellipse2_EqFile, '-ascii');

    screen_size = [];
    
%       initial_estimate = [ 
%                 0  0 -1  0; 
%                 1  0  0  0;
%                 0 -1  0  700;
%                 0  0  0  1   
%             ];
% 

    
      initial_estimate = [ 
         0   0  1  0; 
         -1   0  0   0;
         0   -1 0   700;
         0   0  0   1];

    [anglePose, angleError] = evaluate_pose_ftrac(BBs, Line1_Eq, Line2_Eq, Line3_Eq, Ellipse1_Eq, Ellipse2_Eq, pixel_ratio, screen_size, sourcePosition', focalLength, initial_estimate);

    if isempty(anglePose)
        for i = 1:5
                [anglePose, angleError] = evaluate_pose_ftrac(BBs, Line1_Eq, Line2_Eq, Line3_Eq, Ellipse1_Eq, Ellipse2_Eq, pixel_ratio, screen_size, sourcePosition', focalLength, initial_estimate);
                if i == 3
                    disp('NOT CONVERGED!')
                end
                if ~isempty(anglePose)
                    i = 3;
                end
        end
    end
end

