function T = xyz_xform(angles, P)

%Produces the frame transformation. To get the new frame, rotate along x-axis
%,then by the new y axis and then by the newest z-axis. What is left is the 
%final frame. The matrix converts any point in the old frame to a point in the
%new frame.

x = angles(1);
y = angles(2);
z = angles(3);

R = [cos(z) sin(z) 0 ; -sin(z) cos(z) 0 ; 0 0 1]*[cos(y) 0 -sin(y) ; 0 1 0 ; sin(y) 0 cos(y)]*[1 0 0;0 cos(x) sin(x);0 -sin(x) cos(x)];
T = [R P ; 0 0 0 1];