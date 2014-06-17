% $Id: add_one_object.m,v 1.1 2005/12/21 00:19:17 jain Exp $
%Add the projection of a single object at a known position
function [image, pt] = add_one_object(object_pts, image, Grnd_2_fluoro_xform, object_2_grnd_xform, pixel_ratio, screen_size, origin, focal_len)

%calculate the object transform to the camera frame. Then transform all the object points
%into the camera frame. From these obtain the pixels that would be projections of these points.
object_2_fluoro_xform = Grnd_2_fluoro_xform*object_2_grnd_xform;
pts_in_fluoro_frame = object_2_fluoro_xform*[object_pts(:,1:3)' ; ones(1, size(object_pts, 1))];
pixels_unrounded = create_image_pixels(pixel_ratio, screen_size, focal_len, origin, [pts_in_fluoro_frame(1:3, :)' object_pts(:,4)]);
pixels = [round(pixels_unrounded(:,1:2)) pixels_unrounded(:,3)]; 

%If the projections are within the image, add the weight of the projection to that pixel, i.e. the value.
for count = 1 : size(pixels, 1)
    if ( sum((pixels(count, 1:2)>[0 0]) + (pixels(count, 1:2)<=size(image)))==4)
        image(pixels(count, 1), pixels(count, 2)) = image(pixels(count, 1), pixels(count, 2)) + pixels(count, 3);
    end
end

if(size(size(object_pts, 1)==1))
    pt = pixels_unrounded(1,1:2);
else
    pt = -1;
end

% $Log: add_one_object.m,v $
% Revision 1.1  2005/12/21 00:19:17  jain
% The first version of a wrking code, including seed orientation and our chosen final coordinate frame.
%


