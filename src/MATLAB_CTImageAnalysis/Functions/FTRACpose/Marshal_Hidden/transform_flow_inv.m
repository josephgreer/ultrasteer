function flow = transform_flow_inv(flow, n_seeds1, n_seeds2)

for i = 1:n_seeds1
    flow(i+1, 1) = -flow(1, i+1);
    flow(n_seeds1 + n_seeds2 + 2, n_seeds1 + 1 + i) = -flow(n_seeds1 + 1 + i, n_seeds1 + n_seeds2 + 2);
end

% for i = 1:n_seeds2
%     flow(n_seeds1 + n_seeds2 + 2, n_seeds1 + 1 + i) = -flow(n_seeds1 + 1 + i, n_seeds1 + n_seeds2 + 2);
% end