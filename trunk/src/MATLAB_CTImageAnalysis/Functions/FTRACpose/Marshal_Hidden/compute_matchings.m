function matched = compute_matchings(pts_2d_hidden, pts_2d, trs, ex, ey, xo, yo, focal, total_seeds,hidden_seeds,correct_matching)
    
percentage_hidden = [];
percentage = [];
fout = [];
   
% seed numbers
n_seeds = cell(4,1);
for i = 1:3
    n_seeds{i} = length(pts_2d{i});
end
n_seeds{4} = n_seeds{1};

n_seeds

[fewest_seeds, fewest_index] = min([n_seeds{1} n_seeds{2} n_seeds{3}]);

lambda = 1;
% create individual components of the network
if (length(pts_2d) == 3)
    cost = generate_cost_matrices(pts_2d_hidden, trs, ex, ey, xo, yo, focal, lambda);
else
    cost = generate_cost_matrices_4imgs(pts_2d_hidden, trs, ex, ey, xo, yo, focal);
    pts_2d{4} = pts_2d_hidden{4};
end

for i = 1:3
    for j = 1:size(hidden_seeds{i}, 1)
        cost{i} = [cost{i}(1:hidden_seeds{i}(j,1) - 1, :); 1000*ones(1,size(cost{i},2)); cost{i}(hidden_seeds{i}(j,1):size(cost{i}, 1),:)];
    end
    
    for j = 1:size(hidden_seeds{mod(i,3) + 1}, 1)
        cost{i} = [cost{i}(:, 1:hidden_seeds{mod(i,3) + 1}(j,1) - 1) 1000*ones(size(cost{i},1),1) cost{i}(:,hidden_seeds{mod(i,3) + 1}(j,1):size(cost{i}, 2))];
    end
end

correct_cost = current_cost(cost, correct_matching)

node_edges = create_node_edges(cost, n_seeds, 0.1);
[flow, node_edges] = create_initial_flow(cost, n_seeds, node_edges, total_seeds, hidden_seeds);

% EDIT COST MATRIX
% % HIDDEN SEED COSTS
% for i = 1:size(hidden_seeds{1}, 1)
%     cost{1}(hidden_seeds{1}(i,1),:) = inf;
%     cost{3}(:,hidden_seeds{1}(i,1)) = inf;
% end
% 
% for i = 1:size(hidden_seeds{2}, 1)
%     cost{1}(:,hidden_seeds{2}(i,1)) = inf;
%     cost{2}(hidden_seeds{2}(i,1),:) = inf;
% end
% 
% for i = 1:size(hidden_seeds{3}, 1)
%     cost{2}(:,hidden_seeds{3}(i,1)) = inf;
%     cost{3}(hidden_seeds{3}(i,1),:) = inf;
% end

% create the network and perform the seed matching using cycle canceling
[path_costs, capacity] = create_total_network(n_seeds, cost, node_edges, hidden_seeds);
[flow, n_c] = match_cycle(flow, capacity, path_costs, 1);

% obtain matching from resulting flow
[match seed_nums] = extract_flow_matching(flow, n_seeds);
img_tot = process_matching_flow(match, n_seeds);

link_costs = zeros(size(img_tot,1),3);
for i = 1:size(img_tot, 1)
    link_costs(i,1) = cost{1}(img_tot(i,1),img_tot(i,2));
    link_costs(i,2) = cost{2}(img_tot(i,2),img_tot(i,3));
    link_costs(i,3) = cost{3}(img_tot(i,3),img_tot(i,4));
end

img_tot = [img_tot link_costs];
[matched, loops, mis_matched_seeds] = find_loops(img_tot, cost, pts_2d, pts_2d_hidden, trs, ex, ey, xo, yo, focal, hidden_seeds);


loops_sub = {};

% matched = [matched; [mis_matched_seeds -2*ones(size(mis_matched_seeds, 1), 1)]];

[pt_3d,d]=sm_recon_core2({pts_2d{1}(4,:);pts_2d{2}(4,:);pts_2d{3}(4,:)},trs,ex,ey,xo,yo,focal)

while (~isempty(mis_matched_seeds))
    if (size(mis_matched_seeds, 1) < 5)
        mis_matched_seeds
        error('too small');
    else
        [matched_sub loops_sub mis_matched_seeds] = resolve_outer_hidden_seeds(pts_2d, trs, ex, ey, xo, yo, focal, mis_matched_seeds);
        if (~isempty(matched_sub))
            matched = [matched; [matched_sub -2*ones(size(matched_sub, 1), 1)]];
        end
        loop_cost = current_cost(cost, loops_sub{1})
        temp_cost = current_cost(cost, [4 4 4; 18 18 18; 4 9 9])

        if (~isempty(loops_sub))
            matched_sub = resolve_loops(loops_sub, cost, matched(:,1:3), pts_2d, trs, ex, ey, xo, yo, focal);
            if (~isempty(matched_sub))
                matched = [matched; [matched_sub -3*ones(size(matched_sub, 1), 1)]];
            end
        end
    end
end

loops_all = [];
for i = 1:size(loops, 2)
    loops_all = [loops_all; loops{i}];
end

% test_cost = current_cost(cost, matched(:,1:3))
temp_cost = current_cost(cost, [matched(:,1:3); loops_all])

if (~isempty(loops))
    matched_sub = resolve_loops(loops, cost, matched(:,1:3), pts_2d, trs, ex, ey, xo, yo, focal);
    if (~isempty(matched_sub))
        matched = [matched; [matched_sub -4*ones(size(matched_sub, 1), 1)]];
    end
end

matched = sortrows(matched, 3);
matched = sortrows(matched, 2);
matched = sortrows(matched, 1);

correct_cost
matched_cost = current_cost(cost, matched)

% if (matched_cost > 1000)
%     pause
% end
