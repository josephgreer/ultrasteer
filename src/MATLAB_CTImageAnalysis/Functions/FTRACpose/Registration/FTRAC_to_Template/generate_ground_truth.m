%$Id: generate_ground_truth.m,v 1.3 2006/11/29 00:48:49 jain Exp $
%function to generate the ground truth locations of the 3D lines wrt to
%each frame. The first 4 are the FTRAC lines while the last three are the
%template ones. Output is 3x2xN matrix for the lines.
function true_lines = generate_ground_truth()

true_lines = [];

true_lines = cat(3, true_lines, [-20 0 70; 0 1 0]');
true_lines = cat(3, true_lines, [0 20 70; 1 0 0]');
true_lines = cat(3, true_lines, [20 0 70; 0 1 0]');
true_lines = cat(3, true_lines, [0 -20 70; 1 0 0]');

% The lines for the new template.
true_lines = cat(3, true_lines, [-10   30   -15.875;  0 1 0]');
true_lines = cat(3, true_lines, [ 30   65   -15.875;  1 0 0]');
true_lines = cat(3, true_lines, [ 70   30   -15.875;  0 1 0]');

% The lines for the old template.
% true_lines = cat(3, true_lines, [-15  30    -12.26;  0 1 0]');
% true_lines = cat(3, true_lines, [30   -2.5  -12.26;  1 0 0]');
% true_lines = cat(3, true_lines, [75   30    -12.26;  0 1 0]');

%$Log: generate_ground_truth.m,v $
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
