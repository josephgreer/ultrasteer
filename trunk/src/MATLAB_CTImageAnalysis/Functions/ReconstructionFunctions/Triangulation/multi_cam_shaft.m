% A simple script to go through every pixel found in one image and look for
% a corresponding pixel in another image. 

clear needle
    % I am not fitting a plane to the tip locations 
    %tip_normal = 0;
    %tip_mean_data = 0;
graphics = isGraphicsOn;


% Start at the base of the needle (arrange the centers)
for n = 1 : cam_num
    [row_vals, row_indicies] = sort(cam(n).img_centers_row,1,'descend');
    cam(n).img_centers_row = row_vals;
    cam(n).img_centers_col = cam(n).img_centers_col(row_indicies);
end


%% Forward Stereo Directions
clear needle 
stereo = stereo_lr3; n = 2; m = 1; %n is source, m is match camera
needle(1) = triangulate_shaft(stereo, [ cam(n).img_centers_col, cam(n).img_centers_row ]', cam(n).name,...
    cam(m).img_needle, cam(m).name, refraction, graphics);

stereo = stereo_lc3; n = 2; m = 3; %n is source, m is match camera
needle(2) = triangulate_shaft(stereo, [ cam(n).img_centers_col, cam(n).img_centers_row ]', cam(n).name,...
    cam(m).img_needle, cam(m).name, refraction, graphics);

stereo = stereo_cr3; n = 3; m = 1; %n is source, m is match camera
needle(3) = triangulate_shaft(stereo, [ cam(n).img_centers_col, cam(n).img_centers_row ]', cam(n).name,...
    cam(m).img_needle, cam(m).name, refraction, graphics);

%% Reverse Stereo Directions
stereo = stereo_lr3; n = 1; m = 2; %n is source, m is match camera
needle(4) = triangulate_shaft(stereo, [ cam(n).img_centers_col, cam(n).img_centers_row ]', cam(n).name,...
    cam(m).img_needle, cam(m).name, refraction, graphics);

stereo = stereo_lc3; n = 3; m = 2; %n is source, m is match camera
needle(5) = triangulate_shaft(stereo, [ cam(n).img_centers_col, cam(n).img_centers_row ]', cam(n).name,...
    cam(m).img_needle, cam(m).name, refraction, graphics);

stereo = stereo_cr3; n = 1; m = 3; %n is source, m is match camera
needle(6) = triangulate_shaft(stereo, [ cam(n).img_centers_col, cam(n).img_centers_row ]', cam(n).name,...
    cam(m).img_needle, cam(m).name, refraction, graphics);

%% Display the needle points and the correspondance metricies
%close all
if (isGraphicsOn)
    symbols = ['r*';'bx';'ms';'kd';'go';'yp'];

    for pair = 1:6
        num = length (needle(pair).correspondance_metric);

        figure(30)
        hold on
        subplot(2,1,1)
        hold on 
        plot(1:num,needle(pair).correspondance_metric(:,1),symbols(pair,:))
        title('# of needle pixels on eline')

        subplot(2,1,2)
        plot(1:num,needle(pair).correspondance_metric(:,2),symbols(pair,:))
        title('\theta difference between needle and eline')
        hold off

        figure (31)
        title('Non-common world frame needle points')
        hold on 
        points = needle(pair).points;
        indicies = find(needle(pair).valid_points == 1);
        plot3(points(1,indicies),points(2,indicies),points(3,indicies),symbols(pair,:))
        %plot3(points(1,:),points(2,:),points(3,:),symbols(pair,:))    

    end
end

%% Save the data
needle_frame(shaft_count).needle = needle;
needle_frame(shaft_count).loop = loop; %so that we know when the frame was determined
%needle_frame(shaft_count).cam = cam; %this may get very big! but it does have all of the data we may want later

% %Save the needle and center points
% for n = 1 : cam_num
%     frame(shaft_count,n).imageNumber = loop;
%     frame(shaft_count,n).centers = [cam(n).img_centers_row, cam(n).img_centers_col];
%     frame(shaft_count,n).needle = [cam(n).img_needle_row, cam(n).img_needle_col];
% end
