function [matched_seeds, loops, mis_matched_seeds] = break_links(pts_2d, pts_2d_hidden, cost, node_edges, flow, bpe_thresh, trs, ex, ey, xo, yo, focal, matching, hidden_seeds, total_seeds)
                 
% seed numbers
n_seeds = cell(4,1);
for i = 1:3
    n_seeds{i} = size(pts_2d{i}, 1);
end
n_seeds{4} = n_seeds{1};
image_hidden = cell(4,1);
for i = 1:4
    temp = matching(:,i);
    temp = sort(temp);
    prev = 0;
    for j = 1:length(temp)
        if (temp(j) == prev)
            image_hidden{i} = [image_hidden{i} temp(j)];
        end
        prev = temp(j);
    end
end
total_seeds
n_seeds

[matched_seeds loops] = find_loops(matching, cost, pts_2d, pts_2d_hidden, trs, ex, ey, xo, yo, focal, hidden_seeds, total_seeds);

d_left = calculate_bpe(pts_2d,matching(:,1:3),trs,ex,ey,xo,yo,focal);
d_right = calculate_bpe({pts_2d{1};pts_2d{2};pts_2d{3}},[matching(:,4) matching(:,2) matching(:,3)],trs,ex,ey,xo,yo,focal);

ind_left = find(d_left > bpe_thresh);
ind_right = find(d_right > bpe_thresh);

diff_left = setdiff(ind_left, ind_right);
diff_right = setdiff(ind_right, ind_left);

in_both = union(ind_left, ind_right);
% in_both = intersect(ind_left, ind_right);

non_hidden2 = find(ismember(matching(:,2), image_hidden{2}) == 0);
non_hidden3 = find(ismember(matching(:,3), image_hidden{3}) == 0);
non_hidden_both = intersect(non_hidden2, non_hidden3);

in_both = intersect(in_both, non_hidden_both);

non_matched = find(~ismember(matching(:,1:4), matched_seeds));
in_both = intersect(in_both, non_matched);

size(in_both)

d_max = max([d_left d_right], [], 2);
d_max_sum = sum(d_max);
mis_matched_seeds = [matching(in_both, 1:4) d_left(in_both) d_right(in_both)];
if (~isempty(mis_matched_seeds))
    mis_matched_seeds = sortrows(mis_matched_seeds, -5);
end

matching = [matching(:,1:4) d_left d_right];
img_tot = matching;

sum_prev = inf;
matched_size_prev = size(matched_seeds, 1);
img_tot_all = cell(3,1);
matched_all = cell(3,1);
mis_matched = cell(3,1);
in_both_all = cell(3,1);
bpe_sum_all = zeros(1,3);
cij_sums = zeros(1,3);
cost_all = cell(3,1);

while (~isempty(mis_matched_seeds))
    link = mis_matched_seeds(1,:)
        
    if (link(5) < bpe_thresh)
        for i = 2:3
            cost_old = cost{i}(link(i),link(i + 1));
            cost{i}(link(i),link(i + 1)) = 1000;

            [img_tot_all{i}, matched_all{i}, mis_matched_all{i}, in_both_all{i}, bpe_sum_all(i)] = rerun_network(pts_2d, pts_2d_hidden, n_seeds, cost, node_edges, flow, bpe_thresh, trs, ex, ey, xo, yo, focal, matched_seeds, hidden_seeds, total_seeds);
            cij_sums(i) = sum(img_tot_all{i}(:,5)) + sum(img_tot_all{i}(:,6)) + sum(img_tot_all{i}(:,7));
            cost_all{i} = cost{i};
            cost{i}(link(i),link(i + 1)) = cost_old;
        end    
        cij_sums(1) = inf;
        bpe_sum_all(1) = inf;
    elseif (link(6) < bpe_thresh)
        for i = 1:2
            cost_old = cost{i}(link(i),link(i + 1));
            cost{i}(link(i),link(i + 1)) = 1000;

            [img_tot_all{i}, matched_all{i}, mis_matched_all{i}, in_both_all{i}, bpe_sum_all(i)] = rerun_network(pts_2d, pts_2d_hidden, n_seeds, cost, node_edges, flow, bpe_thresh, trs, ex, ey, xo, yo, focal, matched_seeds, hidden_seeds, total_seeds);
            cij_sums(i) = sum(img_tot_all{i}(:,5)) + sum(img_tot_all{i}(:,6)) + sum(img_tot_all{i}(:,7));
            cost_all{i} = cost{i};
            cost{i}(link(i),link(i + 1)) = cost_old;
        end    
        cij_sums(3) = inf;
        bpe_sum_all(3) = inf;
    else
        for i = 1:3
            cost_old = cost{i}(link(i),link(i + 1));
            cost{i}(link(i),link(i + 1)) = 1000;

            [img_tot_all{i}, matched_all{i}, mis_matched_all{i}, in_both_all{i}, bpe_sum_all(i)] = rerun_network(pts_2d, pts_2d_hidden, n_seeds, cost, node_edges, flow, bpe_thresh, trs, ex, ey, xo, yo, focal, matched_seeds, hidden_seeds, total_seeds);
            cij_sums(i) = sum(img_tot_all{i}(:,5)) + sum(img_tot_all{i}(:,6)) + sum(img_tot_all{i}(:,7));
            cost_all{i} = cost{i};
            cost{i}(link(i),link(i + 1)) = cost_old;
        end    
    end
        
    bpe_sum_all
    cij_sums
        
    matched_sizes = [size(matched_all{1}, 1) size(matched_all{2}, 1) size(matched_all{3}, 1)]
    [min_total, min_ind] = min(cij_sums);
    [min_total, min_ind2] = min(bpe_sum_all);
    min_ind
    min_ind2
    
