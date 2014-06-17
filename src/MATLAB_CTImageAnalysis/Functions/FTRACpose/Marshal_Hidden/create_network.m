function [cost,capacity] = create_network(c_tab, n_seeds1, n_seeds2)

cost = ones(n_seeds1 + n_seeds2 + 2)*1000000;
capacity = zeros(n_seeds1 + n_seeds2 + 2);

size(c_tab)
cost(2:1 + n_seeds1, 2 + n_seeds1:1 + n_seeds1 + n_seeds2) = c_tab;
capacity(2:1 + n_seeds1, 2 + n_seeds1:1 + n_seeds1 + n_seeds2) = ones(n_seeds1, n_seeds2);

[n_row,n_col] = size(cost)
cost(1, 2:1 + n_seeds1) = 0;
cost(n_row - n_seeds2:n_row - 1, n_col) = 0;

capacity(1, 2:1 + n_seeds1) = 2;
capacity(n_row - n_seeds2:n_row - 1, n_col) = 2;

% capacity(1, 2:1 + n_seeds1) = 1;
% capacity(n_row - n_seeds2:n_row - 1, n_col) = 1;

cost = abs(cost);
