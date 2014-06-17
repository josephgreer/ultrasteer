% $Id: rufPoseIsComputed.m,v 1.1 2006/09/01 01:46:15 anton Exp $

% check if the pose has been computed for an image
function poseIsComputed = rufPoseIsComputed(directory, image)
    [pathstr, imagePrefix] = fileparts(image);
    poseFile = length(dir(strcat(directory, '/Carm_Poses/Pose_', imagePrefix, '.txt')));
    
    poseIsComputed = (poseFile == 1);