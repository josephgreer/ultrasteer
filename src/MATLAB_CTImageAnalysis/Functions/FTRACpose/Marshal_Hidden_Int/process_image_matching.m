function [correct_matched, matched, bad_loops, bpe_sum] = process_image_matching(matched, loops, mis_matched_seeds, bpe_thresh, cost, pts_2d, trs, ex, ey, xo, yo, focal)
% PROCESS_IMAGE_MATCHING - processing the matching loops
%   This processing the matching loops that are greater than size 1.  It
%   returns the correct matching as matched, bad_loops contains the seeds
%   we ae unable to match, and bpe_sum is the sum of all the BPE.

bad_loops = {};
loop_count = 1;

% resolve the loops
if (~isempty(loops))
    for i = 1:length(loops)
        [matched_sub mis_matched_sub] = resolve_loops_one(loops{i}, bpe_thresh, cost, matched(:,1:3), pts_2d, trs, ex, ey, xo, yo, focal);
        if (~isempty(matched_sub) && isempty(mis_matched_sub))
            matched = [matched(:,1:3); matched_sub];
        end

        if (~isempty(mis_matched_sub))
            bad_loops{loop_count} = loops{i};
            loop_count = loop_count + 1;
        end     
    end
end

correct_matched = matched;
mis_matched = [];

% resolve mismatched outer hidden seeds
while (~isempty(mis_matched_seeds))
    [matched_sub loops_sub mis_matched_seeds] = resolve_outer_hidden_seeds(pts_2d, trs, ex, ey, xo, yo, focal, mis_matched_seeds);
    if (~isempty(matched_sub))
        matched = [matched(:,1:3); matched_sub];
    end

    if (~isempty(loops_sub))
        [matched_sub, mis_matched_sub] = resolve_loops(loops_sub, bpe_thresh * 100, cost, matched(:,1:3), pts_2d, trs, ex, ey, xo, yo, focal);
        if (~isempty(matched_sub))
            matched = [matched(:,1:3); matched_sub];
        end

        if (~isempty(mis_matched_sub))
            mis_matched = [mis_matched; mis_matched_sub];
        end                
    end
end

bpe_sum = sum(calculate_bpe(pts_2d,matched,trs,ex,ey,xo,yo,focal));