%     if (cij_sums(min_ind) < sum_prev)% & matched_sizes(min_ind) > matched_size_prev)
    if (min_ind == min_ind2)
        sum_prev = cij_sums(min_ind); 
        matched_size_prev = matched_sizes(min_ind);
        img_tot = img_tot_all{min_ind};  
        cost{min_ind} = cost_all{min_ind};
        matched_seeds = matched_all{min_ind};
        mis_matched_seeds = mis_matched_all{min_ind};   
    else
        link
        if (size(mis_matched_seeds, 1) > 1)
            mis_matched_seeds = mis_matched_seeds(2:size(mis_matched_seeds, 1),:)
        else
            mis_matched_seeds = []
        end
    end
end

[matched_seeds, loops, mis_matched_seeds, bpe_thresh] = find_loops(img_tot, cost, pts_2d, pts_2d_hidden, trs, ex, ey, xo, yo, focal, hidden_seeds, total_seeds);
1

% [matched, loops, mis_matched_seeds, bpe_thresh] = find_loops(img_tot, cost, pts_2d, pts_2d_hidden, trs, ex, ey, xo, yo, focal, hidden_seeds, total_seeds);
% 1

% % resolve the loops
% if (~isempty(loops))
%     [matched_sub mis_matched_sub] = resolve_loops(loops, 100, cost, matched(:,1:3), pts_2d, trs, ex, ey, xo, yo, focal);
%     if (~isempty(matched_sub))
%         matched = [matched(:,1:3); matched_sub];
%     end
%     
%     if (~isempty(mis_matched_sub))
%         mis_matched = [mis_matched; mis_matched_sub];
%     end     
% end
% 
% loops_sub = {};
% matched = matched(:,1:3);
% 
% while (~isempty(mis_matched_seeds))
%     if (size(mis_matched_seeds, 1) < 0)
%         mis_matched_seeds
%         error('too small');
%     else
%         [matched_sub loops_sub mis_matched_seeds] = resolve_outer_hidden_seeds(pts_2d, trs, ex, ey, xo, yo, focal, mis_matched_seeds);
%         if (~isempty(matched_sub))
%             matched = [matched; matched_sub];
%         end
%         
%         if (~isempty(loops_sub))
%             [matched_sub, mis_matched_sub] = resolve_loops(loops_sub, 100, cost, matched(:,1:3), pts_2d, trs, ex, ey, xo, yo, focal);
%             if (~isempty(matched_sub))
%                 matched = [matched; matched_sub];
%             end
%             
%             if (~isempty(mis_matched_sub))
%                 mis_matched = [mis_matched; mis_matched_sub];
%             end                
%         end
%     end
% end
% 
% img_tot = matched; 
% [pt_3d, d_left, d_array_left] = sm_recon_general_2({pts_2d{1}(img_tot(:,1),:);pts_2d{2}(img_tot(:,2),:);pts_2d{3}(img_tot(:,3),:)},trs,ex,ey,xo,yo,focal);
% % [pt_3d, d_right, d_array_right] = sm_recon_general_2({pts_2d{1}(img_tot(:,4),:);pts_2d{2}(img_tot(:,2),:);pts_2d{3}(img_tot(:,3),:)},trs,ex,ey,xo,yo,focal);
% ind_left = find(d_left > bpe_thresh);
% % ind_right = find(d_right > bpe_thresh);
% 
% diff_left = setdiff(ind_left, ind_right);
% % diff_right = setdiff(ind_right, ind_left);
% 
% % in_both = intersect(ind_left, ind_right);
% img_tot = [img_tot(:,1:3) d_left];
% matched_seeds = img_tot;
% 
% 1