function [flow, node_edges] = create_initial_flow(cost, n_seeds, node_edges, total_seeds, hidden_seeds)
% CREATE_INITIAL_FLOW - creates an initial legal flow
%   This function creates a legal initial flow using the hungarian
%   algorithm to find a good first matching.  When there are hidden seeds,
%   the hungarian is run on the all the images using an equal number of
%   seeds, and hence some seeds are not matched and arbitrarily assigned.  
%   The function returns the inital flow as flow, and an update
%   nodge_edges.

flow = zeros(2 * n_seeds{1} + 2 * n_seeds{2} + 2 * n_seeds{3} + 2);

cmb = [1 2; 2 3; 3 1];

% pad cost matrices and perform hungarian matching
init_guess = cell(3,1);
for i = 1:3
    temp_cost = ones(total_seeds) * 100000;
    temp_cost(1:n_seeds{cmb(i,1)}, 1:n_seeds{cmb(i,2)}) = cost{i};
    temp_cost(n_seeds{cmb(i,1)} + 1:total_seeds, n_seeds{cmb(i,2)} + 1:total_seeds) = zeros(total_seeds - n_seeds{cmb(i,1)}, total_seeds - n_seeds{cmb(i,2)});

    [init_guess{i} T] = hungarian(temp_cost);
end

% create starting flows between images
temp_flow = cell(3,1);
for i = 1:3
    temp_flow{i} = zeros(total_seeds);
    for j = 1:total_seeds
        temp_flow{i}(j, init_guess{i}(j)) = 1;
    end
end

% create feasible flows between images
% image 1
assigned_nodes = [];
for j = n_seeds{1} + 1:total_seeds
    % remove flow from dummy nodes
    k = 1;
    temp_flow{1}(j, init_guess{1}(j)) = 0;
    
    while (init_guess{1}(k) == init_guess{1}(j) || ismember(k, assigned_nodes) == 1)
        k = k + 1;
    end
    
    temp_flow{1}(k, init_guess{1}(j)) = 1;
    assigned_nodes = [assigned_nodes k];
end

% image 2 and 3
for i = 2:3
    assigned_nodes = [];
    for j = n_seeds{i} + 1:total_seeds
        % remove flow from dummy nodes
        k = 1;
        temp_flow{i}(j, init_guess{i}(j)) = 0;

        x = find(temp_flow{i - 1}(:, j) == 1);
        while (init_guess{i}(k) == init_guess{i}(j) || ismember(k, assigned_nodes) == 1 || temp_flow{i - 1}(x, k) == 1)
            k = k + 1;
        end

        temp_flow{i}(k, init_guess{i}(j)) = 1;
        assigned_nodes = [assigned_nodes k];
        
        % update incoming flow
        x = find(temp_flow{i - 1}(:, j) == 1);
        temp_flow{i - 1}(x, j) = 0;
        temp_flow{i - 1}(x, k) = 1;
    end
end

% image 1 end
assigned_nodes = [];
for j = n_seeds{1} + 1:total_seeds
    % remove flow from dummy nodes and update incoming flow
    x = find(temp_flow{3}(:, j) == 1);
    temp_flow{3}(x, j) = 0;

    k = 1;
    while (temp_flow{3}(x, k) == 1 || ismember(k, assigned_nodes) == 1)
        k = k + 1;
    end
    
    temp_flow{3}(x, k) = 1;
    assigned_nodes = [assigned_nodes k];
end

% calculate the end flows for each image matching
end_flows = cell(3,2);
for i = 1:3
    for j = 1:n_seeds{i}
        end_flows{i,1} = [end_flows{i,1} length(find(temp_flow{i}(j, :)))];
    end
    
    for j = 1:n_seeds{i + 1}
        end_flows{i,2} = [end_flows{i,2} length(find(temp_flow{i}(:, j)))];
    end
end

% keep the useful part of the flows
image_flows = cell(3,1);
for i = 1:3
    image_flows{i} = temp_flow{i}(1:n_seeds{i}, 1:n_seeds{i + 1});
end

% make sure there is a link where there is an initial flow
for i = 1:3
    x = find(image_flows{i} == 1);
    node_edges{i}(x) = 1;
end

% create flow
[n_row,n_col] = size(flow);

% flow between nodes
flow(2:1 + n_seeds{1}, 2 + n_seeds{1}:1 + n_seeds{1} + n_seeds{2}) = image_flows{1};
flow(2 + n_seeds{1}:1 + n_seeds{1} + n_seeds{2}, 2:1 + n_seeds{1}) = -image_flows{1}';

