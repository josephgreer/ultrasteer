% $Id: test_orientation.m,v 1.1 2006/02/02 16:48:37 jain Exp $

%Check for errors in orientation. If flag is 1, plot all the errors in a
%figure. dir is the directory to look for and the other variables are the
%carm parameters. num_images is the numer of images in the directory.
function err = test_orientation(dir, flag, pixel_ratio, focal_len, origin, num_images)

% Read the 3D data.
all_seed_rots = load( strcat(dir, '/3DSeedRotations.txt'), '-mat' );
all_seed_rots = all_seed_rots.seed_rot_wrt_grnd;
all_seed_poss = load( strcat(dir, '/3DSeedPositions.txt'), '-ascii' );

%For each image calculate the origin projection and then that of the -ve
%z-axis. Use that to compute the image direction and compare that to the
%one already present in the file for that image. Dump all this information
%in a file and display if flag is 1.
err = [];
for count = 1 : num_images
    diff = [];
    xform_implant_2_carm = load(strcat(dir, '/image_transform_',int2str(count),'.txt'), '-ascii');
    im_seed_loc = load(strcat(dir, '/image_seeds_',int2str(count),'.txt'), '-ascii');
    im_seed_loc = im_seed_loc(:,1:3);
    for i = 1 : size(all_seed_rots,3)
        T = xform_implant_2_carm*[squeeze(all_seed_rots(:,:,i)) all_seed_poss(i,:)' ; 0 0 0 1];
        P1 = T*[0 0 0 1]';
        P2 = T*[0 0 -1 1]';
        p1 = create_image_pixels(pixel_ratio, 2*origin, focal_len, origin, P1');
        p2 = create_image_pixels(pixel_ratio, 2*origin, focal_len, origin, P2');
        p1 = p1(1:2);
        p2 = p2(1:2);
        center_diff = sqrt(sum((p1 - im_seed_loc(i,1:2)).^2));
        d = (p2-p1)/sqrt(sum((p2-p1).^2));
        angle_diff = acos(dot(d, [1 0]))*sign(d(2))*180/pi - im_seed_loc(i,3);
        diff = [diff; center_diff angle_diff]; 
    end
    if(flag==1)
        figure 
        plot(diff(:,1), 'r')    
        hold on
        plot(diff(:,2), 'g')
    end
    err = [err; mean(abs(diff))];
    if( sum(mean(abs(diff)) > [0.1 0.1]) > 0 )
        display 'Orientation/center is somehow inconsistant.'
        mean(abs(diff))
    end
end

% $Log: test_orientation.m,v $
% Revision 1.1  2006/02/02 16:48:37  jain
% Checked for orientation errors. Added a function to check for orientation errors.
% Also the Gaussian averaging of the image added boundary to the image, making
% the segmentation information incorrect - have changed this. Added another input
% for the number of equal spaced projections required.
%
