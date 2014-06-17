%Function to return the 4x4 transformation such that one refernce frame is
%fixed and the other reference frame is sitting on a cone centered at the
%first frame. The z-axis of the second frame is always away from the
%center. Cone angle is the half angle of the cone, center angle is the
%location of the second frame on the outer rim of the cone (0-2*pi),
%z-angle is the rotation along the z-axis of the second frame, and
%translation defines the center of the cone.
function xform = cone_pose(cone_angle, center_angle, z_angle, translation)

%A rotation matrix that I derived on paper. This achieves the necessary
%transformation.
denom = sqrt(1+ (tan(cone_angle)*cos(center_angle))^2);
inv_rot = [1/denom                                                                      (tan(cone_angle)*cos(center_angle))/denom    0 ;
           -(sin(cone_angle)*tan(cone_angle)*cos(center_angle)*sin(center_angle))/denom (sin(cone_angle)*sin(center_angle))/denom   (sin(cone_angle)*tan(cone_angle)*(cos(center_angle)^2) + cos(cone_angle))/denom ;
           sin(cone_angle)*cos(center_angle)                                            -cos(cone_angle)                            sin(cone_angle)*sin(center_angle)]';

%get the rotation matrix. Rotate along the z-axis. Get the translation 
%values...and return the answer.              
xform(1:3,1:3) = inv(inv_rot);
z_rot_xform = xyz_xform([0 0 z_angle]', [0 0 0]');
xform(1:3,1:3) = z_rot_xform(1:3,1:3)*xform(1:3,1:3);
xform(1:3, 4) = translation;
xform(4,1:4) = [0 0 0 1];
    
           