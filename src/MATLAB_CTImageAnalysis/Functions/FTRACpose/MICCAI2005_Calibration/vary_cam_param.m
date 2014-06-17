function [res_ra,res_bpe,res_re,res_ren] = vary_cam_param

% profile on;
warning('off');
f_dir = 'Dewarped\Dataset_5\';

rand('state',sum(100*clock));

pixel_ratio = [3.234, 3.083];
screen_size = [1280 960];
origin = [647.1188 483.2224];
% focal_len = 6310/6;
focal_len = 1050;

% origin_err = [0 0].*[3.243 3.083].*rand(1,2).*sign(rand(1,2)-0.5)
% focal_err = 200.*rand(1,1).*sign(rand(1,1)-0.5)

% case: focal length
[res_ra,res_bpe,res_re,res_ren,averages,minmax] = compute_error(f_dir, pixel_ratio, screen_size, origin, focal_len);
averages
% save(strcat('Results\Origin_Err-','0','.txt'), 'averages', 'minmax','-ascii');
% 
% % all other cases
% for i = 1:25
%     averages = zeros(4);
%     minmax = [inf(4,1),-inf(4,1)];
%     
%     for j = 1:50
%     	[res_ra,res_bpe,res_re,res_ren,averages_new,minmax_new] = compute_error(f_dir, pixel_ratio, screen_size, add_error(origin,i), focal_len);    
%         minmax(1,1) = min(minmax(1,1), minmax_new(1,1));
%         minmax(2,1) = min(minmax(2,1), minmax_new(2,1));
%         minmax(3,1) = min(minmax(3,1), minmax_new(3,1));
%         minmax(4,1) = min(minmax(4,1), minmax_new(4,1));
%         minmax(1,2) = max(minmax(1,2), minmax_new(1,2));
%         minmax(2,2) = max(minmax(2,2), minmax_new(2,2));
%         minmax(3,2) = max(minmax(3,2), minmax_new(3,2));
%         minmax(4,2) = max(minmax(4,2), minmax_new(4,2));
%         
%         averages = averages + averages_new;
%     end
%     averages = averages / 50;
%     save(strcat('Results\Origin_Err-',int2str(i),'.txt'), 'averages', 'minmax','-ascii');
% end
% 
% % profile report;