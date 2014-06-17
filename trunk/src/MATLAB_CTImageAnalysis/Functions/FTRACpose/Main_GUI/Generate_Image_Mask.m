
%Function to compute the mask on an X-ray image. The input is a sample
%image that is obtained from the C-arm/video signal in matrix form. The
%output is a binary image of the same size - 0 being where the outer pixels
%should be removed and 1 which should be presered. Ideally a bank image
%should be taken to obtain the best mask. Also the mask removes 1-2 pixels
%on the outside of the intensifier just for safety (last third line in the
%code)
function Image_Mask = Generate_Image_Mask(Input_Image)

%Rescale the image
Input_Image = double(Input_Image);
Input_Image = Input_Image/max(max(Input_Image));

%Figure out a decent threshold to segment the backgound. This is called as an
%approximate mask. The threshold is a litle more than the maximum intensity
%bin.
num_bins = 100;
intensity_histogram = hist(reshape(Input_Image, prod(size(Input_Image)), 1), num_bins);
max_index = find(intensity_histogram == max(intensity_histogram));
threshold = (max_index+4)/num_bins;
approx_mask = Input_Image<threshold;

%Fine tune the background by connecting all the 4-connected objects in this
%thresholded binary image. Then choose the largest area which has a low
%average intensity. This will remove any noisy region from somewhere else
%from being part of the mask.
labels = bwlabel(approx_mask, 4);
label_histogram = hist(reshape(labels, prod(size(labels)), 1), max(max(labels))+1);
index1 = find(label_histogram==max(label_histogram));
region1 = index1 - 1;
label_histogram(index1) = 0;
index2 = find(label_histogram==max(label_histogram));
region2 = index2 - 1;
intensity1 = (sum(sum((labels==region1).*Input_Image)))/sum(sum(labels==region1));
intensity2 = (sum(sum((labels==region2).*Input_Image)))/sum(sum(labels==region2));
if(intensity1 < intensity2)
    region = region1;
else
    region = region2;
end
approx_mask = (labels==region);
approx_mask = 1-approx_mask;

%On this largest connected region, run an imopen operation using a large
%disk. This would remove all the writing on the sides. The result is a
%mask. Do a small erosion to decrease the size of the mask a little to
%smooth it out.
structuring_element = strel('disk', 10);
approx_mask = imopen(approx_mask, structuring_element);
structuring_element = strel('disk', 2);
approx_mask = imerode(approx_mask, structuring_element);

%return the answer.
Image_Mask = approx_mask;