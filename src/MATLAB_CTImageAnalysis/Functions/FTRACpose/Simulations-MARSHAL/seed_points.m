% $Id: seed_points.m,v 1.2 2007/06/28 16:33:09 jain Exp $
function C = seed_points(Rad, len, N1, N2, N3)

C = [];

%Write a loop for the angle, movement along the axis and
%then along the radius. Calculate the coordinate of each point
%and the volume it represents. Save this info as a new row.
for count1 = 0 : 2*pi/N1 : (2*pi - 0.01)
    for count2 = -len/2 : len/N2 : len/2
        for count3 = 0 : Rad/N3 : Rad
            C = [C ; count3*cos(count1) count3*sin(count1) count2 8*(count3 + (Rad/N3))*(Rad/N3)*(2*pi/N1)*(len/N2)]; 
        end
    end
end

% $Log: seed_points.m,v $
% Revision 1.2  2007/06/28 16:33:09  jain
% Have made some small changes:- (1) negated the image, (2) decreased the
% gaussian blurring, (c) halfed the seed size, (d) changed the location of the seeds,
% (e) increased the resolution of the seed voxelization.
%
% Revision 1.1  2005/12/21 00:19:17  jain
% The first version of a wrking code, including seed orientation and our chosen final coordinate frame.
%
