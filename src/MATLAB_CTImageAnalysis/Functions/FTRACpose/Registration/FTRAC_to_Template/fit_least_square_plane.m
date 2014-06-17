%$Id: fit_least_square_plane.m,v 1.2 2005/12/14 21:36:47 jain Exp $

%Function to fit a least square plane to a set of lines. if a.n + d = 0 is
%the equation and line p + k*v lies on it, then a.p+d = 0 and a.v =
%0...thus two lines are sufficient. Input is 3x2xN for lines and 4x1 for
%the output plane.
function [lsq_plane, res_err] = fit_least_square_plane(lines);

num_lines = size(lines,3);

A = [];
b = [];
for line_counter = 1 : num_lines
    A = [A ; squeeze(lines(:,:,line_counter))'];
    b = [b ; -1; 0];
end
x = A\b;
res_err = mean(abs(A*x - b));

lsq_plane = [x ; 1];
lsq_plane = lsq_plane/sqrt(sum(lsq_plane(1:3).^2));

%$Log: fit_least_square_plane.m,v $
%Revision 1.2  2005/12/14 21:36:47  jain
%Added the input and output formats in the comments.
%
%Revision 1.1  2005/12/12 23:16:22  jain
%Commiting the first complete version of the code for registering the FTRAC fiducial to the template. Code works and has been tested.
%
