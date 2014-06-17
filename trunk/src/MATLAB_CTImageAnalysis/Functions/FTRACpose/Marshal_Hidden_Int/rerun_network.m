function [img_tot, matched, mis_matched_seeds, in_both, d_max_sum] = rerun_network(pts_2d, pts_2d_hidden, n_seeds, cost, node_edges, flow, bpe_thresh, trs, ex, ey, xo, yo, focal, matched_seeds, hidden_seeds, total_seeds)

% create zero cost links for matched seeds
for i = 1:size(matched_seeds, 1)
    cost{1}(matched_seeds(i,1), matched_seeds(i,2)) = 0;
    cost{2}(matched_seeds(i,2), matched_seeds(i,3)) = 0;
    cost{3}(matched_seeds(i,3), matched_seeds(i,1)) = 0;
end

% create and solve the network flow
[path_costs, capacity] = create_total_network(n_seeds, cost, node_edges, hidden_seeds);
[flow, n_c] = match_cycle(flow, capacity, path_costs, 1);

[match seed_nums] = extract_flow_matching(flow, n_seeds);
img_tot = process_matching_flow(match, n_seeds);

% link costs of the matching
link_costs = zeros(size(img_tot,1),3);
for i = 1:size(img_tot, 1)
    link_costs(i,1) = cost{1}(img_tot(i,1),img_tot(i,2));
    link_costs(i,2) = cost{2}(img_tot(i,2),img_tot(i,3));
    link_costs(i,3) = cost{3}(img_tot(i,3),img_tot(i,4));
end

img_tot = [img_tot link_costs];

image_hidden = cell(4,1);
for i = 1:4
    temp = img_tot(:,i);
    temp = sort(temp);
    prev = 0;
    for j = 1:length(temp)
        if (temp(j) == prev)
            image_hidden{i} = [image_hidden{i} temp(j)];
        end
        prev = temp(j);
    end
end

[matched, loops, mis_matched_seeds] = find_loops(img_tot, cost, pts_2d, pts_2d_hidden, trs, ex, ey, xo, yo, focal, hidden_seeds, total_seeds);

d_left = calculate_bpe(pts_2d,img_tot(:,1:3),trs,ex,ey,xo,yo,focal);
d_right = calculate_bpe({pts_2d{1};pts_2d{2};pts_2d{3}},[img_tot(:,4) img_tot(:,2) img_tot(:,3)],trs,ex,ey,xo,yo,focal);ind_left = find(d_left > bpe_thresh);
ind_right = find(d_right > bpe_thresh);

diff_left = setdiff(ind_left, ind_right);
diff_right = setdiff(ind_right, ind_left);

in_both = union(ind_left, ind_right);
% in_both = intersect(ind_left, ind_right);

non_hidden2 = find(ismember(img_tot(:,2), image_hidden{2}) == 0);
non_hidden3 = find(ismember(img_tot(:,3), image_hidden{3}) == 0);
non_hidden_both = intersect(non_hidden2, non_hidden3);

in_both = intersect(in_both, non_hidden_both);

non_matched = find(~ismember(img_tot(:,1:4), matched_seeds));
in_both = intersect(in_both, non_matched);

img_tot = [img_tot d_left d_right];

% [matched_seeds, total_matching, bad_loops, bpe_sum] = process_image_matching(matched_seeds, loops, mis_matched_seeds, bpe_thresh, cost, pts_2d, trs, ex, ey, xo, yo, focal);


d_max = max([d_left d_right], [], 2);
d_max_sum = sum(d_max);

mis_matched_seeds = [img_tot(in_both, 1:4) d_left(in_both) d_right(in_both)];
if (~isempty(mis_matched_seeds))
    mis_matched_seeds = sortrows(mis_matched_seeds, -5);
end
