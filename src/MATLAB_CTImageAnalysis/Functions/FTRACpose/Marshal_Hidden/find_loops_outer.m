function [matched, loops, mis_matched_seeds] = find_loops_outer(matching)

[matched, mis_matched, mis_matched_seeds, incorrect_hidden] = remove_subgroups(matching);

n_seeds_mis = size(mis_matched,1);
id = zeros(n_seeds_mis, 1);
loops = {};
loop_count = 1;

for i = 1:n_seeds_mis
    if (id(i) == 1)
        continue;
    end
    
    loops{loop_count} = mis_matched(i,:);
    id(i) = 1;
    
    connect_ind = 1;
    while (~isempty(find(connect_ind == 1)))
        mis_matched_other = mis_matched(logical(ones(size(id,1),1) - id),:);
        
        connect_ind = logical(ismember(mis_matched_other(:,1), loops{loop_count}(:,1)) + ismember(mis_matched_other(:,2), loops{loop_count}(:,2)) + ismember(mis_matched_other(:,3), loops{loop_count}(:,3)) + ismember(mis_matched_other(:,4), loops{loop_count}(:,4)) + ismember(mis_matched_other(:,1), loops{loop_count}(:,4)) + ismember(mis_matched_other(:,4), loops{loop_count}(:,1)));
        loops{loop_count} = [loops{loop_count}; mis_matched_other(connect_ind,:)];
        id_ind = ismember(mis_matched, loops{loop_count}, 'rows');
        
        id(id_ind) = 1;        
    end
    loop_count = loop_count + 1;
end

for i = 1:length(loops)
    loops{i} = loops{i}(:,1:3);
end

mis_matched_seeds = mis_matched_seeds(:,1:3);
if (~isempty(matched))
    matched = [matched(:,1:3) -ones(size(matched, 1), 1)];
end