flow(2 + n_seeds{1} + n_seeds{2}:1 + n_seeds{1} + 2 * n_seeds{2}, 2 + n_seeds{1} + 2 * n_seeds{2}:1 + n_seeds{1} + 2 * n_seeds{2} + n_seeds{3}) = image_flows{2};
flow(2 + n_seeds{1} + 2 * n_seeds{2}:1 + n_seeds{1} + 2 * n_seeds{2} + n_seeds{3}, 2 + n_seeds{1} + n_seeds{2}:1 + n_seeds{1} + 2 * n_seeds{2}) = -image_flows{2}';

flow(2 + n_seeds{1} + 2 * n_seeds{2} + n_seeds{3}:1 + n_seeds{1} + 2 * n_seeds{2} + 2 * n_seeds{3}, 2 + n_seeds{1} + 2 * n_seeds{2} + 2 * n_seeds{3}: 1 + n_seeds{1} + 2 * n_seeds{2} + 2 * n_seeds{3} + n_seeds{1}) = image_flows{3};
flow(2 + n_seeds{1} + 2 * n_seeds{2} + 2 * n_seeds{3}: 1 + n_seeds{1} + 2 * n_seeds{2} + 2 * n_seeds{3} + n_seeds{1}, 2 + n_seeds{1} + 2 * n_seeds{2} + n_seeds{3}:1 + n_seeds{1} + 2 * n_seeds{2} + 2 * n_seeds{3}) = -image_flows{3}';

% intermmediary flows
flow(2 + n_seeds{1}: 1 + n_seeds{1} + n_seeds{2}, 2 + n_seeds{1} + n_seeds{2}:1 + n_seeds{1} + 2 * n_seeds{2}) = repmat(end_flows{1,2}, length(end_flows{1,2}), 1) .* eye(length(end_flows{1,2}));

int_flow = repmat(-(end_flows{1,2} - 1), length(end_flows{1,2}), 1) .* eye(length(end_flows{1,2}));

% % HIDDEN_SEEDS IMAGE 2
% for i = 1:size(hidden_seeds{2},1)
%     int_flow(hidden_seeds{2}(i,1), hidden_seeds{2}(i,1)) = int_flow(hidden_seeds{2}(i,1), hidden_seeds{2}(i,1)) - 1;
% end

flow(2 + n_seeds{1} + n_seeds{2}:1 + n_seeds{1} + 2 * n_seeds{2}, 2 + n_seeds{1}: 1 + n_seeds{1} + n_seeds{2}) = int_flow;

flow(2 + n_seeds{1} + 2 * n_seeds{2}: 1 + n_seeds{1} + 2 * n_seeds{2} + n_seeds{3}, 2 + n_seeds{1} + 2 * n_seeds{2} + n_seeds{3}:1 + n_seeds{1} + 2 * n_seeds{2} + 2 * n_seeds{3}) = repmat(end_flows{2,2}, length(end_flows{2,2}), 1) .* eye(length(end_flows{2,2}));

int_flow = repmat(-(end_flows{2,2} - 1), length(end_flows{2,2}), 1) .* eye(length(end_flows{2,2}));

% % HIDDEN_SEEDS IMAGE 3
% for i = 1:size(hidden_seeds{3},1)
%     int_flow(hidden_seeds{3}(i,1), hidden_seeds{3}(i,1)) = int_flow(hidden_seeds{3}(i,1), hidden_seeds{3}(i,1)) - 1;
% end

flow(2 + n_seeds{1} + 2 * n_seeds{2} + n_seeds{3}:1 + n_seeds{1} + 2 * n_seeds{2} + 2 * n_seeds{3}, 2 + n_seeds{1} + 2 * n_seeds{2}: 1 + n_seeds{1} + 2 * n_seeds{2} + n_seeds{3}) = int_flow;

% source and sink flows
flow(1, 2:1 + n_seeds{1}) = end_flows{1,1};
flow(2:1 + n_seeds{1}, 1) = -(end_flows{1,1} - 1)';

% % HIDDEN SEEDS IMAGE 1 SOURCE
% for i = 1:size(hidden_seeds{1},1)
%     flow(hidden_seeds{1}(i,1) + 1, 1) = flow(hidden_seeds{1}(i,1) + 1, 1) - 1;
% end

flow(n_row - n_seeds{1}:n_row - 1, n_col) = end_flows{3,2};
flow(n_col, n_row - n_seeds{1}:n_row - 1) = -(end_flows{3,2} - 1)';

% % HIDDEN SEEDS IMAGE 1 SINK
% for i = 1:size(hidden_seeds{1},1)
%     flow(n_col, n_row - n_seeds{1} + hidden_seeds{1}(i,1) - 1) = flow(n_col, n_row - n_seeds{1} + hidden_seeds{1}(i,1) - 1) - 1;
% end