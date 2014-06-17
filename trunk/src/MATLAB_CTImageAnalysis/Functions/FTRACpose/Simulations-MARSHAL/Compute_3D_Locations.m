% $Id: Compute_3D_Locations.m,v 1.1 2006/03/22 00:38:58 jain Exp $

function ThreeD_Locations = Compute_3D_Locations(Image_Seeds, Carm_Poses, Pixel_Size, Center, Focal_Length);

ThreeD_Locations = [];
for counter_point = 1 : size(Image_Seeds,1)
    sum_A = zeros(3);
    sum_Ap = zeros(3,1);
    for counter_image = 1 : size(Image_Seeds, 3)
        xform = inv(Carm_Poses(:, :, counter_image));
        line = xform(1:3,1:3)*[(Image_Seeds(counter_point,:,counter_image) - Center).*Pixel_Size Focal_Length]';
        point = xform(1:3, 4);
        skew_line = [0 -line(3) line(2); line(3) 0 -line(1); -line(2) line(1) 0];
        sum_A = sum_A + (skew_line')*skew_line;
        sum_Ap = sum_Ap + ((skew_line')*skew_line)*point;
    end
    ThreeD_Locations = [ThreeD_Locations; (inv(sum_A)*sum_Ap)'];
end

% $Log: Compute_3D_Locations.m,v $
% Revision 1.1  2006/03/22 00:38:58  jain
% Computes the 3D location of a seed, whereever it might be.
%