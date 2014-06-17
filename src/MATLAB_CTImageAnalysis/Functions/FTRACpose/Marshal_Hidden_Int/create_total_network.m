function [cost, capacity] = create_total_network(n_seeds, c_tab, node_edges, hidden_seeds)
% CREATE_TOTAL_NETWORK - combines all the components to create the network
%   We combine all the previous created components to create the total
%   network, represented by the cost and capacity variables.  

cost = ones(2 * n_seeds{1} + 2 * n_seeds{2} + 2 * n_seeds{3} + 2)*inf;
capacity = zeros(2 * n_seeds{1} + 2 * n_seeds{2} + 2 * n_seeds{3} + 2);
% flow = zeros(2 * n_seeds{1} + 2 * n_seeds{2} + 2 * n_seeds{3} + 2);

[n_row,n_col] = size(cost);

% cost matrices
cost(2:1 + n_seeds{1}, 2 + n_seeds{1}:1 + n_seeds{1} + n_seeds{2}) = c_tab{1};
cost(2 + n_seeds{1} + n_seeds{2}:1 + n_seeds{1} + 2 * n_seeds{2}, 2 + n_seeds{1} + 2 * n_seeds{2}:1 + n_seeds{1} + 2 * n_seeds{2} + n_seeds{3}) = c_tab{2};
cost(2 + n_seeds{1} + 2 * n_seeds{2} + n_seeds{3}:1 + n_seeds{1} + 2 * n_seeds{2} + 2 * n_seeds{3}, 2 + n_seeds{1} + 2 * n_seeds{2} + 2 * n_seeds{3}: 1 + n_seeds{1} + 2 * n_seeds{2} + 2 * n_seeds{3} + n_seeds{1}) = c_tab{3};

% zero cost links
cost(1, 2:1 + n_seeds{1}) = 0; % source
cost(n_row - n_seeds{1}:n_row - 1, n_col) = 0; % sink

int_cost = zeros(n_seeds{2});

% % HIDDEN_SEEDS IMAGE 2
% for i = 1:size(hidden_seeds{2},1)
%     int_cost(hidden_seeds{2}(i,1), hidden_seeds{2}(i,1)) = 100000;
% end
cost(2 + n_seeds{1}: 1 + n_seeds{1} + n_seeds{2}, 2 + n_seeds{1} + n_seeds{2}:1 + n_seeds{1} + 2 * n_seeds{2}) = int_cost;

int_cost = zeros(n_seeds{3});

% % HIDDEN_SEEDS IMAGE 3
% for i = 1:size(hidden_seeds{3},1)
%     int_cost(hidden_seeds{3}(i,1), hidden_seeds{3}(i,1)) = 100000;
% end
cost(2 + n_seeds{1} + 2 * n_seeds{2}: 1 + n_seeds{1} + 2 * n_seeds{2} + n_seeds{3}, 2 + n_seeds{1} + 2 * n_seeds{2} + n_seeds{3}:1 + n_seeds{1} + 2 * n_seeds{2} + 2 * n_seeds{3}) = int_cost;

% % % EDIT SOURCE COSTS
% % HIDDEN SEEDS IMAGE 1 SOURCE
% for i = 1:size(hidden_seeds{1},1)
%     cost(1, hidden_seeds{1}(i,1) + 1) = 100000;
% end
% 
% % HIDDEN SEEDS IMAGE 1 SINK
% for i = 1:size(hidden_seeds{1},1)
%     cost(n_row - n_seeds{1} + hidden_seeds{1}(i,1) - 1, n_col) = 100000;
% end

% edges between nodes
capacity(2:1 + n_seeds{1}, 2 + n_seeds{1}:1 + n_seeds{1} + n_seeds{2}) = node_edges{1};
capacity(2 + n_seeds{1} + n_seeds{2}:1 + n_seeds{1} + 2 * n_seeds{2}, 2 + n_seeds{1} + 2 * n_seeds{2}:1 + n_seeds{1} + 2 * n_seeds{2} + n_seeds{3}) = node_edges{2};
capacity(2 + n_seeds{1} + 2 * n_seeds{2} + n_seeds{3}:1 + n_seeds{1} + 2 * n_seeds{2} + 2 * n_seeds{3}, 2 + n_seeds{1} + 2 * n_seeds{2} + 2 * n_seeds{3}: 1 + n_seeds{1} + 2 * n_seeds{2} + 2 * n_seeds{3} + n_seeds{1}) = node_edges{3};

% source and sink nodes
capacity(1, 2:1 + n_seeds{1}) = 2;
capacity(n_row - n_seeds{1}:n_row - 1, n_col) = 2;

% % % EDIT SOURCE CAPACITY
% % HIDDEN SEEDS IMAGE 1 SOURCE
% for i = 1:size(hidden_seeds{1},1)
%     capacity(1, hidden_seeds{1}(i,1) + 1) = 1;
% end
% 
% % HIDDEN SEEDS IMAGE 1 SINK
% for i = 1:size(hidden_seeds{1},1)
%     capacity(n_row - n_seeds{1} + hidden_seeds{1}(i,1) - 1, n_col) = 1;
% end

