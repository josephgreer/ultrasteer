function node_edges = create_node_edges(cost, n_seeds, edge_percent)

% node edge capacities
node_edges = cell(3,1);

for i = 1:3
    if (size(cost, 1) > 10)
        % remove edges via histogram
        [N, X] = hist(reshape(cost{i}, size(cost{i}, 1) * size(cost{i}, 2), 1), 100);

        j = 1;
        edge_count = 0;

        % find threshold for specified percentage of edges
        while (edge_count < edge_percent * n_seeds{i} * n_seeds{i + 1})
           edge_count = edge_count + N(j);
           j = j + 1;
        end

        threshold = X(j) - (X(j) - X(j - 1)) / 2
    else
        threshold = inf;
    end
    % remove specified percentage of edges and create capacity
    cost_ind = find(cost{i} > threshold & cost{i} ~= 100000);
    node_edges{i} = ones(n_seeds{i}, n_seeds{i + 1});
    node_edges{i}(cost_ind) = 0;
    node_edges{i} = node_edges{i} .* (ones(n_seeds{i}, n_seeds{i + 1}) - eye(n_seeds{i}, n_seeds{i + 1}));
    node_edges{i} = node_edges{i} + eye(n_seeds{i}, n_seeds{i + 1});
end