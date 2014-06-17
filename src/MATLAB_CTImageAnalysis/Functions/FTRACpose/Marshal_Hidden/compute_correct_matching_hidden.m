function [match, M1, M2] = compute_correct_matching_hidden(total_seeds, n_seeds1, n_seeds2, hidden_seeds1, hidden_seeds2)

match = zeros(total_seeds, total_seeds);

for i = 1:total_seeds
    match(i,i) = 1;
end 

hidden_seeds1
hidden_seeds2

M1 = [1:total_seeds;1:total_seeds]';

for i = 1:size(hidden_seeds1, 1)
    M1(hidden_seeds1(i, 1), 2) = hidden_seeds1(i, 2);
end

M2 = [1:total_seeds;1:total_seeds]';

for i = 1:size(hidden_seeds2, 1)
    M2(hidden_seeds2(i, 1), 2) = hidden_seeds2(i, 2);
end

% for i = 1:size(hidden_seeds1, 1)
%     M_col1 = [M(1:hidden_seeds1(i, 1) - 1, 1); M(hidden_seeds1(i, 1) + 1:hidden_seeds1(i, 2), 1); M(hidden_seeds1(i, 2), 1); M(hidden_seeds1(i, 2) + 1:size(M, 1), 1)];
%     M_col2 = [M(1:hidden_seeds1(i, 1) - 1, 2); M(hidden_seeds1(i, 1) + 1:hidden_seeds1(i, 2), 2); hidden_seeds1(i, 1); M(hidden_seeds1(i, 2) + 1:size(M, 1), 2)];
%     M = [M_col1 M_col2];
%     M;
% end

% M
% M = [1:hidden_seeds1(1,2)]'
% M = [M; 
% % % M= [];
% 
% for i = 1:size(hidden_seeds1, 1)
%    
% end
% 
for i = 1:size(hidden_seeds1, 1)
    match = [match(1:hidden_seeds1(i, 1) - i, :); match(hidden_seeds1(i, 1) - i + 2:size(match, 1), :)];
    match(hidden_seeds1(i, 2) - i, hidden_seeds1(i, 1)) = 1;
end