% $Id: AcquireVideoImages.m,v 1.1 2006/01/30 22:24:51 gouthami Exp $

function AcquireVideoImages(ParentDirName, PatientId)
% function AcquireVideoImages(ParentDirName, PatientId)
% AcquireVideoImages will grab and dewarp the images acquired through a
% frame grabber. This function will create a directory for the patient,
% grabs the images and puts them in acquired_images directory, undistorts
% them and saves the undistorted images into dewarped_images directory
% within the patient directory. 
% @param ParentDirName is the 'data' directory (upperlevel directory to
% store the data.
% @param PatientId is the unique identifier for each patient.

% create a directory for PatientId
dirSeparator = '\';
% create a directory for each patient
patientDirName = strcat(ParentDirName, dirSeparator, PatientId);
dirExists = isdir(patientDirName);
if (~dirExists)
    mkdir(patientDirName);
end
% create a directory to store the video acquired images
IMAGE_ACQUISITION_DIRECTORY_NAME = 'Acquired_Images';
imageAcquisitionDirName = strcat(patientDirName, IMAGE_ACQUISITION_DIRECTORY_NAME);
dirExists = isdir(imageAcquisitionDirName);
if (~dirExists)
    mkdir(imageAcquisitionDirName);
end

% create a directory to store the dewarped images
DEWARPED_IMAGES_DIRECTORY_NAME = 'Dewarped_Images';
dewarpedImagesDirName = strcat(patientDirName, DEWARPED_IMAGES_DIRECTORY_NAME);
dirExists = isdir(dewarpedImagesDirName);
if (~dirExists)
    mkdir(dewarpedImagesDirName);
end

% acquire images
MENU_INPUT_TO_CAPTURE_IMAGE = 'c';
MENU_INPUT_TO_QUIT = 'q';
menu = sprintf('Enter\n  %s to capture image \n  %s to quit', MENU_INPUT_TO_CAPTURE_IMAGE, MENU_INPUT_TO_QUIT);
imageNum = 1;
imageID = strcat(PatientId, 'IMAGE');
vid = videoinput('winvideo', 1, 'RGB32_720x480');
start(vid);
while (1)
    user_entry = input(menu, 's');
    switch user_entry
        case MENU_INPUT_TO_CAPTURE_IMAGE
            % data = getData(vid, 10) will grab 10 images, data is a 4D
            % array, with the first three dimensions representing r,g,b.
            data = getData(vid, 1);
            % for the gray scale images, the r,g,b components are all the
            % same, here we are using the first component as our image.
            im = data(:,:,1);
            figure; imshow(im); title('Video Acquired Image');

            % write this image as a tif file in the
            filename = sprintf('%s_%40d', imageID, imageNum);
            absFilename = strcat(imageAcquisitionDirName, dirSeparator, filename);
            imwrite(im, absFilename,'tif');

            % dewarp the image. This step for now assumes a single
            % dewarping function for all the poses and assumes the
            % polynomial coefficients are stored in a file.
            disp('dewarping the image');
            im = imread(absFileName);
            im = im2double(im);
            % read the polynomial coefficients from the file
            dewarpFilename = 'Polycoeff.txt';
            [PolyOrder, PolyCoeff] = ReadDewarpCoefficientsFrmFile(dewarpFilename);
            dewarpIm = DewarpImage(im, PolyOrder, PolyCoeff);
            
            % alternatively, we can dewarp using mapped coordinates
            % dewarpIm = DewarpImageFromMappedCoordinates(dewarpFilename, im);
                        
            % write the dewarped image to dewarped_images directory
            DEWARPED_IMAGE_FILENAME_TAG = 'dewarped';
            filename = sprintf('%s_%s_%40d', imageID, DEWARPED_IMAGE_FILENAME_TAG, imageNum);
            absFileName = strcat(dewarpedImagesDirName, dirSeparator, filename);
            imwrite(im, absFilename, 'tif');
            
            imageNum = imageNum + 1;

        case MENU_INPUT_TO_QUIT
            disp('Stopping video acquisition ');
            stop(vid);
            delete(vid);
            return;

        otherwise
            error('Invalid entry \n');
    end
end

%$Log: AcquireVideoImages.m,v $
%Revision 1.1  2006/01/30 22:24:51  gouthami
%Adding to CVS
%