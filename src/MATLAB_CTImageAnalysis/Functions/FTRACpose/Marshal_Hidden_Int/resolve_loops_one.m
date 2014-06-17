function [matched, mis_matched] = resolve_loops_one(loop, bpe_thresh, all_cost, current_matched, pts_2d, trs, ex, ey, xo, yo, focal)
% RESOLVE_LOOPS_ONE - resolves a single loop
%   This function resolves a single loop and returns the matched seeds as
%   matched, and the ones that are above the BPE threshold as mis_matched.

matched = [];
mis_matched = [];

combinations = sm_comb_all(loop);
cost = sm_comb_all_cost2(combinations,pts_2d,trs,ex,ey,xo,yo,focal);
k = size(loop, 1);

% There are different handling methods for different sized loops.  When the
% loop is small (<7), we can do a brute force. 
if k < 7
    if k <= 5
        match = [sm_combset_match(cost,k,combinations),zeros(k,1)];
    else
        match = [sm_combset_match_bypart(cost,k,5,combinations),zeros(k,1)];
    end
else
    pts_2d_new = cell(length(pts_2d),1);
    for j = 1:3
        pts_2d_new{j} = pts_2d{j}(loop(:,j),:);
    end

    for j = 4:length(pts_2d)
        pts_2d_new{j} = pts_2d{j};
    end

    hidden_seeds = cell(3,1);
    match = break_down_loops(pts_2d_new, pts_2d_new, trs, ex, ey, xo, yo, focal, size(loop, 1), hidden_seeds);

    for j = 1:3
        temp = loop(:,j);
        match(:,j) = temp(match(:,j)); 
    end
end

d = calculate_bpe(pts_2d,match,trs,ex,ey,xo,yo,focal);
% [pt_3d, d, d_array] = sm_recon_general_2({pts_2d{1}(match(:,1),:);pts_2d{2}(match(:,2),:);pts_2d{3}(match(:,3),:)},trs,ex,ey,xo,yo,focal);

mis_ind = find(d > bpe_thresh);
match_ind = find(d <= bpe_thresh);

if (~isempty(mis_ind))
    mis_matched = [mis_matched; [match(mis_ind,1:3) d(mis_ind,:)]];
    matched = [matched; match(match_ind,:)];
else
    matched = [matched; match];
end

% loops_all = [];
% for j = i + 1:length(loops)
%     loops_all = [loops_all; loops{j}];
% end

current_matched = [current_matched; match(:,1:3)];

if (~isempty(matched))
    matched = matched(:,1:3);
end