% $Id: Compute_PE.m,v 1.1 2006/03/22 00:39:17 jain Exp $

function Projection_Errs = Compute_PE(Seeds_Image, Carm_Poses, Orig_Scale, Pixel_Size, Center, Focal_Length)

Num_Points = size(Seeds_Image,1);
Num_Images = size(Seeds_Image,3);
Computed_ThreeD_Locations = Compute_3D_Locations(Seeds_Image, Carm_Poses, Pixel_Size, Center, Focal_Length);
if(Orig_Scale==-1)
    Scale = 1;
else
    Scale = (Orig_Scale*abs(Focal_Length))/norm(Computed_ThreeD_Locations(1,:));
end
Computed_ThreeD_Locations = Scale*Computed_ThreeD_Locations;

Projection_Errs = [];
for counter_image = 1 : Num_Images
    Proj_Matrix = [Focal_Length 0 0 0; 0 Focal_Length 0 0; 0 0 1 0]*Carm_Poses(:,:,counter_image);
    proj_points = Proj_Matrix*([Computed_ThreeD_Locations ones(Num_Points, 1)])';
    proj_points = [proj_points(1,:)./proj_points(3,:) ; proj_points(2,:)./proj_points(3,:)]';
    all_points = (Seeds_Image(:,:,counter_image) - repmat(Center, Num_Points, 1)).*repmat(Pixel_Size, Num_Points,1);
    num_proj_points = size(proj_points,1);
    err_points = (proj_points - all_points);
    Projection_Errs = [Projection_Errs err_points(:,1) err_points(:,2)];
end

if(Orig_Scale==-1)
    Projection_Errs = reshape(Projection_Errs, Num_Points, 2*Num_Images);
    Projection_Errs = sqrt(sum(Projection_Errs.^2, 2));
else
    Projection_Errs = reshape(Projection_Errs, Num_Points*2*Num_Images, 1);
end

% $Log: Compute_PE.m,v $
% Revision 1.1  2006/03/22 00:39:17  jain
% Computes the PE for the seeds.
%

