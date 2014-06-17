function matched = compute_matchings(pts_2d_hidden, pts_2d, trs, ex, ey, xo, yo, focal, total_seeds,hidden_seeds)
% COMPUTE_MATCHINGS - Begin the seed matching process
%   This fuction calculates and computes the correct matching and returns a
%   Nx3 matching array with the matched seeds.
    
mis_matched = [];
   
% calculate the number of seeds in each image
n_seeds = cell(4,1);
for i = 1:3
    n_seeds{i} = size(pts_2d{i},1);
end
n_seeds{4} = n_seeds{1};

n_seeds

% We need to create individual components of the network
% Lambda is a weighting function used for alternative cost calculations and
% should be left at 1
lambda = 1;
% create the cost matrices used
% if (length(pts_2d) == 3)
cost = generate_cost_matrices(pts_2d_hidden, trs, ex, ey, xo, yo, focal, lambda);

% this is ugly - FIX THIS
for i = 4:length(pts_2d)
    pts_2d{i} = pts_2d_hidden{i};
end
    
% elseif (length(pts_2d) == 4)
%     cost = generate_cost_matrices_4imgs(pts_2d_hidden, trs, ex, ey, xo, yo, focal);
%     pts_2d{4} = pts_2d_hidden{4};
% else
%     cost = generate_cost_matrices_5imgs(pts_2d_hidden, trs, ex, ey, xo, yo, focal);
%     pts_2d{4} = pts_2d_hidden{4};
%     pts_2d{5} = pts_2d_hidden{5};
% end

% This portion of code is used for hidden seed matrix alterations used only
% in testing when the hidden seeds are known.  It allows for easier error
% computation and should not be used for real data.
% for i = 1:3
%     for j = 1:size(hidden_seeds{i}, 1)
%         cost{i} = [cost{i}(1:hidden_seeds{i}(j,1) - 1, :); 1000*ones(1,size(cost{i},2)); cost{i}(hidden_seeds{i}(j,1):size(cost{i}, 1),:)];
%     end
%     
%     for j = 1:size(hidden_seeds{mod(i,3) + 1}, 1)
%         cost{i} = [cost{i}(:, 1:hidden_seeds{mod(i,3) + 1}(j,1) - 1) 1000*ones(size(cost{i},1),1) cost{i}(:,hidden_seeds{mod(i,3) + 1}(j,1):size(cost{i}, 2))];
%     end
% end

% correct_cost = current_cost(cost, correct_matching)

% The edge_perc variable defines the percentage of total nodes are
% allowable in the network based up cost histograms
edge_perc = 0.1;
% create the appropriate node edges
node_edges = create_node_edges(cost, n_seeds, edge_perc);
[flow, node_edges] = create_initial_flow(cost, n_seeds, node_edges, total_seeds, hidden_seeds);

% create the network and perform the seed matching using cycle canceling
[path_costs, capacity] = create_total_network(n_seeds, cost, node_edges, hidden_seeds);
[flow, n_c] = match_cycle(flow, capacity, path_costs, 1);

% obtain matching from resulting flow
[match seed_nums] = extract_flow_matching(flow, n_seeds);
img_tot = process_matching_flow(match, n_seeds);

% We create zero cost links in order to force the network to choose what we
% have decided to be "good" matchings
link_costs = zeros(size(img_tot,1),3);
for i = 1:size(img_tot, 1)
    link_costs(i,1) = cost{1}(img_tot(i,1),img_tot(i,2));
    link_costs(i,2) = cost{2}(img_tot(i,2),img_tot(i,3));
    link_costs(i,3) = cost{3}(img_tot(i,3),img_tot(i,4));
end

img_tot = [img_tot link_costs];

[matched, loops, mis_matched_seeds, bpe_thresh] = find_loops(img_tot, cost, pts_2d, pts_2d_hidden, trs, ex, ey, xo, yo, focal, hidden_seeds, total_seeds);
% [matched_seeds, loops, mis_matched_seeds] = break_links(pts_2d, pts_2d_hidden, cost, node_edges, flow, bpe_thresh, trs, ex, ey, xo, yo, focal, img_tot, hidden_seeds, total_seeds)
[matched_temp, matched, bad_loops, bpe_sum] = process_image_matching(matched, loops, mis_matched_seeds, 10000, cost, pts_2d, trs, ex, ey, xo, yo, focal);