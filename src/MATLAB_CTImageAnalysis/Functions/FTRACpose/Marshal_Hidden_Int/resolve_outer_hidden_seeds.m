function [matched_sub loops_sub mis_matched_seeds_sub] = resolve_outer_hidden_seeds(pts_2d, trs, ex, ey, xo, yo, focal, mis_matched_seeds)
% RESOLVE_OUTER_HIDDEN_SEEDS - resolves cases where there is an outer
% hidden seed mismatch
%   This fuction creates the network flow formulation when there are
%   mismatched outer hidden seeds.  The other seeds are put on the inside
%   to force consistency in these cases, which the images with the fewest
%   number of hidden seeds on the outside.  If there are no image subsets
%   with zero hidden seeds, then the hidden seeds are replicated on the
%   outside to ensure that there is no outer hidden seed mismatch.  


n_seeds_mis = size(mis_matched_seeds, 1)
% seed_order = sortrows([[1:3]' [size(unique(mis_matched_seeds(:,1)),1); size(unique(mis_matched_seeds(:,2)),1); size(unique(mis_matched_seeds(:,3)),1)]], -2);
seed_order = sortrows([[2:3]' [size(unique(mis_matched_seeds(:,2)),1); size(unique(mis_matched_seeds(:,3)),1)]], -2);


% seed_order = sortrows([[1:3]' [n_seeds{1}; n_seeds{2}; n_seeds{3}]], -2);
reordered_ind = [seed_order(1,1); 1; seed_order(2,1)];

pts_2d_new = cell(3,1);
pts_2d_new{1} = pts_2d{reordered_ind(1)}(mis_matched_seeds(:,reordered_ind(1)),:);
pts_2d_new{2} = pts_2d{reordered_ind(2)}(unique(mis_matched_seeds(:,reordered_ind(2))),:);
pts_2d_new{3} = pts_2d{reordered_ind(3)}(mis_matched_seeds(:,reordered_ind(3)),:);

% pts_2d_new{1} = pts_2d{1}(mis_matched_seeds(:,reordered_ind(1)),:);
% pts_2d_new{2} = pts_2d{2}(unique(mis_matched_seeds(:,reordered_ind(2))),:);
% pts_2d_new{3} = pts_2d{3}(mis_matched_seeds(:,reordered_ind(3)),:);

n_seeds_new = cell(4,1);
n_seeds_new{1} = size(pts_2d_new{1},1);
n_seeds_new{2} = size(pts_2d_new{2},1);
n_seeds_new{3} = size(pts_2d_new{3},1);
n_seeds_new{4} = n_seeds_new{1};

total_seeds = n_seeds_new{1};

hidden_seeds = cell(3,1);

lambda = 1;

% create individual components of the network
cost = generate_cost_matrices({pts_2d_new{1};pts_2d_new{2};pts_2d_new{3}}, trs(reordered_ind), ex(reordered_ind), ey(reordered_ind), xo(reordered_ind), yo(reordered_ind), focal(reordered_ind), lambda);
node_edges = create_node_edges(cost, n_seeds_new, 0.5);
[flow, node_edges] = create_initial_flow(cost, n_seeds_new, node_edges, total_seeds, hidden_seeds);

% create the network and perform the seed matching using cycle canceling
[path_costs, capacity] = create_total_network(n_seeds_new, cost, node_edges, hidden_seeds);
[flow, n_c] = match_cycle(flow, capacity, path_costs, 1);

% obtain matching from resulting flow
[match seed_nums] = extract_flow_matching(flow, n_seeds_new);
img_tot = process_matching_flow(match, n_seeds_new);

link_costs = zeros(size(img_tot,1),3);
for i = 1:size(img_tot, 1)
    link_costs(i,1) = cost{1}(img_tot(i,1),img_tot(i,2));
    link_costs(i,2) = cost{2}(img_tot(i,2),img_tot(i,3));
    link_costs(i,3) = cost{3}(img_tot(i,3),img_tot(i,4));
end

temp = mis_matched_seeds(:,reordered_ind(1));
img_tot(:,1) = temp(img_tot(:,1)); 
img_tot(:,4) = temp(img_tot(:,4));

temp = unique(mis_matched_seeds(:,reordered_ind(2)));
img_tot(:,2) = temp(img_tot(:,2));

temp = mis_matched_seeds(:,reordered_ind(3));
img_tot(:,3) = temp(img_tot(:,3)); 

img_tot = [img_tot link_costs];

% {pts_2d{reordered_ind(1)};pts_2d{reordered_ind(2)};pts_2d{reordered_ind(3)}}, trs(reordered_ind), ex(reordered_ind), ey(reordered_ind), xo(reordered_ind), yo(reordered_ind), focal(reordered_ind);
[matched_sub, loops_sub, mis_matched_seeds_sub] = find_loops_outer(img_tot, pts_2d, trs, ex, ey, xo, yo, focal);

% reorder
reordered_ind = [find(reordered_ind == 1); find(reordered_ind == 2); find(reordered_ind == 3)];

if (~isempty(matched_sub))
    matched_sub = [matched_sub(:,reordered_ind(1))  matched_sub(:,reordered_ind(2))  matched_sub(:,reordered_ind(3))];
end

if (~isempty(mis_matched_seeds_sub))
    mis_matched_seeds_sub = [mis_matched_seeds_sub(:,reordered_ind(1))  mis_matched_seeds_sub(:,reordered_ind(2)) mis_matched_seeds_sub(:,reordered_ind(3))];
end

for i = 1:size(loops_sub, 2)
    loops_sub{i} = [loops_sub{i}(:,reordered_ind(1)) loops_sub{i}(:,reordered_ind(2)) loops_sub{i}(:,reordered_ind(3))];
end
