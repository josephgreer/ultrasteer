% function [match match1 percentage_hidden] = process_matching_flow(flow, n_seeds1, n_seeds2, hidden_seeds1, hidden_seeds2, M1, M2)
function img_tot = process_matching_flow(match, n_seeds)

seeds = cell(3,1);
for i = 2:3

    temp1 = [];
    for j = 1:n_seeds{i}
        temp1 = [temp1; size(find(match{i - 1}(:, j)), 1)];
    end

    temp2 = [];
    for j = 1:n_seeds{i}
        temp2 = [temp2; size(find(match{i}(j, :)), 1)];
    end
    seeds{i} = [temp1 temp2];
end

temp1 = [];
for j = 1:n_seeds{1}
    temp1 = [temp1; size(find(match{1}(j, :)), 1)];
end

temp2 = [];
for j = 1:n_seeds{1}
    temp2 = [temp2; size(find(match{3}(:, j)), 1)];
end
seeds{1} = [temp1 temp2];

for i = 1:size(match, 1)
    if (size(match{i}, 1) == 1)
        match{i} = [match{i}; zeros(1, size(match{i}, 2))];
    end
end


[img1R img2L] = find(match{1} == 1);
[img2R img3L] = find(match{2} == 1);
[img3R img1L] = find(match{3} == 1);

img1 = [img1L img1R];
img2 = [img2L img2R];
img3 = [img3L img3R];

match1_sorted = sortrows([img1R img2L], 2);
match2_sorted = sortrows([img2R img3L], 1);

img_tot = [match1_sorted match2_sorted(:,2)];

img_tot = sortrows(img_tot, 3);
match3_sorted = sortrows([img3R img1L], 1);

img_tot = [img_tot match3_sorted(:,2)];
img_tot = sortrows(img_tot, 1);