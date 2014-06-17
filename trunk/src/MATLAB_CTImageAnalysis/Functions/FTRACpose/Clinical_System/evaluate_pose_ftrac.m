% $ID: $

%PAss the segmented features as input in this order and seg the pose value.
%The BBs are 9x3, lines each 1x3, ellipses each 6x1. Pose is 4x4 and Error
%is num of features x 1.
function [Pose, Error] = evaluate_pose_ftrac(BBLocations, lLine1PointsEq, lLine2PointsEq, lLine3PointsEq, ellipse_1_eq, ellipse_2_eq, pixel_ratio, screen_size, origin, focal_len, initial_estimate)

%The definition of our FTRAC fiducial.
%D = 1.2*25.4;
% D = 30;
% th = 30*pi/180;

% ANN CHANGED FOR SMALLER FTRAC
D = 20;
th = 30*pi/180;
[all_model_pts all_model_line_pts model_ellipse1_pts model_ellipse2_pts] = Fiducial_points(D, th);

% invalid/unfound BBs (-1 value) are deleted from the list of model points.
ValidIndices = find (BBLocations(:,1) >= 0);
seg_pts = BBLocations(ValidIndices, :);
model_pts = all_model_pts(ValidIndices, :);

%Invalid/unsegmented lines ([] matrix) are deleted from the list of model lines.
seg_lns = [];
model_line_pts = [];
if(size(lLine1PointsEq,1)==1)
    seg_lns = [seg_lns; lLine1PointsEq; lLine1PointsEq];
    model_line_pts = [model_line_pts; all_model_line_pts(1:2,:)];
end
if(size(lLine2PointsEq,1)==1)
    seg_lns = [seg_lns; lLine2PointsEq; lLine2PointsEq];
    model_line_pts = [model_line_pts; all_model_line_pts(3:4,:)];
end
if(size(lLine3PointsEq,1)==1)
    seg_lns = [seg_lns; lLine3PointsEq; lLine3PointsEq];
    model_line_pts = [model_line_pts; all_model_line_pts(5:6,:)];
end

%Invalid/unsegmented ellipses ([] matrix) are deleted from the list of
%model ellipses.
seg_els = [];
if (size(ellipse_1_eq,1)==1)
    if (size(ellipse_2_eq,1)==1)
        seg_els = [ellipse_1_eq; ellipse_2_eq];
        model_ellipse1_pts = model_ellipse1_pts;
        model_ellipse2_pts = model_ellipse2_pts;
    elseif (size(ellipse_2_eq,1)==0)
        seg_els = [ellipse_1_eq];
        model_ellipse1_pts = model_ellipse1_pts;
        model_ellipse2_pts = [];
    else
        disp('Ellipse 2 seems to have a segmentation problem.');
    end
elseif (size(ellipse_1_eq,1)==0)
    if (size(ellipse_2_eq,1)==1)
        seg_els = [ellipse_2_eq];
        model_ellipse1_pts = model_ellipse2_pts;
        model_ellipse2_pts = [];
    elseif (size(ellipse_2_eq,1)==0)
        seg_els = [];
        model_ellipse1_pts = [];
        model_ellipse2_pts = [];
    else
        disp('Ellipse 2 seems to have a segmentation problem.');
    end
else
    disp('Ellipse 1 seems to have a segmentation problem.');
end
    
%Call the pose function and return the answer.    
[Pose Error] = Estimate_Pose(model_pts, seg_pts, model_line_pts, seg_lns, model_ellipse1_pts,  model_ellipse2_pts, seg_els, pixel_ratio, screen_size, origin, focal_len, initial_estimate);

% $Log: evaluate_pose_ftrac.m,v $
% Revision 1.4  2007/07/15 20:32:49  anton
% Clinical_System: Code updated to reflect new dimensions in FTRAC
%
% Revision 1.3  2005/12/29 01:12:11  jain
% Added comments.
%
% Revision 1.2  2005/12/28 02:49:41  jain
% Changed the code to take care of cases when only partial features of the
% FTRAC fiducial are segmented. Also Carm parameters are an input now.
%
% Revision 1.1  2005/12/27 19:36:16  jain
% First version of a clinical integration. There are still many bugs that I am fixing.
%
