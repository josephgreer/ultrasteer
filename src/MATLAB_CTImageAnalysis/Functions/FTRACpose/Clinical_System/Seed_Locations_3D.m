% $ID: $

%All the data is in a directory called Data, one level before this
%directory. Each patient has his own directory for saving all the files,
%which inturn has a directory for each component of the problem. The input
%is the patient code (the directory corresponding to which should be
%present with the relevant images) and a list of images to be segmented.
%The choice of these images to be segmented can be cancelled or changed 
%at a later stage. A cancel does not try to segment, a changed filename
%changes the image to be segmented and a 'crossed' image figure means that
%the segmentation has to be rejected. eg: Patient_Code = 'Test1'; 
%Input_Images = [1 3]; images are tiff.

%To do. Automatically add the path of all relevant directories. Add/reject
%after segmentation. Matlav 7 test. Initialization, aquisition, MARSHAL, 
%Calibration, distortion functions. Add different directory for each run.
%Use the 'input' function to take input from user. Iitialisation function
%should be added. each directory should be checked for existance. each file
%should be checked before opening. catch exception when image cannot be
%opened. Change to accomodate for all image formats.
function Seed_coordinates = Seed_Locations_3D(Patient_Code, Input_Images)

%Initialisations.
Patient_Directory = strcat('../Data/Patient_', Patient_Code, '/');
%All these directory creations should be in the aquisition function.
%mkdir(Patient_Directory);
%mkdir( strcat(Patient_Directory, 'Carm_Parameters') );
%mkdir( strcat(Patient_Directory, 'Aquired_Images') );
%mkdir( strcat(Patient_Directory, 'Distortion_Corrected_Images') );
mkdir( strcat(Patient_Directory, 'Seed_Segmentation') );
mkdir( strcat(Patient_Directory, 'FTRAC_Segmentation') );
mkdir( strcat(Patient_Directory, 'Carm_Poses') );
mkdir( strcat(Patient_Directory, '3D_Seed_Locations') );

Num_Images = length(Input_Images);
Segmentation_Successful = -1*ones(1, Num_Images);

%Segmentation.
%Segment each image and print answer. If image selection is cancelled, move
%on to next image. If a different image is selected, change that to be the
%truth appropriately. If a image is selected but cancelled during
%segmentation, don't save the answer. If it is fully correctly segmented,
%then return the answer, save the output and mark the segmented flag as 1.
image_counter = 0;
while(image_counter < Num_Images)
    image_counter = image_counter + 1;
    image_number = Input_Images(image_counter);
    disp( ['Segmentation started for image ', int2str(image_number), '.'] );
    
    %Open file and check if same.
    [Image_Filename PATH] = uigetfile('*', 'Choose an Image', strcat(Patient_Directory, 'Distortion_Corrected_Images/', int2str(image_number), '.tiff'));
    if (Image_Filename == 0)
        disp( ['Segmentation cancelled for image ', int2str(image_number), '. Ignoring this image.'] );
        continue;
    end
    opened_image_number = str2num(strrep(Image_Filename, '.tiff', []));
    if(opened_image_number~=image_number)
        disp( ['Opened image ', int2str(opened_image_number), ' instead of image ', int2str(image_number), '. Replacing old image with new image.']);
        Input_Images(image_counter) = opened_image_number;
        image_number = opened_image_number;
    end
    
    % Open image.
    Img = imread( strcat(PATH, Image_Filename) );
    Img = squeeze(Img(:,:,1));
    
    %Segmentation
    [Figure_Handle Segmentation_Flag Seeds BBs Line1_Eq Line2_Eq Line3_Eq Ellipse1_Eq Ellipse2_Eq] = SeedandFiducialSegmentationGUI(Img);
    
    %Check segmentation.
    if(Segmentation_Flag==1)
        disp( ['Segmentation successfully completed for image ', int2str(image_number), '.'] );
        Segmentation_Successful(image_counter) = 1;
        pause(1);
        close(Figure_Handle);
    else
        disp( ['Segmentation failed for image ', int2str(image_number), '.'] );
        Segmentation_Successful(image_counter) = 0;
        continue;
    end
    
    %Save
    save( strcat(Patient_Directory, 'Seed_Segmentation/', 'Seeds_Carm_Image_', int2str(image_number), '.txt'), 'Seeds', '-ascii' );
    save( strcat(Patient_Directory, 'FTRAC_Segmentation/', 'BBs_Carm_Image_', int2str(image_number), '.txt'), 'BBs', '-ascii' );
    save( strcat(Patient_Directory, 'FTRAC_Segmentation/', 'Line1_Carm_Image_', int2str(image_number), '.txt'), 'Line1_Eq', '-ascii' );
    save( strcat(Patient_Directory, 'FTRAC_Segmentation/', 'Line2_Carm_Image_', int2str(image_number), '.txt'), 'Line2_Eq', '-ascii' );
    save( strcat(Patient_Directory, 'FTRAC_Segmentation/', 'Line3_Carm_Image_', int2str(image_number), '.txt'), 'Line3_Eq', '-ascii' );
    save( strcat(Patient_Directory, 'FTRAC_Segmentation/', 'Ellipse1_Carm_Image_', int2str(image_number), '.txt'), 'Ellipse1_Eq', '-ascii' );
    save( strcat(Patient_Directory, 'FTRAC_Segmentation/', 'Ellipse2_Carm_Image_', int2str(image_number), '.txt'), 'Ellipse2_Eq', '-ascii' );
