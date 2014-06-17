function img_tot = match_remaining(pts_2d, pts_2d_hidden, trs, ex, ey, xo, yo, focal, matched_seeds, hidden_seeds, total_seeds)
                 
% seed numbers
n_seeds = cell(4,1);
for i = 1:3
    n_seeds{i} = size(pts_2d{i}, 1);
end
n_seeds{4} = n_seeds{1};

% hidden_seeds = cell(3,1);
total_seeds
n_seeds

[fewest_seeds, fewest_index] = min([n_seeds{1} n_seeds{2} n_seeds{3}])

% create individual components of the network
if (length(pts_2d) == 3)
    cost = generate_cost_matrices(pts_2d_hidden, trs, ex, ey, xo, yo, focal);
else
    cost = generate_cost_matrices_4imgs(pts_2d_hidden, trs, ex, ey, xo, yo, focal);
end

for i = 1:3
    for j = 1:size(hidden_seeds{i}, 1)
        cost{i} = [cost{i}(1:hidden_seeds{i}(j,1) - 1, :); 1000*ones(1,size(cost{i},2)); cost{i}(hidden_seeds{i}(j,1):size(cost{i}, 1),:)];
    end
    
    for j = 1:size(hidden_seeds{mod(i,3) + 1}, 1)
        cost{i} = [cost{i}(:, 1:hidden_seeds{mod(i,3) + 1}(j,1) - 1) 1000*ones(size(cost{i},1),1) cost{i}(:,hidden_seeds{mod(i,3) + 1}(j,1):size(cost{i}, 2))];
    end
end

for i = 1:size(matched_seeds{1}, 1)
    cost{1}(matched_seeds{1}(i), matched_seeds{2}(i)) = 0;
    cost{2}(matched_seeds{2}(i), matched_seeds{3}(i)) = 0;
    cost{3}(matched_seeds{3}(i), matched_seeds{1}(i)) = 0;
end

node_edges = create_node_edges(cost, n_seeds, 0.1);
[flow, node_edges] = create_initial_flow(cost, n_seeds, node_edges ,total_seeds, hidden_seeds);

% create the network and perform the seed matching using cycle canceling
[path_costs, capacity] = create_total_network(n_seeds, cost, node_edges, hidden_seeds);
[flow, n_c] = match_cycle(flow, capacity, path_costs, 1);

[match seed_nums] = extract_flow_matching(flow, n_seeds);
img_tot = process_matching_flow(match, n_seeds);

link_costs = zeros(size(img_tot,1),3);
for i = 1:size(img_tot, 1)
    link_costs(i,1) = cost{1}(img_tot(i,1),img_tot(i,2));
    link_costs(i,2) = cost{2}(img_tot(i,2),img_tot(i,3));
    link_costs(i,3) = cost{3}(img_tot(i,3),img_tot(i,4));
end

img_tot = [img_tot link_costs];