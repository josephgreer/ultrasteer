function [matched, mis_matched, mis_matched_seeds, incorrect_hidden] = remove_subgroups(matching)

% find seeds that are hidden in each image
hidden_seeds = cell(4,1);
for i = 1:4
    temp = matching(:,i);
    temp = sort(temp);
    prev = 0;
    for j = 1:length(temp)
        if (temp(j) == prev)
            hidden_seeds{i} = [hidden_seeds{i} temp(j)];
        end
        prev = temp(j);
    end
end

% remove matchings with mismatched hidden seeds
incorrect_hidden = setxor(hidden_seeds{1}, hidden_seeds{4});
n_seeds = size(matching, 1);

mis_ind = or(ismember(matching(:,1), incorrect_hidden), ismember(matching(:,4), incorrect_hidden));
match_ind = logical(ones(n_seeds, 1) - mis_ind);

matched_temp = matching(match_ind, :);
mis_matched_seeds = matching(mis_ind, :);
dead_end_seeds = setdiff(mis_matched_seeds(:,1), mis_matched_seeds(:,4));

while (~isempty(dead_end_seeds))
    mis_ind = logical(ismember(matched_temp(:,1), dead_end_seeds) + ismember(matched_temp(:,4), dead_end_seeds));
    match_ind = logical(ones(size(mis_ind, 1), 1) - mis_ind);
    
    mis_matched_seeds = [mis_matched_seeds; matched_temp(mis_ind, :)];
    matched_temp = matched_temp(match_ind, :);
    dead_end_seeds = setdiff(mis_matched_seeds(:,1), mis_matched_seeds(:,4))
end

% remove all seeds that are connected to the mismatched outer seeds
connect_ind = 1;
while (~isempty(find(connect_ind == 1)))
    connect_ind = logical(ismember(matched_temp(:,1), mis_matched_seeds(:,1)) + ismember(matched_temp(:,2), mis_matched_seeds(:,2)) + ismember(matched_temp(:,3), mis_matched_seeds(:,3)) + ismember(matched_temp(:,4), mis_matched_seeds(:,4)) + ismember(matched_temp(:,1), mis_matched_seeds(:,4)) + ismember(matched_temp(:,4), mis_matched_seeds(:,1)));
    match_ind = logical(ones(size(connect_ind, 1), 1) - connect_ind);
    
    mis_matched_seeds = [mis_matched_seeds; matched_temp(connect_ind,:)];
    matched_temp = matched_temp(match_ind, :);   
end

% remove loops of length 1
matched = [];
mis_matched = [];
for i = 1:size(matched_temp,1)
    if (matched_temp(i,1) - matched_temp(i,4) == 0 && ~ismember(matched_temp(i,1), hidden_seeds{1}) && ~ismember(matched_temp(i,2), hidden_seeds{2}) && ~ismember(matched_temp(i,3), hidden_seeds{3}) && ~ismember(matched_temp(i,4), hidden_seeds{4}))
        matched = [matched; matched_temp(i,:)];
    else
        mis_matched = [mis_matched; matched_temp(i,:)];
    end
end