% intermmediary nodes
int_cap = 2 * eye(n_seeds{2});
% % HIDDEN_SEEDS IMAGE 2
% for i = 1:size(hidden_seeds{2},1)
%     int_cap(hidden_seeds{2}(i,1), hidden_seeds{2}(i,1)) = 1;
% end

capacity(2 + n_seeds{1}: 1 + n_seeds{1} + n_seeds{2}, 2 + n_seeds{1} + n_seeds{2}:1 + n_seeds{1} + 2 * n_seeds{2}) = int_cap;

int_cap = 2 * eye(n_seeds{3});

% % HIDDEN_SEEDS IMAGE 3
% for i = 1:size(hidden_seeds{3},1)
%     int_cap(hidden_seeds{3}(i,1), hidden_seeds{3}(i,1)) = 1;
% end

capacity(2 + n_seeds{1} + 2 * n_seeds{2}: 1 + n_seeds{1} + 2 * n_seeds{2} + n_seeds{3}, 2 + n_seeds{1} + 2 * n_seeds{2} + n_seeds{3}:1 + n_seeds{1} + 2 * n_seeds{2} + 2 * n_seeds{3}) = int_cap;


% % flow between nodes
% flow(2:1 + n_seeds{1}, 2 + n_seeds{1}:1 + n_seeds{1} + n_seeds{2}) = image_flows{1};
% flow(2 + n_seeds{1}:1 + n_seeds{1} + n_seeds{2}, 2:1 + n_seeds{1}) = -image_flows{1}';
% 
% flow(2 + n_seeds{1} + n_seeds{2}:1 + n_seeds{1} + 2 * n_seeds{2}, 2 + n_seeds{1} + 2 * n_seeds{2}:1 + n_seeds{1} + 2 * n_seeds{2} + n_seeds{3}) = image_flows{2};
% flow(2 + n_seeds{1} + 2 * n_seeds{2}:1 + n_seeds{1} + 2 * n_seeds{2} + n_seeds{3}, 2 + n_seeds{1} + n_seeds{2}:1 + n_seeds{1} + 2 * n_seeds{2}) = -image_flows{2}';
% 
% flow(2 + n_seeds{1} + 2 * n_seeds{2} + n_seeds{3}:1 + n_seeds{1} + 2 * n_seeds{2} + 2 * n_seeds{3}, 2 + n_seeds{1} + 2 * n_seeds{2} + 2 * n_seeds{3}: 1 + n_seeds{1} + 2 * n_seeds{2} + 2 * n_seeds{3} + n_seeds{1}) = image_flows{3};
% flow(2 + n_seeds{1} + 2 * n_seeds{2} + 2 * n_seeds{3}: 1 + n_seeds{1} + 2 * n_seeds{2} + 2 * n_seeds{3} + n_seeds{1}, 2 + n_seeds{1} + 2 * n_seeds{2} + n_seeds{3}:1 + n_seeds{1} + 2 * n_seeds{2} + 2 * n_seeds{3}) = -image_flows{3}';
% 
% % intermmediary flows
% flow(2 + n_seeds{1}: 1 + n_seeds{1} + n_seeds{2}, 2 + n_seeds{1} + n_seeds{2}:1 + n_seeds{1} + 2 * n_seeds{2}) = repmat(end_flows{1,2}, length(end_flows{1,2}), 1) .* eye(length(end_flows{1,2}));
% flow(2 + n_seeds{1} + n_seeds{2}:1 + n_seeds{1} + 2 * n_seeds{2}, 2 + n_seeds{1}: 1 + n_seeds{1} + n_seeds{2}) = repmat(-(end_flows{1,2} - 1), length(end_flows{1,2}), 1) .* eye(length(end_flows{1,2}));
% 
% flow(2 + n_seeds{1} + 2 * n_seeds{2}: 1 + n_seeds{1} + 2 * n_seeds{2} + n_seeds{3}, 2 + n_seeds{1} + 2 * n_seeds{2} + n_seeds{3}:1 + n_seeds{1} + 2 * n_seeds{2} + 2 * n_seeds{3}) = repmat(end_flows{2,2}, length(end_flows{2,2}), 1) .* eye(length(end_flows{2,2}));
% flow(2 + n_seeds{1} + 2 * n_seeds{2} + n_seeds{3}:1 + n_seeds{1} + 2 * n_seeds{2} + 2 * n_seeds{3}, 2 + n_seeds{1} + 2 * n_seeds{2}: 1 + n_seeds{1} + 2 * n_seeds{2} + n_seeds{3}) = repmat(-(end_flows{2,2} - 1), length(end_flows{2,2}), 1) .* eye(length(end_flows{2,2}));
% 
% % source and sink flows
% flow(1, 2:1 + n_seeds{1}) = end_flows{1,1};
% flow(2:1 + n_seeds{1}, 1) = -(end_flows{1,1} - 1)';
% 
% flow(n_row - n_seeds{1}:n_row - 1, n_col) = end_flows{3,2};
% flow(n_col, n_row - n_seeds{1}:n_row - 1) = -(end_flows{3,2} - 1)';