function [matched, total_matching, mis_matched_seeds, bad_loops, bpe_sum] = run_network(pts_2d, pts_2d_hidden, n_seeds, cost, node_edges, flow, bpe_thresh, trs, ex, ey, xo, yo, focal, matched_seeds, mis_matched_seeds, hidden_seeds, total_seeds)

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

[matched, loops, mis_matched_seeds, max_d] = find_loops(img_tot, cost, pts_2d, pts_2d_hidden, trs, ex, ey, xo, yo, focal, hidden_seeds, total_seeds);

[matched, total_matching, bad_loops, bpe_sum] = process_image_matching(matched, loops, mis_matched_seeds, bpe_thresh, cost, pts_2d, trs, ex, ey, xo, yo, focal);