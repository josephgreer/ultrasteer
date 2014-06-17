function [pts_3d bpe_all pts_2d_bp matched_sorted matched_ind] = marshal(pts_2d_all, cam_params, trs, total_seeds)
% function [correct_count incorrect_count incorrect_hidden not_found total_hidden res_re res_ren] = match_seeds_hidden(ns_im, err)
% function [correct_count incorrect_count incorrect_hidden not_found
% total_hidden res_re res_ren] = match_seeds_hidden(ns_im, total_seeds, f_dir, err)
% [res_ra, res_bpe, res_re, res_ren]
% formated as [total; matched; unmatched]

warning off
% profile on;
% 
% total_seeds = 40;
% f_dir = 'Dewarped\Dataset_1\';
% total_seeds = 55;
% f_dir = 'Dewarped\Dataset_2\';
% total_seeds = 70;
% f_dir = 'Dewarped\Dataset_3\';
% total_seeds = 85;
% f_dir = 'Dewarped\Dataset_4\';
% total_seeds = 100;
% f_dir = 'Dewarped\Dataset_5\';
% [pts_2d_all,trs_all,cams_all,pts_3d_truth] = read_data(f_dir);

% total_seeds = 112;
% f_dir = 'Simulation\2_0__55_0__4\';
% 
% [pts_2d_all,trs_all,cams_all,pts_3d_truth] = read_simulation(f_dir, 0);

% max_distance = err;
% ns_im = [1 2 3];


pts_2d = pts_2d_all;
N = size(pts_2d, 1);
% FORMAT HACK
for i = 1:N
    pts_2d{i} = pts_2d{i}(:,1:2);
end

pts_2d_orig = pts_2d;

% trs = trs_all(ns_im);
% cams = cams_all(ns_im);

% reformating camera parameters
for i = 1:N
%     cam_params{i} = struct('ratio', [cams{i,1}(1) cams{i,1}(2)], 'origin', [cams{i,1}(3) cams{i,1}(4)], 'focal', cams{i,1}(5));
      ex(i,1) = cam_params{i}.ratio(1);
      ey(i,1) = cam_params{i}.ratio(2);
      xo(i,1) = cam_params{i}.origin(1);
      yo(i,1) = cam_params{i}.origin(2);
      focal(i,1) = cam_params{i}.focal;
end

% % create hidden seeds
% hidden_seeds = cell(N,1);
% % for i = 1:N
% %     [pts_2d{i}, hidden_seeds{i}] = generate_hidden_fixed(pts_2d{i}, max_distance);
% % end
% for i = 1:N
%     [pts_2d{i}, hidden_seeds{i}] = generate_hidden_percentage(pts_2d{i}, err);
% end

% HACK HACK HACK - FIX THIS
% find the number of hidden seeds
hidden_seeds = {};
for i = 1:N
    hidden_temp = total_seeds - size(pts_2d{i},1);
    if (hidden_temp > 0)
        hidden_seeds{i} = zeros(hidden_temp,2);
    else
        hidden_seeds{i} = [];
    end
end


% Reorder the images so that the fewest seeds are on the outside
% find the order by the number of hidden seeds
hidden_sizes = [];
for i = 1:N
    hidden_sizes = [hidden_sizes; size(hidden_seeds{i}, 1)];
