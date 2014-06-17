function correct_matching = compute_correct_matching(total_seeds, hidden_seeds)

M = cell(3,1);
M{1} = [1:total_seeds;1:total_seeds]';
M{2} = [1:total_seeds;1:total_seeds]';
M{3} = [1:total_seeds;1:total_seeds]';

for i = 1:size(hidden_seeds{1}, 1)
    M{1}(hidden_seeds{1}(i, 1), 2) = hidden_seeds{1}(i, 2);
end

for i = 1:size(hidden_seeds{2}, 1)
    M{2}(hidden_seeds{2}(i, 1), 2) = hidden_seeds{2}(i, 2);
end

for i = 1:size(hidden_seeds{3}, 1)
    M{3}(hidden_seeds{3}(i, 1), 2) = hidden_seeds{3}(i, 2);
end

correct_matching = [M{1}(:,2) M{2}(:,2) M{3}(:,2) M{1}(:,2)];