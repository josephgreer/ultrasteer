function [cost, capacity, flow] = create_network_equal(c_tab, n_seeds, total_seeds)
% Equal number of seeds without consideration for hidden seeds


edge_percentage = 0.1;

% initialize variables
cost = ones(2 * n_seeds + 2)*100000;
capacity = zeros(2 * n_seeds + 2);
flow = zeros(2 * n_seeds + 2);

% cost
size(c_tab)
cost(2:1 + n_seeds, 2 + n_seeds:1 + 2 * n_seeds) = c_tab;

% zero cost links
[n_row,n_col] = size(cost)
cost(1, 2:1 + n_seeds) = 0;
cost(n_row - n_seeds:n_row - 1, n_col) = 0;

% remove edges via histogram
[N, X] = hist(reshape(c_tab, size(c_tab, 1) * size(c_tab, 2), 1), 100);

i = 1;
edge_count = 0;

% find threshold for specified percentage of edges
while (edge_count < edge_percentage * n_seeds ^ 2)
   edge_count = edge_count + N(i);
   i = i + 1;
end

threshold = X(i) - (X(i) - X(i - 1)) / 2

% remove specified percentage of edges and create capacity
cost_ind = find(c_tab > threshold);
cap = ones(n_seeds);
cap(cost_ind) = 0;
cap = cap .* (ones(n_seeds) - eye(n_seeds));
cap = cap + eye(n_seeds);
capacity(2:1 + n_seeds, 2 + n_seeds:1 + 2 * n_seeds) = cap;
    
% source and sink capacities
capacity(1, 2:1 + n_seeds) = 1;
capacity(n_row - n_seeds:n_row - 1, n_col) = 1;

% feasible flow
flow(2:1 + n_seeds, 2 + n_seeds:1 + 2 * n_seeds) = eye(n_seeds);
flow(2 + n_seeds:1 + 2 * n_seeds, 2:1 + n_seeds) = -eye(n_seeds);

flow(1, 2:1 + n_seeds) = 1;
flow(2:1 + n_seeds, 1) = -1;

flow(n_row - n_seeds:n_row - 1, n_col) = 1;
flow(n_col, n_row - n_seeds:n_row - 1) = -1;

cost = abs(cost);