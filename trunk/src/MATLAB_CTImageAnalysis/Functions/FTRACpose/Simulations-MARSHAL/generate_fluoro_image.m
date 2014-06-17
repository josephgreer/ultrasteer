% $Id: generate_fluoro_image.m,v 1.3 2007/06/28 16:33:09 jain Exp $

%Generates a simulated fluoro image for the object passed. returns the seed and the
%centers of each object.
function [image , centers] = generate_fluoro_image(Grnd_2_fluoro_xform, object_points, object_positions, object_rotations, pixel_ratio, screen_size, origin, focal_len)

%initialize the image
image = zeros(screen_size);

%For adding the seeds at various transformation
centers = [];
for count = 1 : size(object_positions, 1)
    object_2_grnd_xform = [ squeeze(object_rotations(1:3, 1:3, count)) object_positions(count, :)' ; 0 0 0 1];
    [image tmp_pt] = add_one_object(object_points, image, Grnd_2_fluoro_xform, object_2_grnd_xform, pixel_ratio, screen_size, origin, focal_len);
    if (size(object_points, 1)==1)
        centers = [centers ; tmp_pt];
    end

end


%Get the centers if only one input point was passed, else smooth out the whole image
%with a gaussian
if (size(object_points, 1)~=1)
    image = conv2(image, fspecial('gaussian', 2, 0.15), 'same');
end
image = 1 - image;

%Mark the image centerlines
% image(origin(1)-1:origin(1)+1, 1:20:end) = 0.2;
% image(1:25:end, origin(2)-1:origin(2)+1) = 0.2;

% $Log: generate_fluoro_image.m,v $
% Revision 1.3  2007/06/28 16:33:09  jain
% Have made some small changes:- (1) negated the image, (2) decreased the
% gaussian blurring, (c) halfed the seed size, (d) changed the location of the seeds,
% (e) increased the resolution of the seed voxelization.
%
% Revision 1.2  2006/02/02 16:48:37  jain
% Checked for orientation errors. Added a function to check for orientation errors.
% Also the Gaussian averaging of the image added boundary to the image, making
% the segmentation information incorrect - have changed this. Added another input
% for the number of equal spaced projections required.
%
% Revision 1.1  2005/12/21 00:19:17  jain
% The first version of a wrking code, including seed orientation and our
% chosen final coordinate frame.
%
