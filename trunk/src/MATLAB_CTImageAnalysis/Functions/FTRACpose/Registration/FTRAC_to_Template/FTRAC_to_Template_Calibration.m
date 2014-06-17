%$Id: FTRAC_to_Template_Calibration.m,v 1.4 2006/11/29 00:48:49 jain Exp $

%function to calibrate the relationship between the FTRAC fiducial and the
%template  using the optotrak. 7 lines are taken in any order using a
%calibrated optotrak ball tip pointer. The input is a file with all the
%points, while the output is the transformation taking any point in the
%ftrac frame to its coordinate in the template frame. residual err is how
%well the measured lines fit the theoretical lines. A figure of all the frames
%, etc. is available in the project development folder with Ameet.
function [xform_ftrac_to_template, final_res_err, xform_ftraclines_to_templatelines] =  FTRAC_to_Template_Calibration(input_file)

%General constants and thresholds. LET is in in mm. LEt is a notion of the
%tolerable residual err while fitting 3D lines. Num_INITIAL_POINTS is for
%the number of points used to start the iterative process of fitting a
%3D line. ball radius is the radius of the ball tip.
LINE_ERR_THRESHOLD = 1;
NUM_INITIAL_POINTS = 10;
BALL_RADIUS = 0.5;

%load the data and display it.
input_pts = load(input_file);
figure, hold on
plot3(input_pts(:,1), input_pts(:,2), input_pts(:,3), 'y.')
pause

%extract all the 3D lines from the data. (seven). A line is charecterized
%by a point on it (column 1), its normalised direction vector (column2).
%All the lines are put one after the other in dimension 3.
[segmented_lines final_pts line_fit_err] = extract_lines(input_pts, LINE_ERR_THRESHOLD, NUM_INITIAL_POINTS);

%Establish correspondence and order in the 3D lines. The order 1-7 is in
%the figure page with Ameet. Make a compensation for the ball tip and for
%fitting all the lines into their respective planes.
segmented_lines = establish_line_correspondence(segmented_lines, final_pts, LINE_ERR_THRESHOLD);
segmented_lines = ball_tip_compensation(segmented_lines, BALL_RADIUS);

%The true 3D locations of the lines according to their respective frames.
true_lines = generate_ground_truth;

%Compute the location of the template frame and the FTRAc frame with
%respect to the optotrak (ground) frame. Then use these to compute the
%FTRAC and template registration.
[xform_ground_to_ftrac err1] = Line_registration(true_lines(:,:,1:4), segmented_lines(:,:,1:4));
[xform_ground_to_template err2] = Line_registration(true_lines(:,:,5:7), segmented_lines(:,:,5:7));
xform_ftrac_to_template = xform_ground_to_template*inv(xform_ground_to_ftrac);
final_res_err = (4*err1 + 3*err2)/7;

%Just some simple code to get the transformation between the FTRAC and
%template lines. This can easily be verified to 1mm precision by the
%fabrication design.
% xform_ftraclines_to_templatelines = ftraclines_to_templatelines(segmented_lines);

%$Log: FTRAC_to_Template_Calibration.m,v $
%Revision 1.4  2006/11/29 00:48:49  jain
%Have changed the file that loads the new template as compared to the old one.
%Also have updated the line registration function to incorporate a) reflections in Arun's
%method, b) symmetric registrations that can happen with 3 lines, c) also look at
%residual errors after matching just the rotations.
%
%Revision 1.3  2006/03/23 21:43:57  jain
%Changed the radius of the ball tip.
%
%Revision 1.2  2005/12/14 20:11:03  jain
%Removed the ball radious comment.
%
%Revision 1.1  2005/12/12 23:16:22  jain
%Commiting the first complete version of the code for registering the FTRAC fiducial to the template. Code works and has been tested.
%
