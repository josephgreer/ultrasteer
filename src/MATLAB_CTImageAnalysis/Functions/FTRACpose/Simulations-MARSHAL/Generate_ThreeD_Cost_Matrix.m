% $Id: Generate_ThreeD_Cost_Matrix.m,v 1.1 2006/03/22 00:39:36 jain Exp $

function Cost_Matrix = Generate_ThreeD_Cost_Matrix(Input_Dir, Image_Combos, Pixel_Size, Center, Focal_Length)

%read the Carm poses and the seed locations
Carm_Poses = [];
for counter = 1 : length(Image_Combos)
    file = strcat(Input_Dir,'\image_transform_', int2str(Image_Combos(counter)), '.txt');
    pose_tmp = load(file, '-ascii');
    Carm_Poses(:,:,counter) = pose_tmp;
end
Image_Seeds = [];
for counter = 1 : length(Image_Combos)
    file = strcat(Input_Dir,'\image_seeds_', int2str(Image_Combos(counter)), '.txt');
    seeds_tmp = load(file, '-ascii');
    seeds_tmp = seeds_tmp(:,1:2);
    Image_Seeds(:,:,counter) = seeds_tmp;
end


Num_Seeds1 = size(Image_Seeds(:,:,1), 1);
Num_Seeds2 = size(Image_Seeds(:,:,2), 1);
Num_Seeds3 = size(Image_Seeds(:,:,3), 1);

Cost_Matrix = [];
for counter1 = 1 : Num_Seeds1
    counter1
    for counter2 = 1 : Num_Seeds2
%        for counter3 = 1 : Num_Seeds3
            seeds1_tmp = repmat(Image_Seeds(counter1,:,1), Num_Seeds3, 1);
            seeds2_tmp = repmat(Image_Seeds(counter2,:,2), Num_Seeds3, 1);
            seeds_tmp = cat(3, seeds1_tmp, seeds2_tmp, Image_Seeds(:,:,3));
            cost_tmp = Compute_PE(seeds_tmp, Carm_Poses, -1, Pixel_Size, Center, Focal_Length);
            cost_tmp = reshape(cost_tmp, 1, 1, Num_Seeds3);
            Cost_Matrix(counter1, counter2, :) = cost_tmp;
%        end
    end
end



% $Log: Generate_ThreeD_Cost_Matrix.m,v $
% Revision 1.1  2006/03/22 00:39:36  jain
% Generates the 3D cost matrix for the input.
%