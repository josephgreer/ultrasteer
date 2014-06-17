function [cost,capacity] = create_multi_img_network(c_tab, n_seeds1, n_seeds2, n_seeds3)

threshold = 30;

% initialize the network
cost = ones(2 * n_seeds1 + 2 * n_seeds2 + 2 * n_seeds3 + 2)*1000000;
capacity = zeros(2 * n_seeds1 + 2 * n_seeds2 + 2 * n_seeds3 + 2);

% find high costs
cost_ind1 = find(c_tab{1} > threshold);
cost_ind2 = find(c_tab{2} > threshold);
cost_ind3 = find(c_tab{3} > threshold);

% cost matrices
cost(2:1 + n_seeds1, 2 + n_seeds1:1 + n_seeds1 + n_seeds2) = c_tab{1};
cost(2 + n_seeds1 + n_seeds2:1 + n_seeds1 + 2 * n_seeds2, 2 + n_seeds1 + 2 * n_seeds2:1 + n_seeds1 + 2 * n_seeds2 + n_seeds3) = c_tab{2};
cost(2 + n_seeds1 + 2 * n_seeds2 + n_seeds3:1 + n_seeds1 + 2 * n_seeds2 + 2 * n_seeds3, 2 + n_seeds1 + 2 * n_seeds2 + 2 * n_seeds3: 1 + n_seeds1 + 2 * n_seeds2 + 2 * n_seeds3 + n_seeds1) = c_tab{3};

% links between image nodes
% image 1 - 2
cap1 = ones(n_seeds1, n_seeds2);
cap1(cost_ind1) = 0;
capacity(2:1 + n_seeds1, 2 + n_seeds1:1 + n_seeds1 + n_seeds2) = cap1;

% image 2 - 3
cap2 = ones(n_seeds2, n_seeds3);
cap2(cost_ind2) = 0;
capacity(2 + n_seeds1 + n_seeds2:1 + n_seeds1 + 2 * n_seeds2, 2 + n_seeds1 + 2 * n_seeds2:1 + n_seeds1 + 2 * n_seeds2 + n_seeds3) = cap2;

% image 3 - 1
cap3 = ones(n_seeds3, n_seeds1);
cap3(cost_ind3) = 0;
capacity(2 + n_seeds1 + 2 * n_seeds2 + n_seeds3:1 + n_seeds1 + 2 * n_seeds2 + 2 * n_seeds3, 2 + n_seeds1 + 2 * n_seeds2 + 2 * n_seeds3: 1 + n_seeds1 + 2 * n_seeds2 + 2 * n_seeds3 + n_seeds1) = cap3;

% zero cost links
[n_row,n_col] = size(cost)
cost(1, 2:1 + n_seeds1) = 0; % source
cost(n_row - n_seeds1:n_row - 1, n_col) = 0; % sink
cost(2 + n_seeds1: 1 + n_seeds1 + n_seeds2, 2 + n_seeds1 + n_seeds2:1 + n_seeds1 + 2 * n_seeds2) = zeros(n_seeds2);
cost(2 + n_seeds1 + 2 * n_seeds2: 1 + n_seeds1 + 2 * n_seeds2 + n_seeds3, 2 + n_seeds1 + 2 * n_seeds2 + n_seeds3:1 + n_seeds1 + 2 * n_seeds2 + 2 * n_seeds3) = zeros(n_seeds3);

% source and sink nodes
capacity(1, 2:1 + n_seeds1) = 2;
capacity(n_row - n_seeds1:n_row - 1, n_col) = 2;

% intermmediary nodes
capacity(2 + n_seeds1: 1 + n_seeds1 + n_seeds2, 2 + n_seeds1 + n_seeds2:1 + n_seeds1 + 2 * n_seeds2) = 2 * eye(n_seeds2);
capacity(2 + n_seeds1 + 2 * n_seeds2: 1 + n_seeds1 + 2 * n_seeds2 + n_seeds3, 2 + n_seeds1 + 2 * n_seeds2 + n_seeds3:1 + n_seeds1 + 2 * n_seeds2 + 2 * n_seeds3) = 2 * eye(n_seeds3);

cost = abs(cost);
