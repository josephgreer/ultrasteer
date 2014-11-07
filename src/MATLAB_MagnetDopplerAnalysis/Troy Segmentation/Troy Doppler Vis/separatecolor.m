function [ RGB_Image,Color_Image ] = SeparateColor(Image_32bit)
% -------------------------------------------------------------------------
% author: Troy Adebar data: 2012-03-22
%
% title: SeparateColor
%
% summary: This .m file takes an input 2D array of 64-bit double values,
% and separates them into a 3D array of RGB values.
% -------------------------------------------------------------------------

[h,w] = size(Image_32bit);
% Convert the data first to 32-bit (native size from ultrasonix) then
% to 4 x 8-bit numbers (RGBX)
one_slice_8bit = typecast(int32(Image_32bit(:)),'uint8');

% Separate into vectors
b_chan = one_slice_8bit(1:4:end); 
g_chan = one_slice_8bit(2:4:end);
r_chan = one_slice_8bit(3:4:end);

% Reshape the vectors into image files
r_image = reshape(r_chan,h,w);
g_image = reshape(g_chan,h,w);
b_image = reshape(b_chan,h,w);

% Create mask of the gray (non-Doppler) pixels
mask = r_image == g_image & g_image == b_image; % Gray pixels R=G=B

% Concatenate the images into a combined RGB image
RGB_Image = cat(3,r_image,g_image,b_image);
% Zero the gray-scale values in the RGB image to create the doppler image
Color_Image = RGB_Image;
mask3D = repmat(mask,[1 1 3]);
Color_Image(mask3D) = 0;
end
