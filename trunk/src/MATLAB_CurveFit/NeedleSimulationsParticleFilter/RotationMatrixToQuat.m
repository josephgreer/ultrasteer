function q = RotationMatrixToQuat(R)
theta = acos((trace(R)-1)/2);
if(theta == 0)
    q = [1; 0; 0; 0];
    return;
end
omega = 1/(2*sin(theta))*[R(3,2)-R(2,3);...
                         R(1,3)-R(3,1);...
                         R(2,1)-R(1,2)];
                     
q = [cos(theta/2); sin(theta/2)*omega];                     
% q(4) = sqrt(1+R(1,1)+R(2,2)+R(3,3))/2;
% q(1) = (R(3,2)-R(2,3))/(4*q(4));
% q(2) = (R(1,3)-R(3,1))/(4*q(4));
% q(3) = (R(2,1)-R(1,2))/(4*q(4));
% q = q';
end