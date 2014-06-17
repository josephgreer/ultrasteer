function [pix_match, goodness, goodness2] = locate_match(eline, needle_image)
% function that accepts a needle image and the endpoints of an epipolar
% line and returns the corresponding pixel and the goodness of
% correspondance.
% Author: Tom Wedlick

[v, h] = size (needle_image);

eline_image = zeros(size(needle_image));

if ~(isempty(eline))
    
    % Sample along the epipolar line
    t = 0:.01:1;
    y = eline(2,1) + t *(eline(2,2) - eline(2,1));
    x = eline(1,1) + t *(eline(1,2) - eline(1,1));
    
    % Round the epipolar line to be whole pixels
    x_r = round (x);
    y_r = round (y);
    
    
    % Save the pixels that constitue the epipolar line
    eline_image(x_r + v * (y_r-1)) = 1;
    
    % Spread out the epipolar line
    hSize = 2;
    kern = fspecial('gaussian',[2,1]);
    eline_image = conv2(eline_image, kern, 'same');
    eline_image = conv2(eline_image, kern','same');
    eline_image = double (eline_image > 0);
    
    % Apply the eppipolar line to only keep the part of the needle on the
    % e-line
    needle_segment = eline_image.*needle_image;
    
    % Use the construced mask (modImage) to determine the matching pixel by
    % only considering the segment of the needle on the spread eline
    
    maxVal = max(max(needle_segment));
    
    if maxVal == 0
        ind = find(needle_segment);
        if isempty(ind)
            maxVal = 0;
        else
            maxVal = max(max(needle_segment(ind)));
        end
    end
    [i,j] = find(needle_segment == maxVal);
%    [i,j] = find( needle_segment > 0.05);
%     [m,n] = find(needle_segment <0.6);
%     [c] = intersect([k l],[m n],'rows');
%     i = c(:,1);
%     j = c(:,2);
    
    if ~(isempty(i))
        pix_match = round([sum(j); sum(i)]/ length(i));
        goodness = length(i);
    else
        pix_match = [-100;-100];
        goodness = -1;
    end
     
    % Segment out a small region of the image containing the e-line
    section = [(min([eline(2,2),eline(2,1)])),(max([eline(2,2),eline(2,1)])) ; (min([eline(1,2),eline(1,1)])),(max([eline(1,2),eline(1,1)]))];

    dist = max(abs([section(1,1)-section(1,2),section(2,1)-section(2,2)]));
    mid = [section(1,1)+section(1,2);section(2,1)+section(2,2)]/2;
    offset = 10;
    
    section = [(mid(1) - dist/2 - offset),(mid(1) + dist/2 + offset);(mid(2) - dist/2 - offset),(mid(2) + dist/2 + offset)];
    section = round(section);
    
    %ANN ADDED 3/25/11 FOR DEBUGGING PURPOSES
     
    if section(1,1) < 1
       section(1,1) = 1;
    end
    if section(1,2) < 1 
       section(1,2) = 1;
    end
    if section(2,1) < 1
       section(2,1) = 1;
    end
    if section(2,2) < 1
       section(2,2) = 1;
    end
    
    
    if section(1,1) > v
       section(1,1) = v;
    end
    if section(1,2) > v
       section(1,2) = v;
    end
    if section(2,1) > h
       section(2,1) = h;
    end
    if section(2,2) > h
       section(2,2) = h;
    end
    

    segment_needle = needle_image(section(2,1):section(2,2),section(1,1):section(1,2));

 %   imshow(segment_needle)
 %   imshow(segment_eline) 
    %imshow(segment_needle-segment_eline,[])
    
    %Perform a Hough transform on the segment to determine the princilple
    %needle directions and the princilple eline directions
    [H,T,R] = hough(segment_needle>0);
     P  = houghpeaks(H,1);
    angle_needle = T(P(:,2));
    
    
    %Determine the angle of the epipolar line for comparison
    angle_eline = -atan2(eline(1,1) - eline(1,2),eline(2,1) - eline(2,2)) * 180/pi;
    if (angle_eline > 90)
        angle_eline = angle_eline - 180;
    elseif (angle_eline < -90)
        angle_eline = angle_eline + 180;
    end
    
%      segment_eline = eline_image(section(1,1):section(1,2),section(2,1):section(2,2));
%      [He,Te,Re] = hough(segment_eline>0);
%      Pe  = houghpeaks(He,1); %note that I can directly calulate this!
%      angle_eline = Te(Pe(:,2));

     difference = angle_needle - angle_eline;

    goodness2 = min(abs([difference,difference-180,difference+180])); %The second goodness mesure is an indicator of the angle between the needle and the eline
   
    
else
    pix_match = [-100;-100];
    goodness = -1;
end








% function [pix_match, goodness, goodness2] = locate_match(eline, needle_image)
% % function that accepts a needle image and the endpoints of an epipolar
% % line and returns the corresponding pixel and the goodness of
% % correspondance.
% % Author: Tom Wedlick
% 
% [v, h] = size (needle_image);
% 
% eline_image = zeros(size(needle_image));
% 
% if ~(isempty(eline))
%     
%     % Sample along the epipolar line
%     t = -1:.01:1;
% %     if eline(1,1) < 0 && eline(1,2) < 0                         % ANN ADDED - 3/25/11 AM
% %           y = -1*(eline(1,1) + t *(eline(1,2) - eline(1,1)));
% %     else
% %         if eline(1,1) < 0 && eline(1,2) > 0
% %           y = (eline(1,1)) + t *(eline(1,2) - eline(1,1));
% %           y = y+-1*y(1)+1;
% %         else
% %             if eline(1,1) > 0 && eline(1,2) < 0
% %                 y = eline(1,1) + t *(eline(1,2) - eline(1,1));
% %                 y = y+-1*(y(end))+1;
% %             else
%                 
%                 y = eline(1,1) + t *(eline(1,2) - eline(1,1));
% %              end
% %          end
% %      end
%     x = eline(2,1) + t *(eline(2,2) - eline(2,1));
%     
%     % Round the epipolar line to be whole pixels
%     x_r = round (x);
%     y_r = round (y);
%     
%   %  if y_r(end) <0
%   %      y_r = y_r+-1*(y_r(end))+1;
%   %  end
%     
%    
%     
%    indx_max = find(x_r <= v); 
%    indx_min = find(x_r > 0); 
%    indy_max = find(y_r <= h);
%    indy_min = find(y_r > 0); 
%    
%    idx = intersect(indx_max,indx_min);
%    idy = intersect(indy_max,indy_min);
%    
%    indexOK = intersect(idx,idy);
%    x_r = x_r(indexOK);
%    y_r = y_r(indexOK);
%     
%     
%     
% %     indx_min = find(x_r >= 1);
% %     indx = intersect(indx_max,indx_min);
% %     indx = unique(indx);
% %     
% %     x_r = x_r(indx); y_r = y_r(indx);
% %     
% %      
% %     indy_max = find(y_r <= v);
% %     indy_min = find(y_r >= 1);
% %     indy = intersect(indy_max,indy_min);
% %     indy = unique(indy);
% %     x_r = x_r(indy); y_r = y_r(indy);
% %     
%     
%     
%     % Save the pixels that constitue the epipolar line
%     eline_idx = x_r + v * y_r-1;
%     imax = find(eline_idx <= v*h);
%     imin = find(eline_idx >= 0);
%     idx = intersect(imax,imin); 
% %    
% %    idx = intersect(indx_max,indx_min);
%     
%     eline_image(eline_idx(idx)) = 1;       % PROBLEM WITH THIS BEING NEGATIVE IN SOME CASES - 3/25/11 AM
%     
%     % Spread out the epipolar line
%     hSize = 2;
%     kern = fspecial('gaussian',[2,1]);
%     eline_image = conv2(eline_image, kern, 'same');
%     eline_image = conv2(eline_image, kern','same');
%     eline_image = double (eline_image > 0);
%     
%     % Apply the eppipolar line to only keep the part of the needle on the
%     % e-line
%     needle_segment = eline_image.*needle_image;
%     
%     % Use the construced mask (modImage) to determine the matching pixel by
%     % only considering the segment of the needle on the spread eline
%     [i,j] = find( needle_segment > 0);
%     
%     if ~(isempty(i))
%         pix_match = [sum(j); sum(i)]/ length(i);
%         goodness = length(i);
%     else
%         pix_match = [-100;-100];
%         goodness = -1;
%     end
%      
%     % Segment out a small region of the image containing the e-line
%     section = [(min([eline(2,2),eline(2,1)])),(max([eline(2,2),eline(2,1)])) ; (min([eline(1,2),eline(1,1)])),(max([eline(1,2),eline(1,1)]))];
% 
%     dist = max(abs([section(1,1)-section(1,2),section(2,1)-section(2,2)]));
%     mid = [section(1,1)+section(1,2);section(2,1)+section(2,2)]/2;
%     offset = 10; 
%     
%     section = [(mid(1) - dist/2 - offset),(mid(1) + dist/2 + offset);(mid(2) - dist/2 - offset),(mid(2) + dist/2 + offset)];
%     section = round(section);
% 
%     
%     %ANN ADDED 3/25/11 FOR DEBUGGING PURPOSES
%      
%     if section(1,1) < 1
%        section(1,1) = 1;
%     end
%     if section(1,2) < 1 
%        section(1,2) = 1;
%     end
%     if section(2,1) < 1
%        section(2,1) = 1;
%     end
%     if section(2,2) < 1
%        section(2,2) = 1;
%     end
%     
%     
%     if section(1,1) > v
%        section(1,1) = v;
%     end
%     if section(1,2) > v
%        section(1,2) = v;
%     end
%     if section(2,1) > h
%        section(2,1) = h;
%     end
%     if section(2,2) > h
%        section(2,2) = h;
%     end
%     
%      segment_needle = needle_image(section(1,1):section(1,2),section(2,1):section(2,2));
% % 
% %    imshow(segment_needle)
%     %imshow(segment_eline,[]) 
% %    figure(800)
% %    hold off
% %    imshow(needle_segment+needle_image,[])
%     
%     %Perform a Hough transform on the segment to determine the princilple
%     %needle directions and the princilple eline directions
%     [H,T,R] = hough(segment_needle>0);
%      P  = houghpeaks(H,1);
%     angle_needle = T(P(:,2));
%     
%     
%     %Determine the angle of the epipolar line for comparison
%     angle_eline = -atan2(eline(1,1) - eline(1,2),eline(2,1) - eline(2,2)) * 180/pi;
%     if (angle_eline > 90)
%         angle_eline = angle_eline - 180;
%     elseif (angle_eline < -90)
%         angle_eline = angle_eline + 180;
%     end
%     
% %      segment_eline = eline_image(section(1,1):section(1,2),section(2,1):section(2,2));
% %      [He,Te,Re] = hough(segment_eline>0);
% %      Pe  = houghpeaks(He,1); %note that I can directly calulate this!
% %      angle_eline = Te(Pe(:,2));
% 
%      difference = angle_needle - angle_eline;
% 
%     goodness2 = min(abs([difference,difference-180,difference+180])); %The second goodness mesure is an indicator of the angle between the needle and the eline
%    
%     
% else
%     pix_match = [-100;-100];
%     goodness = -1;
% end