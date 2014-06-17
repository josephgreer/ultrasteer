function [matched, mis_matched, mis_matched_seeds, incorrect_hidden, max_d] = remove_subgroups(matching, bpe_thresh, pts_2d, trs, ex, ey, xo, yo, focal)
% REMOVE_SUBGROUPS - removes subgroups
%   This function removes all the subgroups of size 1, as well as all the
%   groups with mismatched outer hidden seeds.  The matched seeds are
%   returned as matched, the mismatched outer hidden seeds as
%   mis_matched_seeds, and the seeds above the BPE threshold as
%   mis_matched.  incorrect_hidden contains the mismatched outer hidden
%   seeds and max_d is the BPE threshold.


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
    dead_end_seeds = setdiff(mis_matched_seeds(:,1), mis_matched_seeds(:,4));
end

% remove all seeds that are connected to the mismatched outer seeds
connect_ind = 1;
while (~isempty(find(connect_ind == 1)))
%     connect_ind = logical(ismember(matched_temp(:,1), mis_matched_seeds(:,1)) + ismember(matched_temp(:,2), mis_matched_seeds(:,2)) + ismember(matched_temp(:,3), mis_matched_seeds(:,3)) + ismember(matched_temp(:,4), mis_matched_seeds(:,4)) + ismember(matched_temp(:,1), mis_matched_seeds(:,4)) + ismember(matched_temp(:,4), mis_matched_seeds(:,1)));
    connect_ind = logical(ismember(matched_temp(:,1), mis_matched_seeds(:,1)) + ismember(matched_temp(:,4), mis_matched_seeds(:,4)) + ismember(matched_temp(:,1), mis_matched_seeds(:,4)) + ismember(matched_temp(:,4), mis_matched_seeds(:,1)));
    match_ind = logical(ones(size(connect_ind, 1), 1) - connect_ind);
    
    mis_matched_seeds = [mis_matched_seeds; matched_temp(connect_ind,:)];
    matched_temp = matched_temp(match_ind, :);   
end

% remove loops of length 1
matched = [];
mis_matched = [];
max_d = -inf;


% & ~ismember(matched_temp(i,1), hidden_seeds{1}) & ~ismember(matched_temp(i,2), hidden_seeds{2}) & ~ismember(matched_temp(i,3), hidden_seeds{3}) & ~ismember(matched_temp(i,4), hidden_seeds{4})
match_ind = find(matched_temp(:,1) - matched_temp(:,4) == 0  & ~ismember(matched_temp(:,1), hidden_seeds{1}) & ~ismember(matched_temp(:,2), hidden_seeds{2}) & ~ismember(matched_temp(:,3), hidden_seeds{3}) & ~ismember(matched_temp(:,4), hidden_seeds{4}));
mis_ind = find(matched_temp(:,1) - matched_temp(:,4) ~= 0  | ismember(matched_temp(:,1), hidden_seeds{1}) | ismember(matched_temp(:,2), hidden_seeds{2}) | ismember(matched_temp(:,3), hidden_seeds{3}) | ismember(matched_temp(:,4), hidden_seeds{4}));

matched = matched_temp(match_ind, :);
mis_matched = matched_temp(mis_ind, :);

errs = calculate_bpe(pts_2d,matched,trs,ex,ey,xo,yo,focal);
max_d = 2*max(errs);

% for i = 1:size(matched_temp,1)
%     [pt_3d, d_left, d_array_left] = sm_recon_core2({pts_2d{1}(matched_temp(i,1),:);pts_2d{2}(matched_temp(i,2),:);pts_2d{3}(matched_temp(i,3),:)}, trs, ex, ey, xo, yo, focal);    
% 	% backproject to 3 images
%     errs = 0;
%     for k = 1:3 
%         pt2d = sm_backproj_general(pt_3d,trs{k},ex(k),ey(k),xo(k),yo(k),focal(k));
%         p2dtmp = pt2d - pts_2d{k}(matched(:,k),:);
%         errs = errs + sqrt(p2dtmp(:,1).*p2dtmp(:,1) + p2dtmp(:,2).*p2dtmp(:,2));
%     end
%     
%     [pt_3d, d_right, d_array_right] = sm_recon_core2({pts_2d{1}(matched_temp(i,4),:);pts_2d{2}(matched_temp(i,2),:);pts_2d{3}(matched_temp(i,3),:)}, trs, ex, ey, xo, yo, focal);    
%     if (matched_temp(i,1) - matched_temp(i,4) == 0)
% %     if (matched_temp(i,1) - matched_temp(i,4) == 0 & ~ismember(matched_temp(i,1), hidden_seeds{1}) & ~ismember(matched_temp(i,2), hidden_seeds{2}) & ~ismember(matched_temp(i,3), hidden_seeds{3}) & ~ismember(matched_temp(i,4), hidden_seeds{4}) & d < bpe_thresh)
%         if (max(d_left, d_right) > max_d)
%             max_d = max(d_left, d_right);
%         end
%         matched = [matched; matched_temp(i,:)];
%     else
%         mis_matched = [mis_matched; matched_temp(i,:)];
%     end
% end
