function [matched, loops, mis_matched_seeds, bpe_thresh] = find_loops(matching, cost, pts_2d, pts_2d_hidden, trs, ex, ey, xo, yo, focal, hidden_seeds, total_seeds)
% FIND_LOOPS - find the loops from the matching provided
%   This function find the loops from the given matching provided.  It
%   returns the loops of size one as matched, the bigger loops as loops,
%   and the unresolved loops (due to high BPE or mismatched outer hidden
%   seeds) as mis_matched_seeds.  We also return bpe_thresh, which is our
%   threshold for whether a matching is incorrect based upon BPE.  

% [pt_3d, d, d_array] = sm_recon_general_2({pts_2d,trs,ex,ey,xo,yo,focal)

[matched, mis_matched, mis_matched_seeds, incorrect_hidden, bpe_thresh] = remove_subgroups(matching, 1, pts_2d, trs, ex, ey, xo, yo, focal);

% matched_sub = [mis_matched; mis_matched_seeds];
% mis_matched_group = [mis_matched; mis_matched_seeds];

% mis_matched_seeds = [mis_matched; mis_matched_seeds];

% img_tot = match_remaining(pts_2d, pts_2d_hidden, cost, node_edges, flow, bpe_thresh, trs, ex, ey, xo, yo, focal, matched, mis_matched_seeds, hidden_seeds, total_seeds);

if (~isempty(matched))
    matched_seeds = cell(3,1);
    for i = 1:3
        matched_seeds{i} = matched(:,i);
    end
end

% img_tot = match_remaining(pts_2d, pts_2d_hidden, trs, ex, ey, xo, yo, focal, matched_seeds, hidden_seeds, size(matching, 1));
% 
% [matched, mis_matched, mis_matched_seeds, incorrect_hidden] = remove_subgroups(img_tot);
% matched_sub = [mis_matched; mis_matched_seeds];
% mis_matched_group = [mis_matched; mis_matched_seeds];

if (~isempty(matched) & ~isempty(mis_matched) & ~isempty(mis_matched_seeds))
        temp_cost = current_cost(cost, [matched(:,1:3); mis_matched(:,1:3); mis_matched_seeds(:,1:3)]);
end

% % loop network construction here
% while (~isempty(matched_sub) & ~isempty(mis_matched_group))
%     temp_seeds = [mis_matched; mis_matched_seeds];
%     seeds_tot = size(temp_seeds, 1);
% 
%     pts_2d_passed = cell(length(pts_2d),1);
%     for i = 1:3
%         pts_2d_passed{i} = pts_2d{i}(unique(temp_seeds(:,i)),:); 
%     end
%     
%     for i = 4:length(pts_2d)
%         pts_2d_passed{i} = pts_2d{i};
%     end
%     
%     img_tot = match_remaining(pts_2d_passed, trs, ex, ey, xo, yo, focal, seeds_tot);
% 
%     for i = 1:3
%         temp = unique(temp_seeds(:,i));
%         img_tot(:,i) = temp(img_tot(:,i));
%     end
%     temp = unique(temp_seeds(:,1));
%     img_tot(:,4) = temp(img_tot(:,4));
% 
%     [matched_sub mis_matched, mis_matched_seeds] = remove_subgroups(img_tot);
%     mis_matched_group = [mis_matched; mis_matched_seeds];
%     matched = [matched; matched_sub];
% end

n_seeds_mis = size(mis_matched,1);
id = zeros(n_seeds_mis, 1);
loops = {};
loop_count = 1;

% search for loops
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

% for i = 1:length(loops)
%     loops{i} = loops{i}(:,1:3);
% end
% 
% mis_matched_seeds = mis_matched_seeds(:,1:3);
% if (~isempty(matched))
%     matched = [matched(:,1:3) -ones(size(matched, 1), 1)];
% end