end

%Carm parameters for distorted case. Should read from a dumped input file
%from the directory Carm_Parameters.
pixel_ratio = [2.306 2.412];
screen_size = [512 640];
origin = [256, 320];
focal_len = 1000;
initial_estimate = [ 
    0  0 -1  0; 
    1  0  0  0;
    0 -1  0  700;
    0  0  0  1   
];

%Pose estimation.
%Check if this image was correctly segmented or not. Ignore if not
%segmented fully. If segmented fully, read from appropriate file and then
%call the FTRAC code. If the retuned pose is valid, save it into file.
%Display messages at each step.
warning off
image_counter = 0;
while(image_counter < Num_Images)
    image_counter = image_counter + 1;
    image_number = Input_Images(image_counter);
    
    %Check.
    if(Segmentation_Successful(image_counter)~=1)
        disp( ['Pose estimation being cancelled for image ', int2str(image_number), '.'] );
        continue;
    end

    %Open files.
    BBs = load( strcat(Patient_Directory, 'FTRAC_Segmentation/', 'BBs_Carm_Image_', int2str(image_number), '.txt'), 'BBs', '-ascii' );
    Line1_Eq = load( strcat(Patient_Directory, 'FTRAC_Segmentation/', 'Line1_Carm_Image_', int2str(image_number), '.txt'), 'Line1_Eq', '-ascii' );
    Line2_Eq = load( strcat(Patient_Directory, 'FTRAC_Segmentation/', 'Line2_Carm_Image_', int2str(image_number), '.txt'), 'Line2_Eq', '-ascii' );
    Line3_Eq = load( strcat(Patient_Directory, 'FTRAC_Segmentation/', 'Line3_Carm_Image_', int2str(image_number), '.txt'), 'Line3_Eq', '-ascii' );
    Ellipse1_Eq = load( strcat(Patient_Directory, 'FTRAC_Segmentation/', 'Ellipse1_Carm_Image_', int2str(image_number), '.txt'), 'Ellipse1_Eq', '-ascii' );
    Ellipse2_Eq = load( strcat(Patient_Directory, 'FTRAC_Segmentation/', 'Ellipse2_Carm_Image_', int2str(image_number), '.txt'), 'Ellipse2_Eq', '-ascii' );
    
    %Pose
    [FTRAC_Pose FTRAC_Error] = evaluate_pose_ftrac(BBs, Line1_Eq, Line2_Eq, Line3_Eq, Ellipse1_Eq, Ellipse2_Eq, pixel_ratio, screen_size, origin, focal_len, initial_estimate);
    
    %Check
    if(isempty(FTRAC_Pose))
        disp( ['Pose estimation failed for image ', int2str(image_number)] );
    else
        disp( ['Successful pose estimation for image ', int2str(image_number), ' has a mean residual error of : ', num2str(mean(FTRAC_Error))] );
    end
    
    %Save.
    save( strcat(Patient_Directory, 'Carm_Poses/', 'Pose_Image_', int2str(image_number), '.txt'), 'FTRAC_Pose', '-ascii' );
    save( strcat(Patient_Directory, 'Carm_Poses/', 'Residual_Error_Image_', int2str(image_number), '.txt'), 'FTRAC_Error', '-ascii' );
end

%Appropriately call MARSHAL with Segmentation_Successful and other
%filenames.
disp('MARSHAL being called.');
Seed_coordinates = [-1];

% $Log: Seed_Locations_3D.m,v $
% Revision 1.3  2005/12/29 01:08:52  jain
% Added comments.
%
% Revision 1.2  2005/12/28 02:51:12  jain
% A non commented version of code that runs. Have tested it on the two
% datasets. Will show it to Anton tomorrow before leaving. Found some cases
% which I have emailed Siddharth. Aquisition and MARSHAL still need to be
% added.
%
% Revision 1.1  2005/12/27 19:36:16  jain
% First version of a clinical integration. There are still many bugs that I am fixing.
%
