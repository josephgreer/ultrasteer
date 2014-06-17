function [correct_count incorrect_count incorrect_hidden not_found total_hidden res_re res_ren] = match_seeds_hidden(ns_im, err)
% [res_ra, res_bpe, res_re, res_ren]
% formated as [total; matched; unmatched]

% profile on;
% 
% total_seeds = 40;
% f_dir = 'Dewarped\Dataset_1\';
total_seeds = 55;
f_dir = 'Dewarped\Dataset_2\';
% total_seeds = 70;
% f_dir = 'Dewarped\Dataset_3\';
% total_seeds = 85;
% f_dir = 'Dewarped\Dataset_4\';
% total_seeds = 100;
% f_dir = 'Dewarped\Dataset_5\';
[pts_2d_all,trs_all,cams_all,pts_3d_truth] = read_data(f_dir);

% total_seeds = 112;
% f_dir = 'Simulation\2_0__55_0__5\';
% 
% [pts_2d_all,trs_all,cams_all,pts_3d_truth] = read_simulation(f_dir, 1);

max_distance = 4;
pts_2d = pts_2d_all(ns_im);
pts_2d_orig = pts_2d;

trs = trs_all(ns_im);
cams = cams_all(ns_im);

N = length(ns_im);

% reformating camera parameters
for i = 1:N
  ex(i,1) = cams{i,1}(1);
  ey(i,1) = cams{i,1}(2);
  xo(i,1) = cams{i,1}(3);
  yo(i,1) = cams{i,1}(4);
  focal(i,1) = cams{i,1}(5);
end

% create hidden seeds
hidden_seeds = cell(N,1);

for i = 1:N
    [pts_2d{i}, hidden_seeds{i}] = generate_hidden_fixed(pts_2d{i}, max_distance);
end

% for i = 1:N
%     [pts_2d{i}, hidden_seeds{i}] = generate_hidden_percentage(pts_2d{i}, err);
% end

% reorder to make sure fewest hidden seeds are on the outside
hidden_sizes = [];
for i = 1:N
    hidden_sizes = [hidden_sizes; size(hidden_seeds{i}, 1)];
end

hidden_order = sortrows([[1:N]' hidden_sizes], 2);

reordered_ind = [];
for i = 1:N
    reordered_ind = [reordered_ind hidden_order(i,1)];
end
reordered_ind

pts_2d_passed = cell(N,1);
pts_2d_passed_orig = cell(N,1);
for i = 1:N
    pts_2d_passed{i} = pts_2d{reordered_ind(i)};
    pts_2d_passed_orig{i} = pts_2d_orig{reordered_ind(i)};
end

correct_matching = compute_correct_matching(total_seeds, {hidden_seeds{reordered_ind(1)};hidden_seeds{reordered_ind(2)};hidden_seeds{reordered_ind(3)}});

% compute the seeds matching in the images
matched = compute_matchings(pts_2d_passed, pts_2d_passed_orig, trs(reordered_ind), ex(reordered_ind), ey(reordered_ind), xo(reordered_ind), yo(reordered_ind), focal(reordered_ind), total_seeds, {hidden_seeds{reordered_ind(1)};hidden_seeds{reordered_ind(2)};hidden_seeds{reordered_ind(3)}}, correct_matching);

[matched mis_matched correct_count incorrect_count incorrect_hidden not_found total_hidden] = compute_matching_rate(matched, total_seeds, {hidden_seeds{reordered_ind(1)};hidden_seeds{reordered_ind(2)};hidden_seeds{reordered_ind(3)}});

pts_2d_matched = cell(N,1);
% pts_2d_matched = pts_2d_orig{reordered_ind(1), reordered_ind(2), reordered_ind(3)};
for i = 1:N
    pts_2d_matched{i} = pts_2d_orig{reordered_ind(i)};
end

[res_re res_ren] = compute_error(pts_2d_matched, pts_3d_truth, matched, mis_matched, trs(reordered_ind), ex(reordered_ind), ey(reordered_ind), xo(reordered_ind), yo(reordered_ind), focal(reordered_ind), N, total_seeds);
% compute_statistics(pts_2d_matched, pts_3d_truth, matched, trs(reordered_ind), ex(reordered_ind), ey(reordered_ind), xo(reordered_ind), yo(reordered_ind), focal(reordered_ind), N, total_seeds);
% compute_matchings({pts_2d{reordered_ind(1)};pts_2d{reordered_ind(2)};pts_2d{reordered_ind(3)}}, trs(reordered_ind), ex(reordered_ind), ey(reordered_ind), xo(reordered_ind), yo(reordered_ind), focal(reordered_ind), N, total_seeds);
res_re

% profile report