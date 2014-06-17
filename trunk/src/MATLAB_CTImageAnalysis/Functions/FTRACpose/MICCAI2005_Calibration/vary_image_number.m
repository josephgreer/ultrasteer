function [res_ra,res_bpe,res_re,res_ren] = vary_image_number

profile on;
warning('off');
f_dir = 'Dewarped\Dataset_5\';

rand('state',sum(100*clock));

pixel_ratio = [3.234, 3.083];
screen_size = [1280 960];
origin = [647.1188 483.2224];
% focal_len = 6310/6;
focal_len = 1050;

num_iterations = 100;

% origin_err = [0 0].*[3.243 3.083].*rand(1,2).*sign(rand(1,2)-0.5)
% focal_err = 200.*rand(1,1).*sign(rand(1,1)-0.5)

% averages = zeros(4,4,26);
% minmax = zeros(4,2,26);

% case: focal length
[res_ra,res_bpe,res_re,res_ren,averages,minmax] = compute_error_varying_images(f_dir, pixel_ratio, screen_size, origin, focal_len);
averages
% minmax
save(strcat('Results\Total_Err-0-Varying.mat'), 'averages', 'minmax');

% other cases
for i = 100:100
    averages = zeros(4,4,5);
    minmax = repmat([inf(4,1),-inf(4,1)],[1 1 5]);
    
    for j = 1:num_iterations
        [new_values] = add_error_3d([origin, focal_len],i)
    	[res_ra,res_bpe,res_re,res_ren,averages_new,minmax_new] = compute_error_varying_images(f_dir, pixel_ratio, screen_size, [new_values(1), new_values(2)], new_values(3));    
        minmax(1,1) = min(minmax(1,1), minmax_new(1,1));
        minmax(2,1) = min(minmax(2,1), minmax_new(2,1));
        minmax(3,1) = min(minmax(3,1), minmax_new(3,1));
        minmax(4,1) = min(minmax(4,1), minmax_new(4,1));
        minmax(1,2) = max(minmax(1,2), minmax_new(1,2));
        minmax(2,2) = max(minmax(2,2), minmax_new(2,2));
        minmax(3,2) = max(minmax(3,2), minmax_new(3,2));
        minmax(4,2) = max(minmax(4,2), minmax_new(4,2));
        
        averages = averages + averages_new;
    end
    averages = averages / num_iterations;
    averages
    size(averages)
    save(strcat('Results\Total_Err-',int2str(i),'.mat'), 'averages', 'minmax');
end

profile report;