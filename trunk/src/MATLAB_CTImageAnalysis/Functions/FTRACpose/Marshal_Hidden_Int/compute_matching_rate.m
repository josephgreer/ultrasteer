function [correct_matched mis_matched correct_count incorrect_count incorrect_hidden not_found total_hidden] = compute_matching_rate(matched, total_seeds, hidden_seeds)

old_matched = matched;
correct_matched = [];
mis_matched = [];

for i = 1:total_seeds
    match(i,i) = 1;
end 

total_hidden = size(hidden_seeds{1}, 1) + size(hidden_seeds{2}, 1) + size(hidden_seeds{3}, 1)

M = cell(3,1);
M{1} = [1:total_seeds;1:total_seeds]';
M{2} = [1:total_seeds;1:total_seeds]';
M{3} = [1:total_seeds;1:total_seeds]';

for i = 1:size(hidden_seeds{1}, 1)
%     temp_col = matched(:,1); 
%     x = find(temp_col >= hidden_seeds{1}(i, 1));
% 	temp_col(x) = temp_col(x) + 1;
%     matched(:,1) = temp_col;
    
%     temp_col = matched(:,4); 
%     x = find(temp_col >= hidden_seeds{1}(i, 1));
% 	temp_col(x) = temp_col(x) + 1;
%     matched(:,4) = temp_col;
    
    M{1}(hidden_seeds{1}(i, 1), 2) = hidden_seeds{1}(i, 2);
end

for i = 1:size(hidden_seeds{2}, 1)
%     temp_col = matched(:,2); 
%     x = find(temp_col >= hidden_seeds{2}(i, 1));
% 	temp_col(x) = temp_col(x) + 1;
%     matched(:,2) = temp_col;
    
    M{2}(hidden_seeds{2}(i, 1), 2) = hidden_seeds{2}(i, 2);
end

for i = 1:size(hidden_seeds{3}, 1)
%     temp_col = matched(:,3); 
%     x = find(temp_col >= hidden_seeds{3}(i, 1));
% 	temp_col(x) = temp_col(x) + 1;
%     matched(:,3) = temp_col;
    
    find(matched(:,3)>= hidden_seeds{3}(i, 1));
    M{3}(hidden_seeds{3}(i, 1), 2) = hidden_seeds{3}(i, 2);
end

correct_matching = [M{1}(:,2) M{2}(:,2) M{3}(:,2) M{1}(:,2)];

% find seeds that are hidden in each image
hidden_seeds_found = cell(3,1);
for i = 1:3
    temp = matched(:,i);
    temp = sort(temp);
    prev = 0;
    for j = 1:length(temp)
        if (temp(j) == prev)
            hidden_seeds_found{i} = [hidden_seeds_found{i} temp(j)];
        end
        prev = temp(j);
    end
end

not_found = 0;
for i = 1:3
    if (~isempty(hidden_seeds_found{i} & ~isempty(hidden_seeds{i})))
        setxor(hidden_seeds_found{i}, hidden_seeds{i}(:,2))
        not_found = not_found + size(setxor(hidden_seeds_found{i}, hidden_seeds{i}(:,2)), 2) / 2;
    end
end

not_found

% row = [3 2 1];
% for i = 1:3
%     correct_matching = sortrows(correct_matching, row(i));
%     matched = sortrows(matched, row(i));
% end

% temp = [matched correct_matching(:,1:3)];

correct_count = 0;
incorrect_count = 0;
network_wrong = 0;
outer_seeds_wrong = 0;
outer_loops_wrong = 0;
normal_loops_wrong = 0;
incorrect_hidden = 0;

for i = 1:size(matched, 1)
    if (ismember(matched(i,1:3), correct_matching(:,1:3),'rows') == 1)
        correct_matched = [correct_matched; old_matched(i,1:3)];
        correct_count = correct_count + 1;
    else
        matched(i,:)
        mis_matched = [mis_matched; old_matched(i,1:3)];
        if (~isempty(hidden_seeds{1}))
            if (ismember(matched(i,1), hidden_seeds{1}(:,2)))
                incorrect_hidden = incorrect_hidden + 1;
            end
        end
        if (~isempty(hidden_seeds{2}))
            if (ismember(matched(i,2), hidden_seeds{2}(:,2)))
                incorrect_hidden = incorrect_hidden + 1;
            end
        end
        if (~isempty(hidden_seeds{3}))
            if (ismember(matched(i,3), hidden_seeds{3}(:,2)))
                incorrect_hidden = incorrect_hidden + 1;
            end
        end
        incorrect_count = incorrect_count + 1;
%         if (matched(i,4) == -1)
%             network_wrong = network_wrong + 1;
%         elseif (matched(i,4) == -2)
%             outer_seeds_wrong = outer_seeds_wrong + 1;
%         elseif (matched(i,4) == -3)
%             outer_loops_wrong = outer_loops_wrong + 1;
%         else
%             normal_loops_wrong = normal_loops_wrong + 1;
%         end
    end
end
total_hidden
incorrect_hidden
correct_count
incorrect_count
network_wrong
outer_seeds_wrong
outer_loops_wrong
normal_loops_wrong