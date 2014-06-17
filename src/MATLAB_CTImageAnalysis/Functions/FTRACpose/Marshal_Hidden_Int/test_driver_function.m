total_seeds = 40;
f_dir = 'Dewarped\Dataset_1\';
% total_seeds = 55;
% f_dir = 'Dewarped\Dataset_2\';
% total_seeds = 70;
% f_dir = 'Dewarped\Dataset_3\';
% total_seeds = 85;
% f_dir = 'Dewarped\Dataset_4\';
% total_seeds = 100;
% f_dir = 'Dewarped\Dataset_5\';
[pts_2d_all,trs_all,cams_all,pts_3d_truth] = read_data(f_dir);

% total_seeds = 112;
% f_dir = 'Simulation\2_0__55_0__4\';
% 
% [pts_2d_all,trs_all,cams_all,pts_3d_truth] = read_simulation(f_dir, 0);

% max_distance = err;
ns_im = [1 2 3];
err = 0;


pts_2d = pts_2d_all(ns_im);
pts_2d_orig = pts_2d;

trs = trs_all(ns_im);
cams = cams_all(ns_im);

N = length(ns_im);

% reformating camera parameters
for i = 1:N
  cam_params{i} = struct('ratio', [cams{i,1}(1) cams{i,1}(2)], 'origin', [cams{i,1}(3) cams{i,1}(4)], 'focal', cams{i,1}(5));
%   ex(i,1) = cams{i,1}(1);
%   ey(i,1) = cams{i,1}(2);
%   xo(i,1) = cams{i,1}(3);
%   yo(i,1) = cams{i,1}(4);
%   focal(i,1) = cams{i,1}(5);
end

% create hidden seeds
hidden_seeds = cell(N,1);
for i = 1:N
    [pts_2d{i}, hidden_seeds{i}] = generate_hidden_fixed(pts_2d{i}, -1);
end
% for i = 1:N
%     [pts_2d{i}, hidden_seeds{i}] = generate_hidden_percentage(pts_2d{i}, 2);
% end

% Reorder the images so that the fewest seeds are on the outside
% find the order by the number of hidden seeds
hidden_sizes = [];
for i = 1:N
    hidden_sizes = [hidden_sizes; size(hidden_seeds{i}, 1)];
end
hidden_order = sortrows([[1:N]' hidden_sizes], 2);



[pts_3d, bpe_all, pts_2d_bp] = marshal(pts_2d, cam_params, trs, total_seeds)
