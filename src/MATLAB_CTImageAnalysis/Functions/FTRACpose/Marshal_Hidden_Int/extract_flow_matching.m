function [match, seed_nums] = extract_flow_matching(flow, n_seeds)

[n_row,n_col] = size(flow);

match = cell(3,1);
match{1} = flow(2:1 + n_seeds{1}, 2 + n_seeds{1}:1 + n_seeds{1} + n_seeds{2});
match{2} = flow(2 + n_seeds{1} + n_seeds{2}:1 + n_seeds{1} + 2 * n_seeds{2}, 2 + n_seeds{1} + 2 * n_seeds{2}:1 + n_seeds{1} + 2 * n_seeds{2} + n_seeds{3});
match{3} = flow(2 + n_seeds{1} + 2 * n_seeds{2} + n_seeds{3}:1 + n_seeds{1} + 2 * n_seeds{2} + 2 * n_seeds{3}, 2 + n_seeds{1} + 2 * n_seeds{2} + 2 * n_seeds{3}: 1 + n_seeds{1} + 2 * n_seeds{2} + 2 * n_seeds{3} + n_seeds{1});

seed_nums = cell(4,1);
seed_nums{1} = flow(1, 2:1 + n_seeds{1})';
seed_nums{2} = flow(2 + n_seeds{1}: 1 + n_seeds{1} + n_seeds{2}, 2 + n_seeds{1} + n_seeds{2}:1 + n_seeds{1} + 2 * n_seeds{2}) * ones(n_seeds{2}, 1);
seed_nums{3} = flow(2 + n_seeds{1} + 2 * n_seeds{2}: 1 + n_seeds{1} + 2 * n_seeds{2} + n_seeds{3}, 2 + n_seeds{1} + 2 * n_seeds{2} + n_seeds{3}:1 + n_seeds{1} + 2 * n_seeds{2} + 2 * n_seeds{3}) * ones(n_seeds{3}, 1);
seed_nums{4} = flow(n_row - n_seeds{1}:n_row - 1, n_col);
