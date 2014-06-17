function f = create_feasible_flow(f, n_seeds1, n_seeds2, hidden_seeds)  

for i = 1:n_seeds1
    % source to nodes
    f(1, i + 1) = 1;
%         f(i + 1, 1) = -1;

    % nodes to nodes
    f(1 + n_seeds1 + i, i + 1) = -1;
    f(i + 1, 1 + n_seeds1 + i) = 1;

    % nodes to sink
    f(1 + n_seeds1 + i, n_seeds1 + n_seeds2 + 2) = 1;
%         f(n_seeds1 + n_seeds2 + 2, 1 + n_seeds1 + i) = -1;
end

for i = 1:hidden_seeds
   % source to nodes
   f(1, i + 1) = f(1, i + 1) + 1;

   % nodes to nodes
   f(i + 1, n_seeds1 + n_seeds2 + 2 - i) = 1;
   f(n_seeds1 + n_seeds2 + 2 - i, i + 1) = -1;

   % nodes to sink
   f(n_seeds1 + n_seeds2 + 2 - i, n_seeds1 + n_seeds2 + 2) = f(n_seeds1 + n_seeds2 + 2 - i, n_seeds1 + n_seeds2 + 2) + 1;
end