end
hidden_order = sortrows([[1:N]' hidden_sizes], 2);

% create reindexing matrix
reordered_ind = [];
for i = 1:N
    reordered_ind = [reordered_ind hidden_order(i,1)];
end

% create reorder point cell arrays
pts_2d_passed = cell(N,1);
pts_2d_passed_orig = cell(N,1);
for i = 1:N
    pts_2d_passed{i} = pts_2d{reordered_ind(i)};
    pts_2d_passed_orig{i} = pts_2d_orig{reordered_ind(i)};
end

% This is done only when the ground truth is known:
% compute the correct matching given the hidden seeds
% correct_matching = compute_correct_matching(total_seeds, {hidden_seeds{reordered_ind(1)};hidden_seeds{reordered_ind(2)};hidden_seeds{reordered_ind(3)}});

% compute the seeds matching in the images
matched = compute_matchings(pts_2d_passed, pts_2d_passed_orig, trs(reordered_ind), ex(reordered_ind), ey(reordered_ind), xo(reordered_ind), yo(reordered_ind), focal(reordered_ind), total_seeds, {hidden_seeds{reordered_ind(1)};hidden_seeds{reordered_ind(2)};hidden_seeds{reordered_ind(3)}});

% This is done only when the ground truth is known:
% compute matching rate
% [matched mis_matched correct_count incorrect_count incorrect_hidden not_found total_hidden] = compute_matching_rate(matched, total_seeds, {hidden_seeds{reordered_ind(1)};hidden_seeds{reordered_ind(2)};hidden_seeds{reordered_ind(3)}});

% % reorder
% reordered_ind_prev = reordered_ind;
% reordered_ind = [];
% for i = 1:N
%     reordered_ind = [reordered_ind find(reordered_ind_prev == i)];
% end
% 
% reordered_ind
% 
% % reorder points to pass
% pts_2d_matched = cell(N,1);
% for i = 1:N
%     pts_2d_matched{i} = pts_2d_orig{reordered_ind(i)};
% end
% 
% matched
% % reorder matching
% matched_reordered = [];
% for i = 1:N
%     matched_reordered = [matched_reordered matched(:,reordered_ind(i))];
% end
% matched_reordered

% calculate BPE
[bpe_all, pts_3d, pts_2d_bp_unordered] = calculate_bpe_final(pts_2d_passed, matched, trs(reordered_ind), ex(reordered_ind), ey(reordered_ind), xo(reordered_ind), yo(reordered_ind), focal(reordered_ind), N);

% reorder
reordered_ind_prev = reordered_ind;
reordered_ind = [];
for i = 1:N
    reordered_ind = [reordered_ind find(reordered_ind_prev == i)];
end

% % reorder the matching
% matched_ordered = [];
% for i = 1:3
%     matched_ordered(:,i) = matched(:,reordered_ind(i));
% end

% correspondence and index
matched_presort = [reordered_ind_prev(1:3); matched(:,1:3)]';
matched_sorted = sortrows(matched_presort, 1)';
matched_ind = matched_sorted(1,:);

matched_sorted = matched_sorted(2:size(matched_sorted, 1),:);


% reorder the bp for the images
for i = 1:N
    pts_2d_bp{i} = pts_2d_bp_unordered{reordered_ind(i)};
end

% pts_3d
% pts_2d_bp{1}
% pts_2d{1}
% pts_2d_bp{2}
% pts_2d{2}
% pts_2d_bp{3}
% pts_2d{3}

% % reorder backprojected points
% pts_2d_bp = cell(N,1);
% 



% compute the error
% [res_re res_ren res_bpe pts_3d] = compute_error(pts_2d_matched, pts_3d_truth, matched, mis_matched, trs(reordered_ind), ex(reordered_ind), ey(reordered_ind), xo(reordered_ind), yo(reordered_ind), focal(reordered_ind), N, total_seeds);

% percentage = correct_count / (correct_count + incorrect_count);
% save(strcat('Results\matched.txt'), 'matched', '-ASCII');
% save(strcat('Results\mis_matched.txt'), 'mis_matched', '-ASCII');
% save(strcat('Results\percentage.txt'), 'percentage', '-ASCII');
% save(strcat('Results\re.txt'), 'res_re', '-ASCII');
% save(strcat('Results\re_rel.txt'), 'res_ren', '-ASCII');
% save(strcat('Results\bpe.txt'), 'res_bpe', '-ASCII');
% save(strcat('Results\pts_3d.txt'), 'pts_3d', '-ASCII');
% 
% res_re
% res_ren

% profile report