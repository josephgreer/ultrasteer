%$Id: ftraclines_to_templatelines.m,v 1.3 2006/11/29 00:48:49 jain Exp $
%Function to get the transform from the FTRAC lines to the template lines.
%Just to validate with the fabrication design. A combination of generate
%ground truth and line_registration, except that the ground truth is wrt
%the frame of the lines and not wrt to the ftrac/template. This code is
%same as the last part of FTRAC_to_template_calibration code. Input is the
%lines (3x2xN) and the output is a 4x4 transform.
function xform_ftraclines_to_templatelines = ftraclines_to_templatelines(segmented_lines);

%ground truth  the way we want it.
true_lines = [];

true_lines = cat(3, true_lines, [-20 0 0; 0 1 0]');
true_lines = cat(3, true_lines, [0 20 0; 1 0 0]');
true_lines = cat(3, true_lines, [20 0 0; 0 1 0]');
true_lines = cat(3, true_lines, [0 -20 0; 1 0 0]');

true_lines = cat(3, true_lines, [0 35 0; 0 1 0]');
true_lines = cat(3, true_lines, [35 0 0; 1 0 0]');
true_lines = cat(3, true_lines, [80 35 0; 0 1 0]');

%registration step.
xform_ground_to_ftraclines = Line_registration(true_lines(:,:,1:4), segmented_lines(:,:,1:4));
xform_ground_to_templatelines = Line_registration(true_lines(:,:,5:7), segmented_lines(:,:,5:7));
xform_ftraclines_to_templatelines = xform_ground_to_templatelines*inv(xform_ground_to_ftraclines);

%$Log: ftraclines_to_templatelines.m,v $
%Revision 1.3  2006/11/29 00:48:49  jain
%Have changed the file that loads the new template as compared to the old one.
%Also have updated the line registration function to incorporate a) reflections in Arun's
%method, b) symmetric registrations that can happen with 3 lines, c) also look at
%residual errors after matching just the rotations.
%
%Revision 1.2  2005/12/14 21:36:47  jain
%Added the input and output formats in the comments.
%
%Revision 1.1  2005/12/12 23:16:22  jain
%Commiting the first complete version of the code for registering the FTRAC fiducial to the template. Code works and has been tested.
%

